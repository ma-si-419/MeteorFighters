#include "SelectManager.h"
#include "SoundManager.h"
#include "SelectUi.h"
#include "Input.h"
#include "Game.h"
#include "EnemyInput.h"
#include "GraphManager.h"

namespace
{
	//�X�e�B�b�N���X�����Ɣ��肷��X���̑傫��
	constexpr int kStickTiltPower = 500;

	//�L�����N�^�[�̕��я�
	constexpr int kCharacterIndex[static_cast<int>(SelectManager::CharacterNumber::kCharacterNum)] =
	{
		{0},{-1},{1}
	};

	//�X�J�C�h�[���̉摜�n���h���̖��O
	const std::string kSkyDomeTextureHandleName[static_cast<int>(EnemyInput::AiLevel::kLevelNum)] =
	{
		"Easy",
		"Normal",
		"Hard"
	};
}

SelectManager::SelectManager() :
	m_playerNumber(0),
	m_enemyNumber(static_cast<int>(CharacterNumber::kBlueHead)),
	m_nextScene(-1),
	m_enemyLevel(0)
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
	m_pUi->SetSkyDomeHandle(GraphManager::GetInstance().GetHandle(kSkyDomeTextureHandleName[m_enemyLevel]));
}

void SelectManager::Update()
{
	(this->*m_updateSelectFunc)();

	//�C���v�b�g�f�[�^�̎擾
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	int lastEnemyLevel = m_enemyLevel;

	//AI�̃��x������ɕύX�ł���悤�ɂ���
	if (input->IsTrigger("RB"))
	{
		//RB�{�^���������ꂽ��AI�̃��x�����グ��
		m_enemyLevel++;

		//�ő�l�𒴂�����
		if (m_enemyLevel > static_cast<int>(EnemyInput::AiLevel::kHard))
		{
			//�ŏ��l�ɖ߂�
			m_enemyLevel = 0;
		}

	}
	else if (input->IsTrigger("LB"))
	{
		//LB�{�^���������ꂽ��AI�̃��x����������
		m_enemyLevel--;

		//�ŏ��l�����������
		if (m_enemyLevel < 0)
		{
			//�ő�l�ɖ߂�
			m_enemyLevel = static_cast<int>(EnemyInput::AiLevel::kHard);
		}
	}

	//�N�����v
	m_enemyLevel = max(m_enemyLevel, 0);
	m_enemyLevel = min(m_enemyLevel, static_cast<int>(EnemyInput::AiLevel::kHard));

	//AI�̃��x����ݒ�
	m_pUi->SetLevel(m_enemyLevel);

	//AI�̃��x�����ς������
	if (lastEnemyLevel != m_enemyLevel)
	{
		//�X�J�C�h�[���̃n���h����ύX����
		m_pUi->SetSkyDomeHandle(GraphManager::GetInstance().GetHandle(kSkyDomeTextureHandleName[m_enemyLevel]));
	}

	//�I�����Ă���ԍ���Ui�ɓn��
	m_pUi->SetNumber(m_playerNumber, true);
	m_pUi->SetNumber(m_enemyNumber, false);

	//UI�̍X�V
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
	m_pUi->SetIconFrame(m_playerNumber, true);

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
		SoundManager::GetInstance().PlayOnceSound("Ok");
		m_updateSelectFunc = &SelectManager::SelectTwoPlayer;

		m_pUi->ChangeSituation(SelectUi::UiSituation::kSelect2P);

	}
	//B�{�^�����������烁�j���[�I���ɖ߂�
	else if (input->IsTrigger("B"))
	{
		SoundManager::GetInstance().PlayOnceSound("Cancel");

		m_nextScene = static_cast<int>(Game::Scene::kMenu);
	}
}

void SelectManager::SelectTwoPlayer()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//�I�����Ă���A�C�R����Ui�ɓn��
	m_pUi->SetIconFrame(m_enemyNumber, false);

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
		SoundManager::GetInstance().PlayOnceSound("Confirm");

		m_nextScene = static_cast<int>(Game::Scene::kGame);
	}
	//B�{�^������������1�v���C���[�̃L�����N�^�[�I����ʂɖ߂�
	else if (input->IsTrigger("B"))
	{
		SoundManager::GetInstance().PlayOnceSound("Cancel");
		m_updateSelectFunc = &SelectManager::SelectOnePlayer;

		m_pUi->ChangeSituation(SelectUi::UiSituation::kSelect1P);
	}
}