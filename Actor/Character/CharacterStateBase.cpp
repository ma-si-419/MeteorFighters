#include "CharacterStateBase.h"
#include "CharacterStateHitAttack.h"
#include "CharacterBase.h"
#include "GameManager.h"
#include "Attack.h"
#include "Input.h"

namespace
{
	constexpr float kDamageCutRate = 0.2f;

	const std::map<CharacterBase::AttackHitKind, CharacterBase::HitReactionKind> kHitKindMap =
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

CharacterStateBase::CharacterStateBase(std::shared_ptr<CharacterBase> character)
{
	m_pCharacter = character;
	//���삳��邩�ǂ�����ݒ�
	if (character->GetTag() == ObjectTag::kOnePlayer)
	{
		m_isPlayer = true;
	}
	else
	{
		m_isPlayer = false;
	}
}

void CharacterStateBase::ChangeState(std::shared_ptr<CharacterStateBase> nextState)
{
	//�I������
	Exit();

	//����State�̐ݒ�
	m_pNextState = nextState;

	//����State�̊J�n����
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

void CharacterStateBase::StopMoveCamera()
{
	m_pCharacter->m_pGameManager->StopMoveCamera();
}

void CharacterStateBase::StartMoveCamera()
{
	m_pCharacter->m_pGameManager->StartMoveCamera();
}

void CharacterStateBase::ShakeCamera(int time)
{
	m_pCharacter->m_pGameManager->ShakeCamera(time);
}

void CharacterStateBase::HitAttack(std::shared_ptr<Attack> attack, CharacterStateBase::CharacterStateKind stateKind)
{
	std::shared_ptr<CharacterStateHitAttack> nextState = std::make_shared<CharacterStateHitAttack>(m_pCharacter);

	//�U���̃X�e�[�^�X
	auto status = attack->GetStatus();

	CharacterBase::HitReactionKind kind = kHitKindMap.at(status.attackHitKind);

	int damage = status.damage;
	//�K�[�h���ł����
	if (stateKind == CharacterStateKind::kGuard)
	{
		//��{�I�ɃK�[�h��Ԃɂ���
		kind = CharacterBase::HitReactionKind::kGuard;

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
			kind = CharacterBase::HitReactionKind::kGuardBreak;
		}
		//�����U���ɂԂ������ꍇ
		else if (status.attackKind == CharacterBase::AttackKind::kThrow)
		{
			//�K�[�h�֌W�Ȃ���ԑJ�ڂ���
			kind = static_cast<CharacterBase::HitReactionKind>(status.attackHitKind);
		}
	}
	nextState->HitAttack(static_cast<int>(kind));

	//�̗͂����炷
	m_pCharacter->SubHp(damage);

	ChangeState(nextState);
}