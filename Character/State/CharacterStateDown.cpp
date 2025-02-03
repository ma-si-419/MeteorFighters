#include "CharacterStateDown.h"
#include "Character.h"
#include "CharacterStateIdle.h"
#include "CharacterStateDash.h"
#include "TutorialManager.h"
#include "GameManagerBase.h"


namespace
{
	//�_�E�����鎞��
	constexpr int kDownTime = 90;

	//������悤�ɂȂ鎞��
	constexpr int kMoveTime = 25;
}

CharacterStateDown::CharacterStateDown(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
	m_isReturn(false)
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
		//���A�t���O�𗧂Ă�
		m_isReturn = true;

	}
	//���̓f�[�^���擾
	auto input = m_pCharacter->GetInputData();

	//���A�ł���󋵂ŕ��A���͂���������
	if (input->IsTrigger("B") && m_isReturn)
	{
		//���̏�Ԃ��_�b�V����Ԃɐݒ肷��
		std::shared_ptr<CharacterStateDash> next = std::make_shared<CharacterStateDash>(m_pCharacter);
		//����ɂȂ�悤�ɐݒ�
		next->SetDodge();
		//�ړ�������ݒ�
		next->SetMoveDir(MyEngine::Vector3(0.0f, 0.0f, -1.0f));
		//�_�b�V����ԂɑJ��
		ChangeState(next);
		//���A�`���[�g���A�����N���A
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kReturn));
		return;
	}

	//�ړ��x�N�g����0�ɂ���
	SetCharacterVelo(MyEngine::Vector3());
}

void CharacterStateDown::Exit()
{
	//�O������G�̍��W��Y�����𔲂������̂ɂ���
	m_pCharacter->SetFrontPos(MyEngine::Vector3(m_pManager->GetTargetPos(m_pCharacter).x, m_pCharacter->GetPos().y, m_pManager->GetTargetPos(m_pCharacter).z));
}

void CharacterStateDown::SetFrontHit(bool front)
{
	//�O������U�����󂯂����ǂ�����ݒ�
	if (front)
	{
		m_pCharacter->ChangeAnim(Character::AnimKind::kFrontDown, false);
	}
	else
	{
		m_pCharacter->ChangeAnim(Character::AnimKind::kBackDown, false);
	}
}
