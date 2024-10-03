#include "EnemyStateButtonBashing.h"
#include "DxLib.h"

void EnemyStateButtonBashing::Enter()
{
	m_kind = CharacterStateKind::kButtonBashing;
	m_pNextState = shared_from_this();
}

void EnemyStateButtonBashing::Update()
{
#ifdef _DEBUG

	DrawString(0, 32, "EnemyState:ButtonBashing", GetColor(255, 255, 255));

	m_time++;

	if (m_time > 60)
	{
		ChangeState(CharacterStateKind::kIdle);
	}

#endif // _DEBUG
}

void EnemyStateButtonBashing::Exit()
{
}
