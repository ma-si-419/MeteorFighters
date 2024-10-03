#include "EnemyStateGuard.h"
#include "DxLib.h"

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
		ChangeState(CharacterStateKind::kIdle);
	}

#endif // _DEBUG
}

void EnemyStateGuard::Exit()
{
}
