#include "EnemyStateAttack.h"
#include "EnemyStateIdle.h"
#include "DxLib.h"

void EnemyStateAttack::Enter()
{
	m_kind = CharacterStateKind::kAttack;
	m_pNextState = shared_from_this();
}

void EnemyStateAttack::Update()
{
#ifdef _DEBUG

	DrawString(0, 32, "EnemyState:Attack", GetColor(255, 255, 255));

	m_time++;

	if (m_time > 60)
	{
		std::shared_ptr<EnemyStateIdle> next = std::make_shared<EnemyStateIdle>();

		ChangeState(next);
	}

#endif // _DEBUG
}

void EnemyStateAttack::Exit()
{
}

void EnemyStateAttack::OnCollide(std::shared_ptr<Collidable> collider)
{
}
