#include "CharacterStateDown.h"
#include "Character.h"
#include "CharacterStateIdle.h"
#include "CharacterStateDash.h"

namespace
{
	//�_�E�����鎞��
	constexpr int kDownTime = 120;

	//������悤�ɂȂ鎞��
	constexpr int kMoveTime = 45;
}

CharacterStateDown::CharacterStateDown(std::shared_ptr<Character> character) :
	CharacterStateBase(character)
{
}

void CharacterStateDown::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kDown;


}

void CharacterStateDown::Update()
{
	//���Ԍv��
	m_time++;

	//�_�E�����鎞�Ԃ��o������
	if (m_time > kDownTime)
	{
		//���̏�Ԃ��A�C�h����Ԃɐݒ肷��
		std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);
		//�A�C�h����ԂɑJ�ڂ���
		ChangeState(next);
	}

	//������悤�ɂȂ鎞�Ԃ��o������
	if (m_time > kMoveTime)
	{
		//���̓f�[�^���擾
		auto input = m_pCharacter->GetInputData();

		//�����͂���������
		if (input->IsTrigger("A"))
		{
			//���̏�Ԃ��_�b�V����Ԃɐݒ肷��
			std::shared_ptr<CharacterStateDash> next = std::make_shared<CharacterStateDash>(m_pCharacter);
			//�_�b�V����ԂɑJ��
			ChangeState(next);
			//����ɂȂ�悤�ɐݒ�
			next->SetDodge();
		}
	}

	//�ړ��x�N�g����0�ɂ���
	SetCharacterVelo(MyEngine::Vector3());
}

void CharacterStateDown::Exit()
{
}

void CharacterStateDown::SetFrontHit(bool front)
{
	//�O������U�����󂯂����ǂ�����ݒ�
	if (front)
	{
		m_pCharacter->ChangeAnim(Character::AnimKind::kFrontDown,false);
	}
	else
	{
		m_pCharacter->ChangeAnim(Character::AnimKind::kBackDown,false);
	}
}
