#include "SelectManager.h"
#include "SoundManager.h"
#include "SelectUi.h"
#include "Input.h"
#include "Game.h"

namespace
{
	//�X�e�B�b�N���X�����Ɣ��肷��X���̑傫��
	constexpr int kStickTiltPower = 500;
	//�L�����N�^�[�̐�(0���琔����)
	constexpr int kCharacterNum = 1;
}

SelectManager::SelectManager():
	m_playerNumber(0),
	m_enemyNumber(0),
	m_nextScene(-1)
{
	m_updateSelectFunc = &SelectManager::SelectOnePlayer;
	m_pUi = std::make_shared<SelectUi>();
}

SelectManager::~SelectManager()
{
}

void SelectManager::Init()
{
	m_pUi->Init();
}

void SelectManager::Update()
{
	(this->*m_updateSelectFunc)();

	m_pUi->SetNumber(m_playerNumber,true);
	m_pUi->SetNumber(m_enemyNumber,false);

	m_pUi->Update();
}

void SelectManager::Draw()
{
	m_pUi->Draw();
}

void SelectManager::SelectOnePlayer()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//�L�����N�^�[�I��
	if (input->IsTrigger("Right"))
	{
		m_playerNumber++;

		m_playerNumber = min(m_playerNumber, static_cast<int>(CharacterNumber::kCharacterNum) - 1);
	}
	else if (input->IsTrigger("Left"))
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

		m_nextScene = static_cast<int>(Game::Scene::kMenu);
	}
}

void SelectManager::SelectTwoPlayer()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//�L�����N�^�[�̑I��
	if (input->IsTrigger("Right"))
	{
		m_enemyNumber++;

		m_enemyNumber = min(m_enemyNumber, static_cast<int>(CharacterNumber::kCharacterNum) - 1);
	}
	else if (input->IsTrigger("Left"))
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

		m_nextScene = static_cast<int>(Game::Scene::kGame);
	}
	//B�{�^������������1�v���C���[�̃L�����N�^�[�I����ʂɖ߂�
	else if (input->IsTrigger("B"))
	{
		SoundManager::GetInstance().OncePlaySound("Cancel");
		m_updateSelectFunc = &SelectManager::SelectTwoPlayer;
	}
}
