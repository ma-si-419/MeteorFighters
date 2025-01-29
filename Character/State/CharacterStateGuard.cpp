#include "CharacterStateGuard.h"
#include "CharacterStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "Character.h"

namespace
{
	//�U��������ł��鎞��
	constexpr float kJustGuardTime = 15;
	//�A�j���[�V�����u�����h�̑���
	constexpr float kBlendSpeed = 1.0f / kJustGuardTime;
	//�X�e�B�b�N���X���Ă���Ɣ��f����X���̑傫��
	constexpr int kStickTiltPower = 300;
	//�㉺��؂�ւ���Ƃ��̃u�����h�̑���
	constexpr float kGuardSwitchBlendSpeed = 0.16f;
}

CharacterStateGuard::CharacterStateGuard(std::shared_ptr<Character> character) :
	CharacterStateBase(character)
{
}

void CharacterStateGuard::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kGuard;
	m_pCharacter->ChangeAnim(Character::AnimKind::kGuardMiddle, true, kBlendSpeed);
	m_guardKind = CharacterGuardKind::kMiddleGuard;

	//�G�̕���������
	m_pCharacter->LookTarget();
}

void CharacterStateGuard::Update()
{
	//State�ɂ��鎞�Ԃ��v������
	m_time++;

	//�����Ȃ����Ԃ����炷
	m_stopTime--;

	//�����Ȃ��悤�ɂ���
	SetCharacterVelo(MyEngine::Vector3(0,0,0));

	//�㉺���͂ŃK�[�h�̌�����ς���
	int stickY = GetCharacterInput()->GetStickInfo().leftStickY;

	//�K�[�h�̕���
	auto guardKind = CharacterGuardKind::kMiddleGuard;

	//��ɌX���Ă�����
	if (stickY < -kStickTiltPower)
	{
		//��K�[�h�ɕύX����
		guardKind = CharacterGuardKind::kUpGuard;

		//�A�j���[�V������ύX
		if (m_pCharacter->GetPlayAnimKind() != Character::AnimKind::kGuardHigh)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kGuardHigh, true, kGuardSwitchBlendSpeed);
		}

	}
	//���ɌX���Ă�����
	else if (stickY > kStickTiltPower)
	{
		//���K�[�h�ɕύX����
		guardKind = CharacterGuardKind::kDownGuard;

		//�A�j���[�V������ύX
		if (m_pCharacter->GetPlayAnimKind() != Character::AnimKind::kGuardLow)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kGuardLow, true, kGuardSwitchBlendSpeed);
		}

	}
	//�ǂ���ɂ��X���Ă��Ȃ����
	else
	{
		//���i�K�[�h�ɕύX����
		guardKind = CharacterGuardKind::kMiddleGuard;

		//�A�j���[�V������ύX
		if (m_pCharacter->GetPlayAnimKind() != Character::AnimKind::kGuardMiddle)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kGuardMiddle, true, kGuardSwitchBlendSpeed);
		}
	}

	//����State�ɗ��Ă����莞�Ԉȓ��ł����
	if (m_time < kJustGuardTime)
	{
		//�W���X�g�K�[�h�ɂ���
		guardKind = CharacterGuardKind::kJustGuard;
	}

	//�K�[�h�̏�Ԃ�ݒ肷��
	m_guardKind = guardKind;

	//�K�[�h�{�^����������Ă���Ԃ���State�ɂ���
	if (!GetCharacterInput()->IsPress("B") && m_stopTime < 0)
	{
		//�K�[�h�{�^���������ꂽ��
		std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		ChangeState(next);
	}
}
void CharacterStateGuard::Exit()
{
	m_guardKind = CharacterGuardKind::kNone;
}
