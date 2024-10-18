#include "EnemyStateHitAttack.h"
#include "EnemyStateIdle.h"
#include "DxLib.h"


void EnemyStateHitAttack::Enter()
{
	m_kind = CharacterStateKind::kHitAttack;
	m_pNextState = shared_from_this();
}

void EnemyStateHitAttack::Update()
{
#ifdef _DEBUG

	DrawString(0, 32, "EnemyState:HitAttack", GetColor(255, 255, 255));

	m_time++;

	if (m_time > 60)
	{
		std::shared_ptr<EnemyStateIdle> next = std::make_shared<EnemyStateIdle>();

		ChangeState(next);
	}
#endif // _DEBUG
}

void EnemyStateHitAttack::Exit()
{
}

void EnemyStateHitAttack::OnCollide(std::shared_ptr<Collidable> collider)
{
}
