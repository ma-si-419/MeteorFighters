#include "CharacterStateSpecialAttack.h"
#include "CharacterStateIdle.h"
#include "CharacterBase.h"
#include "Effect.h"
#include "Attack.h"
#include "GameSceneConstant.h"
#include <cassert>
#include <map>
#include <cmath>

namespace
{

	const std::map<CharacterBase::AttackKind, int> kAttackTimeMap =
	{
		{CharacterBase::AttackKind::kEnergy,90},
		{CharacterBase::AttackKind::kPhysical,90},
		{CharacterBase::AttackKind::kBeam,90},
		{CharacterBase::AttackKind::kRush,90},
		{CharacterBase::AttackKind::kThrow,30},
		{CharacterBase::AttackKind::kAssault,90}
	};

	const std::map<CharacterBase::AttackKind, CharacterBase::AttackHitKind> kHitKindMap =
	{
		{CharacterBase::AttackKind::kEnergy,CharacterBase::AttackHitKind::kWeakUpBurst},
		{CharacterBase::AttackKind::kPhysical,CharacterBase::AttackHitKind::kFarBurst},
		{CharacterBase::AttackKind::kBeam,CharacterBase::AttackHitKind::kWeakUpBurst},
		{CharacterBase::AttackKind::kRush,CharacterBase::AttackHitKind::kFarBurst},
		{CharacterBase::AttackKind::kThrow,CharacterBase::AttackHitKind::kFarBurst},
		{CharacterBase::AttackKind::kAssault,CharacterBase::AttackHitKind::kWeakUpBurst}
	};

	const std::map<CharacterBase::AttackKind, float> kAttackSpeedMap =
	{
		{CharacterBase::AttackKind::kEnergy,6.0f},
		{CharacterBase::AttackKind::kPhysical,0.0f},
		{CharacterBase::AttackKind::kBeam,6.0f},
		{CharacterBase::AttackKind::kRush,0.0f},
		{CharacterBase::AttackKind::kThrow,0.0f},
		{CharacterBase::AttackKind::kAssault,0.0f},
	};
}

CharacterStateSpecialAttack::CharacterStateSpecialAttack(std::shared_ptr<CharacterBase> character) :
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
	CharacterBase::AnimKind anim = CharacterBase::AnimKind::kIdle;

	//��ڂ̕K�E�Z���g�p����ꍇ
	if (m_specialAttackNumber == 1)
	{
		anim = CharacterBase::AnimKind::kStartFirstSpecialAttack;
	}
	//��ڂ̕K�E�Z���g�p����ꍇ
	else if (m_specialAttackNumber == 2)
	{
		anim = CharacterBase::AnimKind::kStartSecondSpecialAttack;
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
	int attackEndFrame = attackData.startFrame + m_attackTime;

	//�K�E�Z���󂯂����̏�ԂƓG�̏�Ԃ���v������
	if (m_endHitReaction == static_cast<int>(GetTargetHitReaction()) &&
		m_time < attackEndFrame)
	{
		//�U������߂�t���[���܂Ői�߂�
		m_time = attackEndFrame;
	}

	//�U������߂�t���[���ɂȂ�����
	if (m_time > attackEndFrame)
	{
		//�A�j���[�V�����̕ύX���s��
		if (m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kOnFirstSpecialAttack)
		{
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kEndFirstSpecialAttack, false);
		}
		else if (m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kOnSecondSpecialAttack)
		{
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kEndSecondSpecialAttack, false);
		}
		//�G�t�F�N�g���~�߂�
		ExitEffect(m_pEffect);
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
		if (m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kStartFirstSpecialAttack)
		{
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kOnFirstSpecialAttack, true, 0.15f);
			//�ύX���s������ʒu�t���[���҂�
			return;
		}
		else if (m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kStartSecondSpecialAttack)
		{
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kOnSecondSpecialAttack, true, 0.15f);
			//�ύX���s������ʒu�t���[���҂�
			return;
		}

		//�U���̐ݒ������
		CharacterBase::AttackData attack;

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
		if (attackData.kind == CharacterBase::AttackKind::kAssault ||
			attackData.kind == CharacterBase::AttackKind::kPhysical ||
			attackData.kind == CharacterBase::AttackKind::kThrow ||
			attackData.kind == CharacterBase::AttackKind::kRush)
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
		EntryEffect(m_pEffect);

		//�U�����o�����Ɛݒ�
		m_isAttacked = true;
	}

}

void CharacterStateSpecialAttack::Exit()
{

}
