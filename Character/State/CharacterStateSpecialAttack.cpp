#include "CharacterStateSpecialAttack.h"
#include "CharacterStateIdle.h"
#include "Character.h"
#include "Effect.h"
#include "Attack.h"
#include "GameSceneConstant.h"
#include "TutorialManager.h"
#include <cassert>
#include <map>
#include <cmath>

namespace
{

	const std::map<Character::AttackKind, int> kAttackTimeMap =
	{
		{Character::AttackKind::kEnergy,90},
		{Character::AttackKind::kPhysical,90},
		{Character::AttackKind::kBeam,90},
		{Character::AttackKind::kRush,90},
		{Character::AttackKind::kThrow,30},
		{Character::AttackKind::kAssault,90}
	};

	const std::map<Character::AttackKind, Character::AttackHitKind> kHitKindMap =
	{
		{Character::AttackKind::kEnergy,Character::AttackHitKind::kWeakUpBurst},
		{Character::AttackKind::kPhysical,Character::AttackHitKind::kFarBurst},
		{Character::AttackKind::kBeam,Character::AttackHitKind::kWeakUpBurst},
		{Character::AttackKind::kRush,Character::AttackHitKind::kFarBurst},
		{Character::AttackKind::kThrow,Character::AttackHitKind::kFarBurst},
		{Character::AttackKind::kAssault,Character::AttackHitKind::kWeakUpBurst}
	};

	const std::map<Character::AttackKind, float> kAttackSpeedMap =
	{
		{Character::AttackKind::kEnergy,6.0f},
		{Character::AttackKind::kPhysical,0.0f},
		{Character::AttackKind::kBeam,6.0f},
		{Character::AttackKind::kRush,0.0f},
		{Character::AttackKind::kThrow,0.0f},
		{Character::AttackKind::kAssault,0.0f},
	};
}

