#include "SelectManager.h"
#include "SoundManager.h"
#include "Input.h"
#include "Game.h"

namespace
{
	//�X�e�B�b�N���X�����Ɣ��肷��X���̑傫��
	constexpr int kStickTiltPower = 500;
	//�L�����N�^�[�̐�(0���琔����)
	constexpr int kCharacterNum = 1;

	//     #ifdef _DEBUG

	const std::string kCharacterName[2] =
	{
		"�^�_�m�}�E�X",
		"�f�J�A�I�A�^�}"
	};

	//	   #endif // _DEBUG
}

SelectManager::SelectManager()
{
	m_updateSelectFunc = &SelectManager::SelectOnePlayer;
}

SelectManager::~SelectManager()
{
}

void SelectManager::Update()
{
	(this->*m_updateSelectFunc)();
}

void SelectManager::SelectOnePlayer()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//�L�����N�^�[�I��
	if (input->IsTrigger("Up"))
	{
		m_playerNumber++;

		m_playerNumber = min(m_playerNumber, kCharacterNum);
	}
	else if (input->IsTrigger("Down"))
	{
		m_playerNumber--;

		m_playerNumber = max(m_playerNumber, 0);
	}

	//A�{�^������������2�v���C���[�̑I���Ɉڂ�
	if (input->IsTrigger("A"))
	{
		SoundManager::GetInstance().OncePlaySound("Ok");
		m_updateSelectFunc = &SelectManager::SelectTwoPlayer;
	}
	//B�{�^�����������烁�j���[�I���ɖ߂�
	else if (input->IsTrigger("B"))
	{
		SoundManager::GetInstance().OncePlaySound("Cancel");

		m_isNextScene = static_cast<int>(Game::Scene::kMenu);
	}
}

void SelectManager::SelectTwoPlayer()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//�L�����N�^�[�̑I��
	if (input->IsTrigger("Up"))
	{
		m_enemyNumber++;

		m_enemyNumber = min(m_enemyNumber, kCharacterNum);
	}
	else if (input->IsTrigger("Down"))
	{
		m_enemyNumber--;

		m_enemyNumber = max(m_enemyNumber, 0);
	}

	//A�{�^������������m���ʂɍs��
	if (input->IsTrigger("A"))
	{
		SoundManager::GetInstance().OncePlaySound("Ok");
		m_updateSelectFunc = &SelectManager::ConfirmCharacter;
	}
	//B�{�^������������1�v���C���[�̃L�����N�^�[�I����ʂɖ߂�
	else if (input->IsTrigger("B"))
	{
		SoundManager::GetInstance().OncePlaySound("Cancel");
		m_updateSelectFunc = &SelectManager::SelectOnePlayer;
	}
}

void SelectManager::ConfirmCharacter()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//A�{�^������������Q�[���V�[���Ɉړ�
	if (input->IsTrigger("A"))
	{
		SoundManager::GetInstance().OncePlaySound("Confirm");

		m_isNextScene = static_cast<int>(Game::Scene::kGame);
	}
	//B�{�^������������1�v���C���[�̃L�����N�^�[�I����ʂɖ߂�
	else if (input->IsTrigger("B"))
	{
		SoundManager::GetInstance().OncePlaySound("Cancel");
		m_updateSelectFunc = &SelectManager::SelectTwoPlayer;
	}
}
