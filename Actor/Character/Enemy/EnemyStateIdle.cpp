#include "EnemyStateIdle.h"
#include "EnemyStateHitAttack.h"
#include "DxLib.h"
#include "Collidable.h"
#include "Attack.h"
#include "Enemy.h"

EnemyStateIdle::EnemyStateIdle(std::shared_ptr<Enemy> enemy):
	EnemyStateBase(enemy)
{
}

void EnemyStateIdle::Enter()
{
	m_kind = CharacterStateKind::kIdle;
	m_pNextState = shared_from_this();
	m_pEnemy->ChangeAnim(CharacterBase::AnimKind::kGuard,true);
	printfDx("アイドルだよー\n");
}

void EnemyStateIdle::Update()
{
#ifdef _DEBUG

	DrawString(0, 32, "EnemyState:Idle", GetColor(255, 255, 255));

	m_time++;

	if (m_time > 60)
	{
		//int rand = GetRand(6) + 1;

		//std::shared_ptr<EnemyStateIdle> next = std::make_shared<EnemyStateIdle>(m_pEnemy);

		//ChangeState(next);
		//if (rand == 1)
		//{
		//	ChangeState(CharacterStateKind::kMove);
		//}
		//else if (rand == 2)
		//{
		//	ChangeState(CharacterStateKind::kGuard);
		//}
		//else if (rand == 3)
		//{
		//	ChangeState(CharacterStateKind::kAttack);
		//}
		//else if (rand == 4)
		//{
		//	ChangeState(CharacterStateKind::kRush);
		//}
		//else if (rand == 5)
		//{
		//	ChangeState(CharacterStateKind::kHitAttack);
		//}
		//else if (rand == 6)
		//{
		//	ChangeState(CharacterStateKind::kButtonBashing);
		//}
	}
#endif // _DEBUG

	SetEnemyVelo(MyEngine::Vector3(0,0,0));

}

void EnemyStateIdle::Exit()
{
}

void EnemyStateIdle::OnCollide(std::shared_ptr<Collidable> collider)
{
	if (collider->GetTag() == ObjectTag::kPlayerAttack)
	{
		auto attack = std::static_pointer_cast<Attack>(collider);

		auto status = attack->GetStatus();

		HitAttack(attack, GetKind());
	}
}
