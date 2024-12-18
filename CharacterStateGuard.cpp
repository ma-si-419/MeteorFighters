#include "CharacterStateGuard.h"
#include "CharacterStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "CharacterBase.h"

namespace
{
	//�U��������ł��鎞��
	constexpr float kDodgeTime = 15;
	//�A�j���[�V�����u�����h�̑���
	constexpr float kBlendSpeed = 1.0f / kDodgeTime;
	//�X�e�B�b�N���X���Ă���Ɣ��f����X���̑傫��
	constexpr int kStickTiltPower = 300;
	//�㉺��؂�ւ���Ƃ��̃u�����h�̑���
	constexpr float kGuardSwitchBlendSpeed = 0.16f;
}

CharacterStateGuard::CharacterStateGuard(std::shared_ptr<CharacterBase> character) :
	CharacterStateBase(character)
{
}

void CharacterStateGuard::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kGuard;
	m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kGuardMiddle, true, kBlendSpeed);
}

void CharacterStateGuard::Update()
{
	//�㉺���͂ŃK�[�h�̌�����ς���
	int stickY = GetCharacterInput()->GetStickInfo().leftStickY;

	//��ɌX���Ă�����
	if (stickY < -kStickTiltPower)
	{
		//��K�[�h�ɕύX����
		//��K�[�h�͉������Ɍ����Ă̐�����΂��U�����K�[�h�ł���

		//TODO : ���̓A�j���[�V���������ύX
		if (m_pCharacter->GetPlayAnimKind() != CharacterBase::AnimKind::kGuardHigh)
		{
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kGuardHigh, true, kGuardSwitchBlendSpeed);
		}

	}
	//���ɌX���Ă�����
	else if (stickY > kStickTiltPower)
	{
		//���K�[�h�ɕύX����
		//���K�[�h�͏�����Ɍ����Ă̐�����΂��U���Ɖ��i���ߍU�����K�[�h�ł���

		//TODO : ���̓A�j���[�V���������ύX
		if (m_pCharacter->GetPlayAnimKind() != CharacterBase::AnimKind::kGuardLow)
		{
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kGuardLow, true, kGuardSwitchBlendSpeed);
		}

	}
	//�ǂ���ɂ��X���Ă��Ȃ����
	else
	{
		if (m_pCharacter->GetPlayAnimKind() != CharacterBase::AnimKind::kGuardMiddle)
		{
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kGuardMiddle, true, kGuardSwitchBlendSpeed);
		}
	}


	//�K�[�h�{�^����������Ă���Ԃ���State�ɂ���
	if (!GetCharacterInput()->IsPress("B"))
	{
		//�K�[�h�{�^���������ꂽ��
		std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		ChangeState(next);
	}


#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Guard", GetColor(255, 255, 255));

#endif // _DEBUG

}
void CharacterStateGuard::Exit()
{
}
