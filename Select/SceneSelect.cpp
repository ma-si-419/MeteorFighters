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
		m_updateSelectFunc = &SceneSelect::SelectTwoPlayer;
	}
	//Bボタンを押したらメニュー選択に戻る
	else if (input->IsTrigger("B"))
	{
		SoundManager::GetInstance().OncePlaySound("Cancel");

		m_sceneManager.ChangeScene(std::make_shared<SceneMenu>(m_sceneManager));
	}

}

void SceneSelect::SelectTwoPlayer()
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
		m_updateSelectFunc = &SceneSelect::ConfirmCharacter;
	}
	//Bボタンを押したら1プレイヤーのキャラクター選択画面に戻る
	else if (input->IsTrigger("B"))
	{
		SoundManager::GetInstance().OncePlaySound("Cancel");
		m_updateSelectFunc = &SceneSelect::SelectOnePlayer;
	}
}

void SceneSelect::ConfirmCharacter()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//Aボタンを押したらゲームシーンに移動
	if (input->IsTrigger("A"))
	{
		SoundManager::GetInstance().OncePlaySound("Confirm");

		//ゲームシーンのポインタ作成
		std::shared_ptr<SceneGame> scene = std::make_shared<SceneGame>(m_sceneManager);
		//ゲームシーンに選択しているキャラクター情報を与える
		scene->SetCharacter(m_playerNumber, m_enemyNumber);
		//ゲームシーンに移行する
		m_sceneManager.ChangeScene(scene);
	}
	//Bボタンを押したら1プレイヤーのキャラクター選択画面に戻る
	else if (input->IsTrigger("B"))
	{
		SoundManager::GetInstance().OncePlaySound("Cancel");
		m_updateSelectFunc = &SceneSelect::SelectTwoPlayer;
	}
}
