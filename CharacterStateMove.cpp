#include "CharacterStateMove.h"
#include "CharacterStateIdle.h"
#include "CharacterStateDash.h"
#include "CharacterStateRush.h"
#include "CharacterStateJump.h"
#include "CharacterStateCharge.h"
#include "CharacterStateNormalAttack.h"
#include "CharacterStateJump.h"
#include "GameSceneConstant.h"
#include "DxLib.h"
#include "Input.h"
#include "CharacterBase.h"
#include <cmath>

namespace
{
	//移動速度を遅くする割合
	constexpr float kSlowMoveSpeed = 0.6f;
	//さらに移動速度を遅くする距離
	constexpr float kNearestLange = 10.0f;
}

CharacterStateMove::CharacterStateMove(std::shared_ptr<CharacterBase> character) :
	CharacterStateBase(character),
	m_attackButtonPushTime(0),
	m_attackKey("empty"),
	m_isFloat(false),
	m_gravityPower(0.0f),
	m_isLastGround(false)
{
	m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kSkyIdle, true);
}

void CharacterStateMove::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kMove;
	if (m_pCharacter->IsGround())
	{
		m_isFloat = false;
	}
	else
	{
		m_isFloat = true;
	}
}

void CharacterStateMove::Update()
{
	//プレイヤーからエネミーへのベクトル
	MyEngine::Vector3 playerToTarget = GetTargetPos() - m_pCharacter->GetPos();

	//インプットを管理しているクラスの参照
	auto input = GetCharacterInput();
	//移動ベクトル
	MyEngine::Vector3 velo;
	//移動方向ベクトル
	MyEngine::Vector3 dir;

	MyEngine::Vector3 inputDir;


	//スティックの情報取得
	auto stick = input->GetStickInfo();

	//左スティックの傾き取得
	inputDir = MyEngine::Vector3(stick.leftStickX, 0, -stick.leftStickY);

	//移動ベクトルが0じゃなければ
	if (inputDir.SqLength() > 0.001)
	{

		inputDir = inputDir.Normalize();

		//移動方向
		dir = inputDir;

		//エネミーの方向に移動方向を回転させる
		float vX = GetTargetPos().x - m_pCharacter->GetPos().x;
		float vZ = GetTargetPos().z - m_pCharacter->GetPos().z;

		float yAngle = std::atan2f(vX, vZ);

		MyEngine::Vector3 rotation;

		rotation = MyEngine::Vector3(0.0f, yAngle, 0.0f);

		MATRIX mat = rotation.GetRotationMat();

		dir = dir.MatTransform(mat);

		MyEngine::Vector3 toTarget = (GetTargetPos() - m_pCharacter->GetPos());
		MyEngine::Vector3 toTargetDir = toTarget.Normalize();

		//空中にいて前入力されていたら
		if (inputDir.z > 0 && m_isFloat)
		{
			float frontRate = inputDir.z;

			dir = dir * (1.0f - frontRate) + toTargetDir * frontRate;
		}

		//移動速度
		float speed = GetSpeed();

		//一定以上敵に近くなると移動速度を遅くする
		if (toTarget.Length() < GameSceneConstant::kNearLange)
		{
			speed *= kSlowMoveSpeed;
		}

		//移動方向にスピードをかける
		velo = dir * speed;
	}

	//宙に浮いていない場合
	if (!m_isFloat)
	{
		m_gravityPower += GameSceneConstant::kSkyGravityPower;

		if (m_gravityPower < GameSceneConstant::kMaxFallSpeed)
		{
			m_gravityPower = GameSceneConstant::kMaxFallSpeed;
		}

		velo.y += m_gravityPower;

		//地上にいなく
		if (!m_pCharacter->IsGround())
		{

			//プレイ中のアニメーションがジャンプ中でなければ
			if (!(m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kJumping))
			{
				//アニメーションを変更する
				m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kJumping, true);
			}
		}
	}

	//ダッシュボタンが押されたら
	if (input->IsTrigger("A"))
	{
		//一緒にレフトショルダーも押されていたら
		if (input->IsPushTrigger(false))
		{
			//ダッシュのコストがあれば
			if (m_pCharacter->SubMp(GameSceneConstant::kDashCost))
			{
				//突撃状態に移行する
				auto next = std::make_shared<CharacterStateRush>(m_pCharacter);

				next->SetMoveDir(inputDir);

				ChangeState(next);
				return;
			}
		}

		//敵との距離からダッシュかステップか判断する
		//(ステップかダッシュかの判定はDashStateの中でも行う)
		//(ここではMPを消費するかしないか、DashStateにはいるかどうかを判断する)
		if ((GetTargetPos() - m_pCharacter->GetPos()).Length() > GameSceneConstant::kNearLange)
		{
			//遠かった場合Mpを消費してダッシュする
			if (m_pCharacter->SubMp(GameSceneConstant::kDashCost))
			{
				auto next = std::make_shared<CharacterStateDash>(m_pCharacter);

				next->SetMoveDir(inputDir);

				ChangeState(next);
				return;
			}
		}
		//敵との距離が近い場合
		else
		{
			//MPを消費せずにステップをする
			auto next = std::make_shared<CharacterStateDash>(m_pCharacter);

			next->SetMoveDir(inputDir);

			ChangeState(next);
			return;
		}
	}


	//地上で
	if (m_pCharacter->IsGround())
	{

		//ジャンプボタンが押されたら
		if (input->IsTrigger("RB"))
		{
			//ジャンプStateに移行する
			auto next = std::make_shared<CharacterStateJump>(m_pCharacter);

			//今の移動ベクトルを渡す
			next->StartJump(velo);

			//ステートを変更する
			ChangeState(next);

			return;
		}

		m_gravityPower = GameSceneConstant::kGroundGravityPower;

		m_isFloat = false;

		MyEngine::Vector3 groundVelo = velo;

		groundVelo.y = 0;

		//移動していれば
		if (groundVelo.SqLength() > 0.01f)
		{
			//プレイ中のアニメーションが地上移動でなければ
			if (!(m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kRun))
			{
				//アニメーションを変更する
				m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kRun, true);
			}

			auto frontPos = m_pCharacter->GetPos() + groundVelo;

			frontPos.y = m_pCharacter->GetPos().y;

			m_pCharacter->SetFrontPos(frontPos);
		}
		//移動していなければ
		else
		{
			//アイドル状態に戻る
			std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);

			ChangeState(next);
		}

		//今のフレーム地上にいたかどうかを保存する
		m_isLastGround = true;

	}
	//空中で
	else
	{

		//上昇ボタンが押されたら
		if (input->IsPress("RB"))
		{
			velo.y = GetSpeed();
			m_isFloat = true;
		}
		//下降ボタンが押されたら
		else if (input->IsPushTrigger(true))
		{
			velo.y = -GetSpeed();

			//プレイ中のアニメーションがジャンプ中でなければ
			if (!(m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kJumping))
			{
				//アニメーションを変更する
				m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kJumping, true);
			}

		}

		//前のフレームから空中にいれば
		if (!m_isLastGround)
		{
			//敵の方向を向く(Y座標はプレイヤーと同じ座標にする)
			auto frontPos = GetTargetPos();

			frontPos.y = m_pCharacter->GetPos().y;

			m_pCharacter->SetFrontPos(frontPos);

			//上昇しているか上下移動をしていない場合
			if (velo.y >= 0)
			{
				//プレイ中のアニメーションが空中待機でなければ
				if (!(m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kSkyIdle))
				{
					//アニメーションを変更する
					m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kSkyIdle, true);
				}
			}
		}

		//今のフレーム空中にいたことを保存する
		m_isLastGround = false;

	}

	//攻撃ボタンが押されていないときに
	if (m_attackKey == "empty")
	{
		//格闘ボタンが押された時
		if (input->IsPress("X"))
		{
			m_attackKey = "X";
		}
		else if (input->IsPress("Y"))
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
		if (input->IsRelease(m_attackKey) ||
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
					if (input->GetStickInfo().leftStickY < -GameSceneConstant::kPhysicalAttackStickPower)
					{
						attackName = "UpCharge";
					}
					//スティックを下に傾けていたら
					else if (input->GetStickInfo().leftStickY > GameSceneConstant::kPhysicalAttackStickPower)
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

	//一定時間レフトショルダーボタンが押されたら
	if (input->GetPushTriggerTime(false) > GameSceneConstant::kChargeStateChangeTime)
	{
		//次のStateのポインタ作成
		auto next = std::make_shared<CharacterStateCharge>(m_pCharacter);
		//StateをChargeに変更する
		ChangeState(next);
		return;
	}

	//移動していなかったら
	if (velo.SqLength() < 0.01f)
	{
		//アイドル状態に戻る
		std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		ChangeState(next);
	}
	//移動ベクトルを設定する
	SetCharacterVelo(velo);

#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Move", GetColor(255, 255, 255));

#endif // _DEBUG

}
void CharacterStateMove::Exit()
{
}