#include "CharacterStateBase.h"
#include "CharacterStateHitAttack.h"
#include "CharacterBase.h"
#include "GameManager.h"
#include "Attack.h"
#include "Input.h"
#include "Effect.h"
#include "EffectManager.h"
#include <cmath>

namespace
{
	constexpr float kDamageCutRate = 0.2f;

	const std::map<CharacterBase::AttackHitKind, CharacterBase::HitReactionKind> kHitKindMap =
	{
		{CharacterBase::AttackHitKind::kLow,CharacterBase::HitReactionKind::kLow},
		{CharacterBase::AttackHitKind::kMiddle,CharacterBase::HitReactionKind::kMiddle},
		{CharacterBase::AttackHitKind::kWeakUpBurst,CharacterBase::HitReactionKind::kWeakUpBurst},
		{CharacterBase::AttackHitKind::kUpBurst,CharacterBase::HitReactionKind::kUpBurst},
		{CharacterBase::AttackHitKind::kDownBurst,CharacterBase::HitReactionKind::kDownBurst},
		{CharacterBase::AttackHitKind::kFarBurst,CharacterBase::HitReactionKind::kFarBurst},
		{CharacterBase::AttackHitKind::kBottomStan,CharacterBase::HitReactionKind::kBottomStan},
		{CharacterBase::AttackHitKind::kMiddleStan,CharacterBase::HitReactionKind::kMiddleStan}
	};

	constexpr int kHitEffectLifeTime = 30;
}

CharacterStateBase::CharacterStateBase(std::shared_ptr<CharacterBase> character)
{
	m_pCharacter = character;
	//操作されるかどうかを設定
	if (character->GetTag() == ObjectTag::kOnePlayer)
	{
		m_isPlayer = true;
	}
	else
	{
		m_isPlayer = false;
	}
}

void CharacterStateBase::OnCollide(std::shared_ptr<Collidable> collider)
{
	//1P側の処理
	if (m_pCharacter->GetTag() == ObjectTag::kOnePlayer)
	{
		//2Pの攻撃に当たったら
		if (collider->GetTag() == ObjectTag::kTwoPlayerAttack)
		{
			auto attack = std::static_pointer_cast<Attack>(collider);

			auto status = attack->GetStatus();

			CharacterStateBase::HitAttack(attack, GetKind());
		}
	}
	//2P側の処理
	else if (m_pCharacter->GetTag() == ObjectTag::kTwoPlayer)
	{
		//2Pの攻撃に当たったら
		if (collider->GetTag() == ObjectTag::kOnePlayerAttack)
		{
			auto attack = std::static_pointer_cast<Attack>(collider);

			auto status = attack->GetStatus();

			CharacterStateBase::HitAttack(attack, GetKind());
		}
	}
}

void CharacterStateBase::ChangeState(std::shared_ptr<CharacterStateBase> nextState)
{
	//終了処理
	Exit();

	//次のStateの設定
	m_pNextState = nextState;

	//次のStateの開始処理
	m_pNextState->Enter();
}

MyEngine::Vector3 CharacterStateBase::GetTargetPos()
{
	return m_pCharacter->m_pGameManager->GetTargetPos(m_pCharacter);
}

MyEngine::Vector3 CharacterStateBase::GetTargetVelo()
{
	return m_pCharacter->m_pGameManager->GetTargetVelo(m_pCharacter);
}

int CharacterStateBase::GetTargetHitReaction()
{
	return static_cast<int>(m_pCharacter->m_pGameManager->GetTargetHitReaction(m_pCharacter));
}

void CharacterStateBase::SetCharacterVelo(MyEngine::Vector3 velo)
{
	m_pCharacter->m_rigidbody.SetVelo(velo);
}

void CharacterStateBase::SetCharacterPos(MyEngine::Vector3 pos)
{
	m_pCharacter->m_rigidbody.SetPos(pos);
}

MyEngine::Vector3 CharacterStateBase::GetTargetBackPos(float distance)
{
	return m_pCharacter->m_pGameManager->GetTargetBackPos(distance, m_pCharacter);
}

void CharacterStateBase::CreateAfterImage()
{
	m_pCharacter->CreateAfterImage();
}

int CharacterStateBase::GetAttackAnimKind(std::string animName)
{
	return static_cast<int>(m_pCharacter->GetAttackAnimKind(animName));
}

float CharacterStateBase::GetSpeed()
{
	return m_pCharacter->GetSpeed();
}

void CharacterStateBase::StopCameraCorrection()
{
	m_pCharacter->m_pGameManager->StopCameraCorrection();
}

void CharacterStateBase::StopCamera(int time)
{
	m_pCharacter->m_pGameManager->StopCamera(time);
}

void CharacterStateBase::StartCameraCorrection()
{
	m_pCharacter->m_pGameManager->StartCameraCorrection();
}

void CharacterStateBase::ShakeCamera(int time)
{
	m_pCharacter->m_pGameManager->ShakeCamera(time);
}

void CharacterStateBase::SwayCamera()
{
	m_pCharacter->m_pGameManager->SwayCamera();
}

void CharacterStateBase::HitAttack(std::shared_ptr<Attack> attack, CharacterStateBase::CharacterStateKind stateKind)
{
	std::shared_ptr<CharacterStateHitAttack> nextState = std::make_shared<CharacterStateHitAttack>(m_pCharacter);

	//攻撃のステータス
	auto status = attack->GetStatus();

	CharacterBase::HitReactionKind kind = kHitKindMap.at(status.attackHitKind);

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
	//ガード時でなければ
	else
	{
		//ヒットエフェクトを再生する
		Effect::EffectKind effectKind = Effect::EffectKind::kEnergyHit;
		if (status.attackKind == CharacterBase::AttackKind::kEnergy)
		{
			effectKind = Effect::EffectKind::kEnergyHit;
		}
		else if (status.attackKind == CharacterBase::AttackKind::kPhysical)
		{
			effectKind = Effect::EffectKind::kLowHit;
		}

		std::shared_ptr<Effect> hitEffect = std::make_shared<Effect>(effectKind);
		hitEffect->SetLifeTime(kHitEffectLifeTime);
		m_pCharacter->m_pGameManager->GetEffectManagerPointer()->Entry(hitEffect, m_pCharacter->GetPos());
		
		MyEngine::Vector3 rotation;

		float vX = attack->GetPos().x - m_pCharacter->GetPos().x;
		float vZ = attack->GetPos().z - m_pCharacter->GetPos().z;

		rotation.y = std::atan2f(vX,vZ);

		hitEffect->SetRotation(rotation);
	}
	nextState->HitAttack(static_cast<int>(kind));

	//体力を減らす
	m_pCharacter->SubHp(damage);

	ChangeState(nextState);
}

void CharacterStateBase::EntryEffect(std::shared_ptr<Effect> effect)
{
	m_pCharacter->m_pGameManager->EntryEffect(effect);
}

void CharacterStateBase::ExitEffect(std::shared_ptr<Effect> effect)
{
	m_pCharacter->m_pGameManager->ExitEffect(effect);
}
