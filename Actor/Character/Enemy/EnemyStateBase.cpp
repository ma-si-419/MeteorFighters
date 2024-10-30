#include "EnemyStateBase.h"
#include "EnemyStateHitAttack.h"
#include "CharacterBase.h"
#include "Enemy.h"
#include "GameManager.h"
#include "Attack.h"

namespace
{
	constexpr float kDamageCutRate = 0.2f;

	const std::map<CharacterBase::AttackHitKind, CharacterBase::HitReactionKind> kEnemyStateHitKindMap =
	{
		{CharacterBase::AttackHitKind::kLow,CharacterBase::HitReactionKind::kLow},
		{CharacterBase::AttackHitKind::kMiddle,CharacterBase::HitReactionKind::kMiddle},
		{CharacterBase::AttackHitKind::kUpBurst,CharacterBase::HitReactionKind::kUpBurst},
		{CharacterBase::AttackHitKind::kDownBurst,CharacterBase::HitReactionKind::kDownBurst},
		{CharacterBase::AttackHitKind::kFarBurst,CharacterBase::HitReactionKind::kFarBurst},
		{CharacterBase::AttackHitKind::kBottomStan,CharacterBase::HitReactionKind::kBottomStan},
		{CharacterBase::AttackHitKind::kMiddleStan,CharacterBase::HitReactionKind::kMiddleStan}
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
	
	CharacterBase::HitReactionKind kind = kEnemyStateHitKindMap.at(status.attackHitKind);

	int damage = status.damage;
	//ガード時であれば
	if (stateKind == CharacterStateKind::kGuard)
	{
		//基本的にガード状態にする
		kind = CharacterBase::HitReactionKind::kGuard;

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
			kind = CharacterBase::HitReactionKind::kGuardBreak;
		}
		//投げ攻撃にぶつかった場合
		else if (status.attackKind == CharacterBase::AttackKind::kThrow)
		{
			//ガード関係なく状態遷移する
			kind = static_cast<CharacterBase::HitReactionKind>(status.attackHitKind);
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
