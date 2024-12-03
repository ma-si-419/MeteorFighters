#include "CharacterStateJump.h"
#include "CharacterStateIdle.h"
#include "CharacterStateMove.h"
#include "CharacterStateDash.h"
#include "CharacterStateNormalAttack.h"
#include "CharacterBase.h"
#include "CapsuleColliderData.h"
#include "Physics.h"
#include "GameSceneConstant.h"
#include "Input.h"

namespace
{
	//前後左右移動をどのくらい反映させるか
	constexpr float kMoveVecRate = 0.9f;

	//着地時の硬直時間
	constexpr int kLandingTime = 7;
}

CharacterStateJump::CharacterStateJump(std::shared_ptr<CharacterBase> character) :
	CharacterStateBase(character),
	m_attackKey("empty"),
	m_attackButtonPushTime(0)
{
}

void CharacterStateJump::StartJump(MyEngine::Vector3 moveVec)
{
	m_moveVec = (moveVec * kMoveVecRate) + MyEngine::Vector3(0.0f, GameSceneConstant::kJumpPower, 0.0f);
}

void CharacterStateJump::StartJump()
{
	StartJump(MyEngine::Vector3(0.0f, 0.0f, 0.0f));
}

void CharacterStateJump::Enter()
{
	//移動ベクトルが上を向いていなければ
	if (m_moveVec.y <= 0)
	{
		//ジャンプ中にする
		m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kJumping, true);
	}
	//移動ベクトルが上を向いていれば
	else
	{
		m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kJumpStart, false);
	}

	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kJump;

}

void CharacterStateJump::Update()
{
	//ジャンプを始めるアニメーションをしていて
	if (m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kJumpStart)
	{
		//アニメーションが終わったら
		if (m_pCharacter->IsEndAnim())
		{
			//ジャンプ中のアニメーションに変化させる
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kJumping, true);
		}
	}
	//重力をかける
	m_moveVec.y += GameSceneConstant::kSkyGravityPower;
	if (m_moveVec.y < GameSceneConstant::kMaxFallSpeed)
	{
		//落下速度が速くなりすぎないように
		m_moveVec.y = GameSceneConstant::kMaxFallSpeed;
	}

	auto& input = MyEngine::Input::GetInstance();

	//ジャンプボタンをもう一度押したら空中に止まる(ステートも変更する)
	if (m_isPlayer && input.IsTrigger("RB"))
	{
		auto next = std::make_shared<CharacterStateMove>(m_pCharacter);

		ChangeState(next);
	}
	//攻撃ボタンが押されていないときに
	if (m_attackKey == "empty")
	{
		//格闘ボタンが押された時
		if (m_isPlayer && input.IsPress("X"))
		{
			m_attackKey = "X";
		}
		else if (m_isPlayer && input.IsPress("Y"))
		{
			m_attackKey = "Y";
		}
	}
	//攻撃ボタンが押されていたら
	else
	{
		//押しているフレーム数を数える
		m_attackButtonPushTime++;

		//押していたボタンが離されたら
		if (m_isPlayer && input.IsRelease(m_attackKey) ||
			m_attackButtonPushTime > GameSceneConstant::kChargeAttackTime)
		{
			//チャージされていたかどうか判定
			bool isCharge = m_attackButtonPushTime > GameSceneConstant::kChargeAttackTime;
			//次のStateのポインタ作成
			auto next = std::make_shared<CharacterStateNormalAttack>(m_pCharacter);

			//何の攻撃を行うかをAttackStateに渡す
			std::string attackName = "empty";

			//チャージされていて
			if (isCharge)
			{
				//Xボタンが押されていて
				if (m_attackKey == "X")
				{
					//スティックを上に傾けていたら
					if (input.GetStickInfo().leftStickY < -GameSceneConstant::kPhysicalAttackStickPower)
					{
						attackName = "UpCharge";
					}
					//スティックを下に傾けていたら
					else if (input.GetStickInfo().leftStickY > GameSceneConstant::kPhysicalAttackStickPower)
					{
						attackName = "DownCharge";
					}
					//スティックを傾けていなければ
					else
					{
						attackName = "MiddleCharge";
					}
				}
				//Yボタンが押されていたら
				else if (m_attackKey == "Y")
				{
					attackName = "EnergyCharge";
				}
			}
			//チャージされていなければ
			else
			{
				if (m_attackKey == "X")
				{
					attackName = "Low1";
				}
				else if (m_attackKey == "Y")
				{
					attackName = "Energy1";
				}
			}

			//気弾攻撃ならば気力を減らす
			if (attackName == "Energy1" || attackName == "EnergyCharge")
			{
				//減らせなければ攻撃をセットしない
				if (m_pCharacter->SubMp(GameSceneConstant::kEnergyAttackCost))
				{
					next->SetAttack(m_attackKey, attackName);

					//StateをAttackに変更する
					ChangeState(next);
					return;
				}
			}
			else
			{
				next->SetAttack(m_attackKey, attackName);


				//StateをAttackに変更する
				ChangeState(next);
				return;
			}
		}
	}

	//ダッシュボタンが押されたら
	if (m_isPlayer && input.IsTrigger("A"))
	{

		MyEngine::Vector3 leftStickDir(input.GetStickInfo().leftStickX, 0, -input.GetStickInfo().leftStickY);

		leftStickDir = leftStickDir.Normalize();

		//敵との距離からダッシュかステップか判断する
		//(ステップかダッシュかの判定はDashStateの中でも行う)
		//(ここではMPを消費するかしないか、DashStateにはいるかどうかを判断する)
		if ((GetTargetPos() - m_pCharacter->GetPos()).Length() > GameSceneConstant::kNearLange)
		{
			//遠かった場合Mpを消費してダッシュする
			if (m_pCharacter->SubMp(GameSceneConstant::kDashCost))
			{
				auto next = std::make_shared<CharacterStateDash>(m_pCharacter);

				if (leftStickDir.SqLength() < 0.01f)
				{
					next->SetMoveDir(MyEngine::Vector3(0, 0, 1));
				}
				else
				{
					next->SetMoveDir(leftStickDir);
				}

				ChangeState(next);
				return;
			}
		}
		//敵との距離が近い場合
		else
		{
			//MPを消費せずにステップをする
			auto next = std::make_shared<CharacterStateDash>(m_pCharacter);

			if (leftStickDir.SqLength() < 0.01f)
			{
				next->SetMoveDir(MyEngine::Vector3(0, 0, 1));
			}
			else
			{
				next->SetMoveDir(leftStickDir);
			}

			ChangeState(next);
			return;
		}
	}

	SetCharacterVelo(m_moveVec);

	//地上にいるなら
	if (m_pCharacter->IsGround() && m_moveVec.y < 0)
	{
		//アイドル状態に戻る
		auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		next->SetEndAnim(static_cast<int>(CharacterBase::AnimKind::kJumpEnd), kLandingTime);

		ChangeState(next);

		return;
	}
}

void CharacterStateJump::Exit()
{
}