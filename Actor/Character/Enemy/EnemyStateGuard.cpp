#include "EnemyStateGuard.h"
#include "EnemyStateIdle.h"
#include "DxLib.h"

EnemyStateGuard::EnemyStateGuard(std::shared_ptr<Enemy> enemy):
	EnemyStateBase(enemy)
{
}

void EnemyStateGuard::Enter()
{
	m_kind = CharacterStateKind::kGuard;
	m_pNextState = shared_from_this();
}

void EnemyStateGuard::Update()
{
#ifdef _DEBUG

	DrawString(0, 32, "EnemyState:Guard", GetColor(255, 255, 255));

	m_time++;

	if (m_time > 60)
	{
		std::shared_ptr<EnemyStateIdle> next = std::make_shared<EnemyStateIdle>(m_pEnemy);

		ChangeState(next);
	}

#endif // _DEBUG
}

void EnemyStateGuard::Exit()
{
}

void EnemyStateGuard::OnCollide(std::shared_ptr<Collidable> collider)
{
}
