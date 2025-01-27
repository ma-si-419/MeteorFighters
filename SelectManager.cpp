#include "SelectManager.h"
#include "SoundManager.h"
#include "SelectUi.h"
#include "Input.h"
#include "Game.h"

namespace
{
	//�X�e�B�b�N���X�����Ɣ��肷��X���̑傫��
	constexpr int kStickTiltPower = 500;

	//�L�����N�^�[�̕��я�
	constexpr int kCharacterIndex[static_cast<int>(SelectManager::CharacterNumber::kCharacterNum)] =
	{
		{0},{-1},{1}
	};
}

SelectManager::SelectManager() :
	m_playerNumber(0),
	m_enemyNumber(static_cast<int>(CharacterNumber::kBlueHead)),
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

	m_pUi->SetNumber(m_playerNumber, true);
	m_pUi->SetNumber(m_enemyNumber, false);

	m_pUi->Update();
}

void SelectManager::Draw()
{
	m_pUi->Draw();
}

int SelectManager::GetPlayerNumber()
{
	int ans = kCharacterIndex[m_playerNumber];

	//�����_�����I�΂�Ă����
	while (ans == -1)
	{
		//�����_���őI��
		ans = kCharacterIndex[GetRand(static_cast<int>(SelectManager::CharacterNumber::kCharacterNum))];
	}

	return ans;
}

int SelectManager::GetEnemyNumber()
{
	int ans = kCharacterIndex[m_enemyNumber];

	//�����_�����I�΂�Ă����
	while (ans == -1)
	{
		//�����_���őI��
		ans = kCharacterIndex[GetRand(static_cast<int>(SelectManager::CharacterNumber::kCharacterNum))];
	}

	return ans;
}

void SelectManager::SelectOnePlayer()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//�I�����Ă���A�C�R����Ui�ɓn��
	m_pUi->SetIconFrame(m_playerNumber,true);

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

		m_pUi->ChangeSituation(SelectUi::UiSituation::kSelect2P);

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

	//�I�����Ă���A�C�R����Ui�ɓn��
	m_pUi->SetIconFrame(m_enemyNumber,false);

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

	//A�{�^������������Q�[���V�[���ɍs��
	if (input->IsTrigger("A"))
	{
		SoundManager::GetInstance().OncePlaySound("Confirm");

		m_nextScene = static_cast<int>(Game::Scene::kGame);
	}
	//B�{�^������������1�v���C���[�̃L�����N�^�[�I����ʂɖ߂�
	else if (input->IsTrigger("B"))
	{
		SoundManager::GetInstance().OncePlaySound("Cancel");
		m_updateSelectFunc = &SelectManager::SelectOnePlayer;

		m_pUi->ChangeSituation(SelectUi::UiSituation::kSelect1P);
	}
}