#include "EnemyStateRush.h"
#include "DxLib.h"

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
		ChangeState(CharacterStateKind::kIdle);
	}

#endif // _DEBUG
}

void EnemyStateRush::Exit()
{
}
