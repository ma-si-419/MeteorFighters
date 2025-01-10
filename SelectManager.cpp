#include "SelectManager.h"
#include "SoundManager.h"
#include "Input.h"
#include "Game.h"

namespace
{
	//スティックが傾いたと判定する傾きの大きさ
	constexpr int kStickTiltPower = 500;
	//キャラクターの数(0から数えて)
	constexpr int kCharacterNum = 1;

	//     #ifdef _DEBUG

	const std::string kCharacterName[2] =
	{
		"タダノマウス",
		"デカアオアタマ"
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

	//キャラクター選択
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

	//Aボタンを押したら2プレイヤーの選択に移る
	if (input->IsTrigger("A"))
	{
		SoundManager::GetInstance().OncePlaySound("Ok");
		m_updateSelectFunc = &SelectManager::SelectTwoPlayer;
	}
	//Bボタンを押したらメニュー選択に戻る
	else if (input->IsTrigger("B"))
	{
		SoundManager::GetInstance().OncePlaySound("Cancel");

		m_isNextScene = static_cast<int>(Game::Scene::kMenu);
	}
}

void SelectManager::SelectTwoPlayer()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//キャラクターの選択
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

	//Aボタンを押したら確定画面に行く
	if (input->IsTrigger("A"))
	{
		SoundManager::GetInstance().OncePlaySound("Ok");
		m_updateSelectFunc = &SelectManager::ConfirmCharacter;
	}
	//Bボタンを押したら1プレイヤーのキャラクター選択画面に戻る
	else if (input->IsTrigger("B"))
	{
		SoundManager::GetInstance().OncePlaySound("Cancel");
		m_updateSelectFunc = &SelectManager::SelectOnePlayer;
	}
}

void SelectManager::ConfirmCharacter()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//Aボタンを押したらゲームシーンに移動
	if (input->IsTrigger("A"))
	{
		SoundManager::GetInstance().OncePlaySound("Confirm");

		m_isNextScene = static_cast<int>(Game::Scene::kGame);
	}
	//Bボタンを押したら1プレイヤーのキャラクター選択画面に戻る
	else if (input->IsTrigger("B"))
	{
		SoundManager::GetInstance().OncePlaySound("Cancel");
		m_updateSelectFunc = &SelectManager::SelectTwoPlayer;
	}
}
