#include "SceneSelect.h"
#include "SceneGame.h"
#include "SceneMenu.h"
#include "DxLib.h"
#include "Input.h"
#include "SoundManager.h"
#include <string>

namespace
{
	const int kRedColor = GetColor(255, 0, 0);
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

SceneSelect::SceneSelect(SceneManager& sceneManager) :
	SceneBase(sceneManager),
	m_playerNumber(0),
	m_enemyNumber(0)
{
	m_updateSelectFunc = &SceneSelect::SelectOnePlayer;
}

SceneSelect::~SceneSelect()
{
}

void SceneSelect::Init()
{
	SoundManager::GetInstance().LoadSceneSound("Select");
}

void SceneSelect::Update()
{
	(this->*m_updateSelectFunc)();
}

void SceneSelect::UpdateAsyncLoad()
{
}

void SceneSelect::Draw()
{
//      #ifdef _DEBUG

	DrawString(100, 100, kCharacterName[m_playerNumber].c_str(), kRedColor);
	DrawString(300, 100, kCharacterName[m_enemyNumber].c_str(), kRedColor);

	DrawString(0, 0, "SceneSelect", kRedColor);

//      #endif // _DEBUG
}

void SceneSelect::End()
{
}

void SceneSelect::SelectOnePlayer()
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
		m_updateSelectFunc = &SceneSelect::SelectTwoPlayer;
	}
	//B�{�^�����������烁�j���[�I���ɖ߂�
	else if (input->IsTrigger("B"))
	{
		SoundManager::GetInstance().OncePlaySound("Cancel");

		m_sceneManager.ChangeScene(std::make_shared<SceneMenu>(m_sceneManager));
	}

}

void SceneSelect::SelectTwoPlayer()
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
		m_updateSelectFunc = &SceneSelect::ConfirmCharacter;
	}
	//B�{�^������������1�v���C���[�̃L�����N�^�[�I����ʂɖ߂�
	else if (input->IsTrigger("B"))
	{
		SoundManager::GetInstance().OncePlaySound("Cancel");
		m_updateSelectFunc = &SceneSelect::SelectOnePlayer;
	}
}

void SceneSelect::ConfirmCharacter()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//A�{�^������������Q�[���V�[���Ɉړ�
	if (input->IsTrigger("A"))
	{
		SoundManager::GetInstance().OncePlaySound("Confirm");

		//�Q�[���V�[���̃|�C���^�쐬
		std::shared_ptr<SceneGame> scene = std::make_shared<SceneGame>(m_sceneManager);
		//�Q�[���V�[���ɑI�����Ă���L�����N�^�[����^����
		scene->SetCharacter(m_playerNumber, m_enemyNumber);
		//�Q�[���V�[���Ɉڍs����
		m_sceneManager.ChangeScene(scene);
	}
	//B�{�^������������1�v���C���[�̃L�����N�^�[�I����ʂɖ߂�
	else if (input->IsTrigger("B"))
	{
		SoundManager::GetInstance().OncePlaySound("Cancel");
		m_updateSelectFunc = &SceneSelect::SelectTwoPlayer;
	}
}
