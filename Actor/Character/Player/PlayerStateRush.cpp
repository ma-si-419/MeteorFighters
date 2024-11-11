#include "PlayerStateRush.h"
#include "PlayerStateDash.h"
#include "PlayerStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"
#include "GameSceneConstant.h"
#include <cmath>

namespace
{
	constexpr float kMoveVerticalPower = 0.03f;

	constexpr float kVerticalMoveMax = 0.5f;
	constexpr float kVerticalMoveMin = -0.5f;
}

PlayerStateRush::PlayerStateRush(std::shared_ptr<Player> player) :
	PlayerStateBase(player),
	m_isRushEnemy(false)
{
}

void PlayerStateRush::SetMoveDir(MyEngine::Vector3 dir)
{
	//エネミーの方向に移動方向を回転させる
	float vX = GetEnemyPos().x - m_pPlayer->GetPos().x;
	float vZ = GetEnemyPos().z - m_pPlayer->GetPos().z;

	float yAngle = std::atan2f(vX, vZ);

	MyEngine::Vector3 rotation;

	rotation = MyEngine::Vector3(0.0f, yAngle, 0.0f);

	MATRIX mat = rotation.GetRotationMat();

	m_moveDir = dir.MatTransform(mat);
}

void PlayerStateRush::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kRush;
	m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kSkyDash, true);
}

void PlayerStateRush::Update()
{
	auto& input = MyEngine::Input::GetInstance();

	//スティックの傾き
	MyEngine::Vector3 stickDir(input.GetStickInfo().leftStickX, 0, -input.GetStickInfo().leftStickY);

	//Aボタンが押されたときに
	if (input.IsTrigger("A"))
	{
		//レフトショルダーも押されていたら
		if (input.IsPushTrigger(false))
		{
			//敵の近くまで向かう突撃状態になる
			m_isRushEnemy = true;
			m_rushTargetPos = GetEnemyBackPos(GameSceneConstant::kEnemyBackPosDistance);
		}
		//押されていなければ
		else
		{
			//スティックの傾きを見る
			if (stickDir.SqLength() > 0.001f)
			{
				//ダッシュ状態に移行する
				auto next = std::make_shared<PlayerStateDash>(m_pPlayer);

				next->SetMoveDir(stickDir.Normalize());

				ChangeState(next);

				return;
			}
			//スティックが傾いていなければ
			else
			{
				//アイドル状態に戻る
				auto next = std::make_shared<PlayerStateIdle>(m_pPlayer);

				ChangeState(next);

				return;
			}
		}
	}


	float speed = m_pPlayer->GetSpeed() * GameSceneConstant::kRushMoveSpeedRate;

	if (stickDir.SqLength() > 0.001f)
	{
		//スティックの入力ベクトルが移動ベクトルの真反対であればステートをアイドルに戻す
		if (-stickDir.x == m_moveDir.x && -stickDir.z == m_moveDir.z)
		{
			auto next = std::make_shared<PlayerStateIdle>(m_pPlayer);

			ChangeState(next);

			return;
		}

		//移動の方向ベクトル
		m_moveDir = stickDir = stickDir.Normalize();

	}
	//エネミーの方向に移動方向を回転させる
	float vX = GetEnemyPos().x - m_pPlayer->GetPos().x;
	float vZ = GetEnemyPos().z - m_pPlayer->GetPos().z;

	float yAngle = std::atan2f(vX, vZ);

	MyEngine::Vector3 rotation;

	rotation = MyEngine::Vector3(0.0f, yAngle, 0.0f);

	MATRIX mat = rotation.GetRotationMat();

	if (input.IsPress("RB"))
	{
		m_moveDir.y += kMoveVerticalPower;
	}
	if (input.IsPushTrigger(true))
	{
		m_moveDir.y -= kMoveVerticalPower;
	}

	//上にも下にも傾きすぎないように
	m_moveDir.y = std::fmax(m_moveDir.y, kVerticalMoveMin);
	m_moveDir.y = std::fmin(m_moveDir.y, kVerticalMoveMax);



	MyEngine::Vector3 dir = m_moveDir.MatTransform(mat);

	MyEngine::Vector3 velo = dir * speed;

	//敵の背後に向かうフラグが立っていれば
	if (m_isRushEnemy)
	{
		MyEngine::Vector3 toTarget = m_rushTargetPos - m_pPlayer->GetPos();

		dir = toTarget.Normalize();

		if (toTarget.Length() > GameSceneConstant::kRushSpeed)
		{
			velo = dir * GameSceneConstant::kRushSpeed;
		}
		else
		{
			velo = dir * toTarget.Length();
		}

		//一定距離までは目的座標を更新し続ける
		if ((GetEnemyBackPos(GameSceneConstant::kEnemyBackPosDistance) - m_pPlayer->GetPos()).Length() > GameSceneConstant::kCameraMoveDistance)
		{
			m_rushTargetPos = GetEnemyBackPos(GameSceneConstant::kEnemyBackPosDistance);
		}
		//一定距離まで近づいたら
		else
		{
			//カメラを高速移動させる
			m_pPlayer->StartFastCameraMove();

			//さらに近くまで近づいたら
			if (toTarget.Length() < GameSceneConstant::kEnemyBackPosDistance)
			{
				auto next = std::make_shared<PlayerStateIdle>(m_pPlayer);

				ChangeState(next);
				return;
			}
		}
	}



	SetPlayerVelo(velo);

	m_pPlayer->SetFrontPos(m_pPlayer->GetPos() + velo);

#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Rush", GetColor(255, 255, 255));

#endif // _DEBUG

}
void PlayerStateRush::Exit()
{
}

void PlayerStateRush::OnCollide(std::shared_ptr<Collidable> collider)
{
}
