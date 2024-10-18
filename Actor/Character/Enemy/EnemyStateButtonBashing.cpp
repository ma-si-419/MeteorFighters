#include "EnemyStateButtonBashing.h"
#include "EnemyStateIdle.h"
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
		std::shared_ptr<EnemyStateIdle> next = std::make_shared<EnemyStateIdle>();

		ChangeState(next);
	}

#endif // _DEBUG
}

void EnemyStateButtonBashing::Exit()
{
}

void EnemyStateButtonBashing::OnCollide(std::shared_ptr<Collidable> collider)
{
}