CharacterStateSpecialAttack::CharacterStateSpecialAttack(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
	m_specialAttackNumber(-1),
	m_attackTime(0),
	m_endHitReaction(-1),
	m_isAttacked(false)
{

}

void CharacterStateSpecialAttack::Enter()
{
	//�K�E�Z���ݒ肳��Ă��Ȃ���Ύ~�߂�
	if (m_specialAttackNumber == -1) assert(false);

	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kSpecialAttack;

	//�K�E�Z�̏��擾
	auto attackData = m_pCharacter->GetSpecialAttackData(m_specialAttackNumber);

	//�A�j���[�V�����̎�ސݒ�
	Character::AnimKind anim = Character::AnimKind::kIdle;

	//��ڂ̕K�E�Z���g�p����ꍇ
	if (m_specialAttackNumber == 1)
	{
		anim = Character::AnimKind::kStartFirstSpecialAttack;
	}
	//��ڂ̕K�E�Z���g�p����ꍇ
	else if (m_specialAttackNumber == 2)
	{
		anim = Character::AnimKind::kStartSecondSpecialAttack;
	}

	//�A�j���[�V�����ύX
	m_pCharacter->ChangeAnim(anim, false);

	//�Z�̎�ނ�ݒ肷��
	auto kind = m_pCharacter->GetSpecialAttackData(m_specialAttackNumber).kind;

	//�Z���o���Ă��鎞�Ԃ�ݒ肷��
	m_attackTime = kAttackTimeMap.at(kind);

	//�G�̕���������
	m_pCharacter->LookTarget();

}

void CharacterStateSpecialAttack::Update()
{
	//����State�ɂ��鎞��
	m_time++;

	//�U���̏��
	auto attackData = m_pCharacter->GetSpecialAttackData(m_specialAttackNumber);

	//State���΂ɔ����鎞��
	int totalFrame = attackData.startFrame + m_attackTime + attackData.endFrame;

	//�U������߂Ă����莞�Ԃ����Ă�����
	if (m_time > totalFrame)
	{
		//�A�C�h����Ԃɖ߂�
		auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		ChangeState(next);
	}

	//�U������߂�t���[��
	float attackEndFrame = static_cast<float>(attackData.startFrame + m_attackTime);

	//�K�E�Z���󂯂����̏�ԂƓG�̏�Ԃ���v������
	if (m_endHitReaction == static_cast<int>(m_pManager->GetTargetHitReaction(m_pCharacter)) &&
		m_time < attackEndFrame)
	{
		//�U������߂�t���[���܂Ői�߂�
		m_time = attackEndFrame;
	}

	//�U������߂�t���[���ɂȂ�����
	if (m_time > attackEndFrame)
	{
		//�A�j���[�V�����̕ύX���s��
		if (m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kOnFirstSpecialAttack)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kEndFirstSpecialAttack, false);
		}
		else if (m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kOnSecondSpecialAttack)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kEndSecondSpecialAttack, false);
		}
		//�G�t�F�N�g���~�߂�
		m_pManager->ExitEffect(m_pEffect);
	}
	//�G�t�F�N�g���ݒ肳��Ă�����
	if (m_pEffect)
	{
		//���W��ݒ肷��
		m_pEffect->SetPos(m_pAttack->GetPos());
	}

	//�U�����o�����ԂɂȂ�����
	if (m_time > attackData.startFrame)
	{
		//�U�����o���Ă�����ʂ�Ȃ��悤�ɂ���
		if (m_isAttacked) return;

		//�A�j���[�V�����̕ύX���s��
		if (m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kStartFirstSpecialAttack)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kOnFirstSpecialAttack, true, 0.15f);
			//�ύX���s������ʒu�t���[���҂�
			return;
		}
		else if (m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kStartSecondSpecialAttack)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kOnSecondSpecialAttack, true, 0.15f);
			//�ύX���s������ʒu�t���[���҂�
			return;
		}

		//�U���̐ݒ������
		Character::AttackData attack;

		//�_���[�W�̐ݒ�
		attack.damage = static_cast<int>(attackData.damageRate * m_pCharacter->GetPower());

		//�󂯂����̃��A�N�V������ݒ�
		attack.attackHitKind = kHitKindMap.at(attackData.kind);

		//1P��2P���̐ݒ�
		if (m_pCharacter->GetTag() == ObjectTag::kOnePlayer)
		{
			attack.isPlayer = true;
		}
		else if (m_pCharacter->GetTag() == ObjectTag::kTwoPlayer)
		{
			attack.isPlayer = false;
		}

		//�K�E�Z���L�����N�^�[�ƂԂ��������Ƀ_���[�W���󂯂���̂ł����
		if (attackData.kind == Character::AttackKind::kAssault ||
			attackData.kind == Character::AttackKind::kPhysical ||
			attackData.kind == Character::AttackKind::kThrow ||
			attackData.kind == Character::AttackKind::kRush)
		{
			//�K�E�Z�̔�����L�����N�^�[�̓����蔻����������傫���傫���ɂ���
			attack.radius = GameSceneConstant::kCharacterRadius + 1.0f;
		}
		//�C�e�n�ł����
		else
		{
			//�ݒ肳�ꂽ�����蔻��̑傫�����g�p����
			attack.radius = attackData.radius;
		}

		//�U���̎�ނ�ݒ�
		attack.attackKind = attackData.kind;

		//�U���̑��x��ݒ�
		attack.speed = kAttackSpeedMap.at(attackData.kind);

		//�U���̐������Ԃ�ݒ�
		attack.lifeTime = kAttackTimeMap.at(attackData.kind);

		//�G�̏�Ԃ����ɂȂ�����U������߂邩��ݒ�
		m_endHitReaction = static_cast<int>(kHitKindMap.at(attackData.kind));

		//�U�����쐬
		m_pAttack = m_pCharacter->CreateAttack(attack);

		//�G�t�F�N�g���쐬
		m_pEffect = std::make_shared<Effect>(attackData.path);

		m_pEffect->SetPos(m_pAttack->GetPos());

		//��]��ݒ�
		float vX = m_pAttack->GetPos().x - m_pCharacter->GetPos().x;
		float vZ = m_pAttack->GetPos().z - m_pCharacter->GetPos().z;

		float rotY = std::atan2f(vX, vZ);

		m_pEffect->SetRotation(MyEngine::Vector3(0.0f, rotY, 0.0f));
		m_pManager->EntryEffect(m_pEffect);

		//�U�����o�����Ɛݒ�
		m_isAttacked = true;
	}

}

void CharacterStateSpecialAttack::Exit()
{
	if (m_pEffect)
	{
		m_pManager->ExitEffect(m_pEffect);
	}

	//�K�E�Z�����̃`���[�g���A�����N���A������
	SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kSpecialAttack));
}