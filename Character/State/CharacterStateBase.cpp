#include "CharacterStateBase.h"
#include "CharacterStateHitAttack.h"
#include "CharacterStateButtonBashing.h"
#include "Character.h"
#include "GameManagerBase.h"
#include "TutorialManager.h"
#include "Attack.h"
#include "Input.h"
#include "Effect.h"
#include "EffectManager.h"
#include <cmath>

namespace
{
	constexpr float kDamageCutRate = 0.2f;

	const std::map<Character::AttackHitKind, Character::HitReactionKind> kHitKindMap =
	{
		{Character::AttackHitKind::kLow,Character::HitReactionKind::kLow},
		{Character::AttackHitKind::kMiddle,Character::HitReactionKind::kMiddle},
		{Character::AttackHitKind::kWeakUpBurst,Character::HitReactionKind::kWeakUpBurst},
		{Character::AttackHitKind::kUpBurst,Character::HitReactionKind::kUpBurst},
		{Character::AttackHitKind::kDownBurst,Character::HitReactionKind::kDownBurst},
		{Character::AttackHitKind::kFarBurst,Character::HitReactionKind::kFarBurst},
		{Character::AttackHitKind::kBottomStan,Character::HitReactionKind::kBottomStan},
		{Character::AttackHitKind::kMiddleStan,Character::HitReactionKind::kMiddleStan}
	};

	constexpr int kHitEffectLifeTime = 30;
}

CharacterStateBase::CharacterStateBase(std::shared_ptr<Character> character)
{
	m_pCharacter = character;
	m_pManager = m_pCharacter->m_pBattleManager;
}

void CharacterStateBase::OnCollide(std::shared_ptr<Collidable> collider)
{
	//1P���̏���
	if (m_pCharacter->GetTag() == ObjectTag::kOnePlayer)
	{
		//2P�̍U���ɓ���������
		if (collider->GetTag() == ObjectTag::kTwoPlayerAttack)
		{
			auto attack = std::static_pointer_cast<Attack>(collider);

			auto status = attack->GetStatus();

			CharacterStateBase::HitAttack(attack, GetKind());
		}
	}
	//2P���̏���
	else if (m_pCharacter->GetTag() == ObjectTag::kTwoPlayer)
	{
		//2P�̍U���ɓ���������
		if (collider->GetTag() == ObjectTag::kOnePlayerAttack)
		{
			auto attack = std::static_pointer_cast<Attack>(collider);

			auto status = attack->GetStatus();

			CharacterStateBase::HitAttack(attack, GetKind());
		}
	}
}

void CharacterStateBase::UpdateCommon()
{
	if (m_kind != CharacterStateKind::kButtonBashing)
	{
		//�����State���{�^���A�ł�������ButtonBashingState�ɕύX����
		if (static_cast<CharacterStateKind>(m_pManager->GetTargetState(m_pCharacter)) == CharacterStateKind::kButtonBashing)
		{
			auto next = std::make_shared<CharacterStateButtonBashing>(m_pCharacter);

			ChangeState(next);
		}
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

std::shared_ptr<MyEngine::InputData> CharacterStateBase::GetCharacterInput()
{
	return m_pCharacter->m_input;
}

void CharacterStateBase::SetCharacterVelo(MyEngine::Vector3 velo)
{
	m_pCharacter->m_rigidbody.SetVelo(velo);
}

void CharacterStateBase::SetCharacterPos(MyEngine::Vector3 pos)
{
	m_pCharacter->m_rigidbody.SetPos(pos);
}

void CharacterStateBase::HitAttack(std::shared_ptr<Attack> attack, CharacterStateBase::CharacterStateKind stateKind)
{
	std::shared_ptr<CharacterStateHitAttack> nextState = std::make_shared<CharacterStateHitAttack>(m_pCharacter);

	//�U���̃X�e�[�^�X
	auto status = attack->GetStatus();

	Character::HitReactionKind kind = kHitKindMap.at(status.attackHitKind);

	int damage = status.damage;
	//�K�[�h���ł����
	if (stateKind == CharacterStateKind::kGuard)
	{
		//��{�I�ɃK�[�h��Ԃɂ���
		kind = Character::HitReactionKind::kGuard;

		//�K�[�h�`���[�g���A�����N���A������
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kGuard));

		//�Ō��n��0�_���[�W
		if (status.attackKind == Character::AttackKind::kPhysical)
		{
			damage = 0;
		}
		//�C�e�n�̍U���ł���΃_���[�W�J�b�g
		else if (status.attackKind == Character::AttackKind::kEnergy ||
			status.attackKind == Character::AttackKind::kBeam)
		{
			damage = static_cast<int>(damage * kDamageCutRate);
		}
		//�K�E�Z�̂Ԃ���Ɖ��o�ɐ؂�ւ��Z�ɓ��������ꍇ
		else if (status.attackKind == Character::AttackKind::kRush ||
			status.attackKind == Character::AttackKind::kAssault)
		{
			//�_���[�W�͂Ȃ�
			damage = 0;
			//�K�[�h�u���C�N��Ԃɂ���
			kind = Character::HitReactionKind::kGuardBreak;
		}
		//�����U���ɂԂ������ꍇ
		else if (status.attackKind == Character::AttackKind::kThrow)
		{
			//�K�[�h�֌W�Ȃ���ԑJ�ڂ���
			kind = static_cast<Character::HitReactionKind>(status.attackHitKind);
		}
	}
	//�K�[�h���łȂ����
	else
	{
		//�q�b�g�G�t�F�N�g���Đ�����
		Effect::EffectKind effectKind = Effect::EffectKind::kEnergyHit;
		if (status.attackKind == Character::AttackKind::kEnergy)
		{
			effectKind = Effect::EffectKind::kEnergyHit;
		}
		else if (status.attackKind == Character::AttackKind::kPhysical)
		{
			effectKind = Effect::EffectKind::kLowHit;
		}

		std::shared_ptr<Effect> hitEffect = std::make_shared<Effect>(effectKind);
		hitEffect->SetLifeTime(kHitEffectLifeTime);
		m_pCharacter->m_pBattleManager->GetEffectManagerPointer()->Entry(hitEffect, m_pCharacter->GetPos());

		MyEngine::Vector3 rotation;

		float vX = attack->GetPos().x - m_pCharacter->GetPos().x;
		float vZ = attack->GetPos().z - m_pCharacter->GetPos().z;

		rotation.y = std::atan2f(vX, vZ);

		hitEffect->SetRotation(rotation);
	}
	nextState->HitAttack(static_cast<int>(kind));

	//�̗͂����炷
	m_pCharacter->SubHp(damage);

	ChangeState(nextState);
}

void CharacterStateBase::SuccessTutorial(int tutorialNumber)
{
	//�Q�[�����[�h���`���[�g���A���łȂ���Ζ�������
	if (m_pCharacter->m_pBattleManager->GetGameKind() != GameManagerBase::GameKind::kTutorial) return;

	auto manager = std::dynamic_pointer_cast<TutorialManager>(m_pCharacter->m_pBattleManager);

	auto clearKind = static_cast<TutorialManager::TutorialSuccessKind>(tutorialNumber);

	manager->SuccessTutorial(clearKind);
}
