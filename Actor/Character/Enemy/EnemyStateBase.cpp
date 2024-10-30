#include "EnemyStateBase.h"
#include "EnemyStateHitAttack.h"
#include "Enemy.h"
#include "GameManager.h"
#include "Attack.h"

namespace
{
	constexpr float kDamageCutRate = 0.2f;

	const std::map<CharacterBase::AttackHitKind, EnemyStateHitAttack::HitKind> kEnemyStateHitKindMap =
	{
		{CharacterBase::AttackHitKind::kLow,EnemyStateHitAttack::HitKind::kLow},
		{CharacterBase::AttackHitKind::kMiddle,EnemyStateHitAttack::HitKind::kMiddle},
		{CharacterBase::AttackHitKind::kUpBurst,EnemyStateHitAttack::HitKind::kUpBurst},
		{CharacterBase::AttackHitKind::kDownBurst,EnemyStateHitAttack::HitKind::kDownBurst},
		{CharacterBase::AttackHitKind::kFarBurst,EnemyStateHitAttack::HitKind::kFarBurst},
		{CharacterBase::AttackHitKind::kBottomStan,EnemyStateHitAttack::HitKind::kBottomStan},
		{CharacterBase::AttackHitKind::kMiddleStan,EnemyStateHitAttack::HitKind::kMiddleStan}
	};
}

EnemyStateBase::EnemyStateBase(std::shared_ptr<Enemy> enemy) :
	m_pEnemy(enemy)
{
}

void EnemyStateBase::SetEnemyVelo(MyEngine::Vector3 moveVec)
{
	m_pEnemy->m_rigidbody.SetVelo(moveVec);
}

void EnemyStateBase::HitAttack(std::shared_ptr<Attack> attack, CharacterStateBase::CharacterStateKind stateKind)
{
	std::shared_ptr<EnemyStateHitAttack> nextState = std::make_shared<EnemyStateHitAttack>(m_pEnemy);

	//攻撃のステータス
	auto status = attack->GetStatus();
	
	EnemyStateHitAttack::HitKind kind = kEnemyStateHitKindMap.at(status.attackHitKind);

	int damage = status.damage;
	//ガード時であれば
	if (stateKind == CharacterStateKind::kGuard)
	{
		//基本的にガード状態にする
		kind = EnemyStateHitAttack::HitKind::kGuard;

		//打撃系は0ダメージ
		if (status.attackKind == CharacterBase::AttackKind::kPhysical)
		{
			damage = 0;
		}
		//気弾系の攻撃であればダメージカット
		else if (status.attackKind == CharacterBase::AttackKind::kEnergy ||
			status.attackKind == CharacterBase::AttackKind::kBeam)
		{
			damage = static_cast<int>(damage * kDamageCutRate);
		}
		//必殺技のぶつかると演出に切り替わる技に当たった場合
		else if (status.attackKind == CharacterBase::AttackKind::kRush ||
			status.attackKind == CharacterBase::AttackKind::kAssault)
		{
			//ダメージはない
			damage = 0;
			//ガードブレイク状態にする
			kind = EnemyStateHitAttack::HitKind::kGuardBreak;
		}
		//投げ攻撃にぶつかった場合
		else if (status.attackKind == CharacterBase::AttackKind::kThrow)
		{
			//ガード関係なく状態遷移する
			kind = static_cast<EnemyStateHitAttack::HitKind>(status.attackHitKind);
		}
	}
	nextState->HitAttack(kind);

	//体力を減らす
	m_pEnemy->SubHp(damage);

	ChangeState(nextState);
}

MyEngine::Vector3 EnemyStateBase::GetPlayerPos()
{
	return m_pEnemy->m_pGameManager->GetPlayerPos();
}
