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
	//スティック操作がされていないとき上下移動
	constexpr float kMoveVerticalPower = 0.03f;
	//スティック操作がされているときの上下移動
	constexpr float kOnStickVerticalPower = 200.0f;

	constexpr float kVerticalMoveMax = 0.5f;
	constexpr float kVerticalMoveMin = -0.5f;

	constexpr float kCurveScale = -80;

	//移動ベクトルがカーブする距離
	constexpr float kCurveDistance = 300.0f;
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
		MyEngine::Vector3 moveDir = stickDir;

		if (input.IsPress("RB"))
		{
			moveDir.y += kOnStickVerticalPower;
		}
		if (input.IsPushTrigger(true))
		{
			moveDir.y -= kOnStickVerticalPower;
		}

		//エネミーの方向に移動方向を回転させる
		float vX = GetEnemyPos().x - m_pPlayer->GetPos().x;
		float vZ = GetEnemyPos().z - m_pPlayer->GetPos().z;

		float yAngle = std::atan2f(vX, vZ);

		MyEngine::Vector3 rotation;

		rotation = MyEngine::Vector3(0.0f, yAngle, 0.0f);

		MATRIX mat = rotation.GetRotationMat();

		if (stickDir.z > 0)
		{
			MyEngine::Vector3 toTarget;////////////////////

			moveDir = moveDir * (1.0 - stickDir.Normalize().z) + ;
		}

		//MyEngine::Vector3 dir = m_moveDir.MatTransform(mat);
		m_moveDir = moveDir.Normalize().MatTransform(mat);
	}
	else
	{
		if (input.IsPress("RB"))
		{
			m_moveDir.y += kMoveVerticalPower;
		}
		if (input.IsPushTrigger(true))
		{
			m_moveDir.y -= kMoveVerticalPower;
		}
	}

	//上にも下にも傾きすぎないように

	m_moveDir.y = std::fmax(m_moveDir.y, kVerticalMoveMin);
	m_moveDir.y = std::fmin(m_moveDir.y, kVerticalMoveMax);
	MyEngine::Vector3 velo = m_moveDir * speed;

	//敵の背後に向かうフラグが立っていれば
	if (m_isRushEnemy)
	{
		//障害物に当たらないようにする
		m_pPlayer->SetIsTrigger(true);

		MyEngine::Vector3 toTarget = m_rushTargetPos - m_pPlayer->GetPos();

		m_moveDir = toTarget.Normalize();

		//一定距離から移動方向を変えるため、距離の割合をとる
		float langeRate = toTarget.Length() / kCurveDistance;
		//カーブの割合
		float curveRate = 0.0f;

		//一定距離まで近づいたら
		if (langeRate < 1.0f)
		{
			//0から1の間で-1から+1にするために*2-1をする
			curveRate = std::sinf(langeRate * 2.0f - 1.0f);

			printfDx("%0.2f\n", langeRate);

			float yAngle = curveRate * kCurveScale;

			MyEngine::Vector3 rotation(0.0f, curveRate, 0.0f);

			MATRIX mat = rotation.GetRotationMat();

			m_moveDir = m_moveDir.MatTransform(mat);
		}

		if (toTarget.Length() > GameSceneConstant::kRushSpeed)
		{
			velo = m_moveDir * GameSceneConstant::kRushSpeed;
		}
		else
		{
			velo = m_moveDir * toTarget.Length();
		}

		//一定距離までは目的座標を更新し続ける
		if ((GetEnemyBackPos(GameSceneConstant::kEnemyBackPosDistance) - m_pPlayer->GetPos()).Length() > GameSceneConstant::kCameraMoveDistance)
		{
			m_rushTargetPos = GetEnemyBackPos(GameSceneConstant::kEnemyBackPosDistance);
			StopMoveCamera();
		}
		//一定距離まで近づいたら
		else
		{
			StartMoveCamera();

			//カメラを高速移動させる
			m_pPlayer->StartFastCameraMove();

			//さらに近くまで近づいたら
			if (toTarget.Length() < GameSceneConstant::kEnemyBackPosDistance)
			{
				auto next = std::make_shared<PlayerStateIdle>(m_pPlayer);

				ChangeState(next);

				//障害物に当たるようにする
				m_pPlayer->SetIsTrigger(false);

				//敵の方向を向く
				MyEngine::Vector3 nextPos = m_pPlayer->GetPos() + velo;

				MyEngine::Vector3 nextToTarget = GetEnemyPos() - nextPos;

				m_pPlayer->SetFrontPos(nextToTarget + m_pPlayer->GetPos());

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
