#include "EnemyStateAttack.h"
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
		ChangeState(CharacterStateKind::kIdle);
	}

#endif // _DEBUG
}

void EnemyStateAttack::Exit()
{
}
