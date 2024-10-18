#include "EnemyStateMove.h"
#include "EnemyStateIdle.h"
#include "DxLib.h"

void EnemyStateMove::Enter()
{
	m_kind = CharacterStateKind::kMove;
	m_pNextState = shared_from_this();
	
}

void EnemyStateMove::Update()
{
#ifdef _DEBUG

	DrawString(0, 32, "EnemyState:Move", GetColor(255, 255, 255));

	m_time++;

	if (m_time > 60)
	{
		std::shared_ptr<EnemyStateIdle> next = std::make_shared<EnemyStateIdle>();

		ChangeState(next);
	}
#endif // _DEBUG
}

void EnemyStateMove::Exit()
{
}

void EnemyStateMove::OnCollide(std::shared_ptr<Collidable> collider)
{
}
