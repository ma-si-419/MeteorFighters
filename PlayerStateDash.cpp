#include "PlayerStateDash.h"
#include "PlayerStateIdle.h"
#include "Player.h"
#include "GameSceneConstant.h"
#include <cmath>

namespace
{
	const MyEngine::Vector3 kDefaultMoveDir(0, 0, 1);

	constexpr float kDashSpeed = 4.0f;

	constexpr float kDodgeSpeed = 1.5f;

	constexpr int kDashTime = 15;

	constexpr int kDodgeTime = 20;

	constexpr float kAnimationBlendSpeed = 0.12f;

	constexpr float kAnimPlaySpeed = 1.5f;
}

PlayerStateDash::PlayerStateDash(std::shared_ptr<Player> player) :
	PlayerStateBase(player),
	m_moveDir(kDefaultMoveDir),
	m_isDodge(false)
{
}

void PlayerStateDash::SetMoveDir(MyEngine::Vector3 moveDir)
{
	m_moveDir = moveDir;
}

void PlayerStateDash::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = GetKind();
	if (m_pPlayer->IsGround())
	{
		m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kGroundDash, true, kAnimationBlendSpeed);
	}
	else
	{
		m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kSkyDash, true, kAnimationBlendSpeed);
	}

	//敵の距離が近ければ
	if ((GetEnemyPos() - m_pPlayer->GetPos()).Length() < GameSceneConstant::kNearLange)
	{
		//回避行動をする
		m_isDodge = true;

		//向きによってアニメーションを変える
		//回避の場合は移動方向を前後左右の四通りに限定する(斜めをなくす)
		if (m_moveDir.z >= 0.5f)
		{
			m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kDodgeFront, false, kAnimationBlendSpeed);
			m_moveDir = MyEngine::Vector3(0.0f, 0.0f, 1.0f);
		}
		else if (m_moveDir.z < -0.5f)
		{
			m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kDodgeBack, false, kAnimationBlendSpeed);
			m_moveDir = MyEngine::Vector3(0.0f, 0.0f, -1.0f);
		}
		else if (m_moveDir.x >= 0.5f)
		{
			m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kDodgeRight, false, kAnimationBlendSpeed);
			m_moveDir = MyEngine::Vector3(1.0f, 0.0f, 0.0f);
		}
		else
		{
			m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kDodgeLeft, false, kAnimationBlendSpeed);
			m_moveDir = MyEngine::Vector3(-1.0f, 0.0f, 0.0f);
		}
	}

	m_pPlayer->SetAnimPlaySpeed(kAnimPlaySpeed);
}

void PlayerStateDash::Update()
{
	m_time++;

	//エネミーの方向に移動方向を回転させる
	float vX = GetEnemyPos().x - m_pPlayer->GetPos().x;
	float vZ = GetEnemyPos().z - m_pPlayer->GetPos().z;

	float yAngle = std::atan2f(vX, vZ);

	MyEngine::Vector3 rotation(0.0f, yAngle, 0.0f);

	MATRIX mat = rotation.GetRotationMat();

	MyEngine::Vector3 dir = m_moveDir.MatTransform(mat);

	//空中にいて前入力されていたら
	if (m_moveDir.z > 0 && !m_pPlayer->IsGround())
	{
		float frontRate = m_moveDir.z;

		LocalPos enemyLocal;

		enemyLocal.SetCenterPos(GetEnemyPos());

		enemyLocal.SetLocalPos(MyEngine::Vector3(0.0f,0.0f,GameSceneConstant::kCharacterRadius));

		enemyLocal.SetFrontPos(m_pPlayer->GetPos());

		MyEngine::Vector3 targetPos = GetEnemyPos() + enemyLocal.GetWorldPos();

		MyEngine::Vector3 toTarget = (GetEnemyPos() - m_pPlayer->GetPos()).Normalize();

		dir = (dir * (1.0 - frontRate)) + toTarget * frontRate;
	}

	MyEngine::Vector3 velo;

	if (m_isDodge)
	{
		velo = dir * kDodgeSpeed;
	}
	else
	{
		velo = dir * kDashSpeed;
	}

	//移動方向にスピードをかける
	SetPlayerVelo(velo);

	if (m_isDodge)
	{
		//回避なら敵の方を向く
		m_pPlayer->LookTarget(true);
	}
	else
	{
		//ダッシュなら移動方向を向く
		m_pPlayer->SetFrontPos(m_pPlayer->GetPos() + velo);
	}

	//回避の場合
	if (m_isDodge)
	{
		//残像を作成するタイミングになったら
		if (static_cast<int>(m_time) % GameSceneConstant::kAfterImageCreateTime == 0)
		{
			CreateAfterImage();
		}

		//一定時間移動したらアイドルStateに戻る
		if (m_time > kDodgeTime)
		{
			auto next = std::make_shared<PlayerStateIdle>(m_pPlayer);

			ChangeState(next);
			return;
		}
	}
	//ダッシュの場合
	else
	{
		//一定時間移動したらアイドルStateに戻る
		if (m_time > kDashTime)
		{
			auto next = std::make_shared<PlayerStateIdle>(m_pPlayer);

			ChangeState(next);
			return;
		}
	}
#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:DashW", GetColor(255, 255, 255));

#endif // _DEBUG
}

void PlayerStateDash::Exit()
{
	m_pPlayer->SetAnimPlaySpeed();
}

void PlayerStateDash::OnCollide(std::shared_ptr<Collidable> collider)
{
}
