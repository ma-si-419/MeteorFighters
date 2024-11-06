#include "PlayerStateDash.h"
#include "PlayerStateIdle.h"
#include "Player.h"
#include <cmath>

namespace
{
	const MyEngine::Vector3 kDefaultMoveDir(0, 0, 1);

	constexpr float kDashSpeed = 4.0f;

	constexpr int kDashTime = 15;

	constexpr float kAnimationBlendSpeed = 0.12f;
}

PlayerStateDash::PlayerStateDash(std::shared_ptr<Player> player) :
	PlayerStateBase(player),
	m_moveDir(kDefaultMoveDir)
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
		m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kGroundDash, true,kAnimationBlendSpeed);
	}
	else
	{
		m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kSkyDash, true,kAnimationBlendSpeed);
	}

	SetPlayerBlur(true);

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

		MyEngine::Vector3 toTarget = (GetEnemyPos() - m_pPlayer->GetPos()).Normalize();

		dir = (dir * (1.0 - frontRate)) + toTarget * frontRate;
	}

	MyEngine::Vector3 velo = dir * kDashSpeed;

	//移動方向にスピードをかける
	SetPlayerVelo(velo);

	//移動方向を向く
	m_pPlayer->SetFrontPos(m_pPlayer->GetPos() + velo);

	//一定時間移動したらアイドルStateに戻る
	if (m_time > kDashTime)
	{
		auto next = std::make_shared<PlayerStateIdle>(m_pPlayer);

		ChangeState(next);
		return;
	}
}

void PlayerStateDash::Exit()
{
	SetPlayerBlur(false);
}

void PlayerStateDash::OnCollide(std::shared_ptr<Collidable> collider)
{
}
