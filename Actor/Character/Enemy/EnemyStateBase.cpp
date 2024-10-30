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

	//�U���̃X�e�[�^�X
	auto status = attack->GetStatus();
	
	EnemyStateHitAttack::HitKind kind = kEnemyStateHitKindMap.at(status.attackHitKind);

	int damage = status.damage;
	//�K�[�h���ł����
	if (stateKind == CharacterStateKind::kGuard)
	{
		//��{�I�ɃK�[�h��Ԃɂ���
		kind = EnemyStateHitAttack::HitKind::kGuard;

		//�Ō��n��0�_���[�W
		if (status.attackKind == CharacterBase::AttackKind::kPhysical)
		{
			damage = 0;
		}
		//�C�e�n�̍U���ł���΃_���[�W�J�b�g
		else if (status.attackKind == CharacterBase::AttackKind::kEnergy ||
			status.attackKind == CharacterBase::AttackKind::kBeam)
		{
			damage = static_cast<int>(damage * kDamageCutRate);
		}
		//�K�E�Z�̂Ԃ���Ɖ��o�ɐ؂�ւ��Z�ɓ��������ꍇ
		else if (status.attackKind == CharacterBase::AttackKind::kRush ||
			status.attackKind == CharacterBase::AttackKind::kAssault)
		{
			//�_���[�W�͂Ȃ�
			damage = 0;
			//�K�[�h�u���C�N��Ԃɂ���
			kind = EnemyStateHitAttack::HitKind::kGuardBreak;
		}
		//�����U���ɂԂ������ꍇ
		else if (status.attackKind == CharacterBase::AttackKind::kThrow)
		{
			//�K�[�h�֌W�Ȃ���ԑJ�ڂ���
			kind = static_cast<EnemyStateHitAttack::HitKind>(status.attackHitKind);
		}
	}
	nextState->HitAttack(kind);

	//�̗͂����炷
	m_pEnemy->SubHp(damage);

	ChangeState(nextState);
}

MyEngine::Vector3 EnemyStateBase::GetPlayerPos()
{
	return m_pEnemy->m_pGameManager->GetPlayerPos();
}
