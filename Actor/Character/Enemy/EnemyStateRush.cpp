#include "EnemyStateRush.h"
#include "EnemyStateIdle.h"
#include "DxLib.h"

EnemyStateRush::EnemyStateRush(std::shared_ptr<Enemy> enemy):
	EnemyStateBase(enemy)
{
}

void EnemyStateRush::Enter()
{
	m_kind = CharacterStateKind::kRush;
	m_pNextState = shared_from_this();
}

void EnemyStateRush::Update()
{
#ifdef _DEBUG

	DrawString(0, 32, "EnemyState:Rush", GetColor(255, 255, 255));

	m_time++;

	if (m_time > 60)
	{
		std::shared_ptr<EnemyStateIdle> next = std::make_shared<EnemyStateIdle>(m_pEnemy);

		ChangeState(next);
	}

#endif // _DEBUG
}

void EnemyStateRush::Exit()
{
}

void EnemyStateRush::OnCollide(std::shared_ptr<Collidable> collider)
{
}
