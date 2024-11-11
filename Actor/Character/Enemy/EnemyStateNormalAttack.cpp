#include "EnemyStateNormalAttack.h"
#include "EnemyStateIdle.h"
#include "DxLib.h"

EnemyStateNormalAttack::EnemyStateNormalAttack(std::shared_ptr<Enemy> enemy):
	EnemyStateBase(enemy)
{
}

void EnemyStateNormalAttack::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kNormalAttack;
}

void EnemyStateNormalAttack::Update()
{

	m_time++;



#ifdef _DEBUG

	DrawString(0, 32, "EnemyState:NormalAttack", GetColor(255, 255, 255));

#endif // _DEBUG
}

void EnemyStateNormalAttack::Exit()
{
}

void EnemyStateNormalAttack::OnCollide(std::shared_ptr<Collidable> collider)
{
}
