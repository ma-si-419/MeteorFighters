#include "PlayerStateJump.h"
#include "PlayerStateIdle.h"
#include "PlayerStateMove.h"
#include "PlayerStateDash.h"
#include "PlayerStateNormalAttack.h"
#include "Player.h"
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

PlayerStateJump::PlayerStateJump(std::shared_ptr<Player> player) :
	PlayerStateBase(player),
	m_attackKey("empty"),
	m_attackButtonPushTime(0)
{
}

void PlayerStateJump::StartJump(MyEngine::Vector3 moveVec)
{
	m_moveVec = (moveVec * kMoveVecRate) + MyEngine::Vector3(0.0f, GameSceneConstant::kJumpPower, 0.0f);
}

void PlayerStateJump::StartJump()
{
	StartJump(MyEngine::Vector3(0.0f, 0.0f, 0.0f));
}

void PlayerStateJump::Enter()
{
	//移動ベクトルが上を向いていなければ
	if (m_moveVec.y <= 0)
	{
		//ジャンプ中にする
		m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kJumping, true);
	}
	//移動ベクトルが上を向いていれば
	else
	{
		m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kJumpStart, false);
	}

	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kJump;

}

void PlayerStateJump::Update()
{
	//ジャンプを始めるアニメーションをしていて
	if (m_pPlayer->GetPlayAnimKind() == CharacterBase::AnimKind::kJumpStart)
	{
		//アニメーションが終わったら
		if (m_pPlayer->IsEndAnim())
		{
			//ジャンプ中のアニメーションに変化させる
			m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kJumping, true);
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
	if (input.IsTrigger("RB"))
	{
		auto next = std::make_shared<PlayerStateMove>(m_pPlayer);

		ChangeState(next);
	}
	//攻撃ボタンが押されていないときに
	if (m_attackKey == "empty")
	{
		//格闘ボタンが押された時
		if (input.IsPress("X"))
		{
			m_attackKey = "X";
		}
		else if (input.IsPress("Y"))
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
		if (input.IsRelease(m_attackKey) ||
			m_attackButtonPushTime > GameSceneConstant::kChargeAttackTime)
		{
			//チャージされていたかどうか判定
			bool isCharge = m_attackButtonPushTime > GameSceneConstant::kChargeAttackTime;
			//次のStateのポインタ作成
			auto next = std::make_shared<PlayerStateNormalAttack>(m_pPlayer);

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

			next->SetAttack(m_attackKey, attackName);
			//StateをAttackに変更する
			ChangeState(next);
			return;
		}
	}

	//ダッシュボタンが押されたら
	if (input.IsTrigger("A"))
	{

		MyEngine::Vector3 leftStickDir(input.GetStickInfo().leftStickX, 0, -input.GetStickInfo().leftStickY);

		leftStickDir = leftStickDir.Normalize();

		//敵との距離からダッシュかステップか判断する
		//(ステップかダッシュかの判定はDashStateの中でも行う)
		//(ここではMPを消費するかしないか、DashStateにはいるかどうかを判断する)
		if ((GetEnemyPos() - m_pPlayer->GetPos()).Length() > GameSceneConstant::kNearLange)
		{
			//遠かった場合Mpを消費してダッシュする
			if (m_pPlayer->SubMp(GameSceneConstant::kDashCost))
			{
				auto next = std::make_shared<PlayerStateDash>(m_pPlayer);

				if (leftStickDir.SqLength() < 0.01f)
				{
					next->SetMoveDir(MyEngine::Vector3(0,0,1));
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
			auto next = std::make_shared<PlayerStateDash>(m_pPlayer);

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

	SetPlayerVelo(m_moveVec);

	//地上にいるなら
	if (m_pPlayer->IsGround() && m_moveVec.y < 0)
	{
		//アイドル状態に戻る
		auto next = std::make_shared<PlayerStateIdle>(m_pPlayer);

		next->SetEndAnim(static_cast<int>(CharacterBase::AnimKind::kJumpEnd),kLandingTime);

		ChangeState(next);

		return;
	}
}

void PlayerStateJump::Exit()
{
}

void PlayerStateJump::OnCollide(std::shared_ptr<Collidable> collider)
{
}
