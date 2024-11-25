#include "CharacterStateSpecialAttack.h"
#include <cassert>
#include "CharacterBase.h"
#include <map>

namespace
{

	const std::map<CharacterBase::AttackKind,int> kAttackTimeMap = 
	{
		{CharacterBase::AttackKind::kEnergy,30},
		{CharacterBase::AttackKind::kPhysical,30},
		{CharacterBase::AttackKind::kBeam,30},
		{CharacterBase::AttackKind::kRush,30},
		{CharacterBase::AttackKind::kThrow,30},
		{CharacterBase::AttackKind::kAssault,30}
	}

}

CharacterStateSpecialAttack::CharacterStateSpecialAttack(std::shared_ptr<CharacterBase> character):
	CharacterStateBase(character),
	m_specialAttackNumber(-1),
	m_attackTime(0)
{

}

void CharacterStateSpecialAttack::Enter()
{
	//�K�E�Z���ݒ肳��Ă��Ȃ���Ύ~�߂�
	if (m_specialAttackNumber == -1) assert(false);

	//�K�E�Z�̏��擾
	auto attackData = m_pCharacter->GetSpecialAttackData(m_specialAttackNumber);

	//�A�j���[�V�����̎�ސݒ�
	CharacterBase::AnimKind anim = CharacterBase::AnimKind::kIdle;

	//��ڂ̕K�E�Z���g�p����ꍇ
	if (m_specialAttackNumber == 1)
	{
		anim = CharacterBase::AnimKind::kFirstSpecialAttack;
	}
	//��ڂ̕K�E�Z���g�p����ꍇ
	else if (m_specialAttackNumber == 2)
	{
		anim = CharacterBase::AnimKind::kSecondSpecialAttack;
	}

	//�A�j���[�V�����ύX
	m_pCharacter->ChangeAnim(anim,false);

	//�Z�̎�ނ�ݒ肷��
	auto kind = m_pCharacter->GetSpecialAttackData(m_specialAttackNumber).kind;

	//�Z���o���Ă��鎞�Ԃ�ݒ肷��
	m_attackTime = kAttackTimeMap.at(kind);

}

void CharacterStateSpecialAttack::Update()
{
	//����State�ɂ��鎞��
	m_time++;

}

void CharacterStateSpecialAttack::Exit()
{

}
