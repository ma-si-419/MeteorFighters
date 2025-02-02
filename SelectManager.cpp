#include "SelectManager.h"
#include "SoundManager.h"
#include "SelectUi.h"
#include "Input.h"
#include "Game.h"
#include "EnemyInput.h"
#include "GraphManager.h"

namespace
{
	//スティックが傾いたと判定する傾きの大きさ
	constexpr int kStickTiltPower = 500;

	//キャラクターの並び順
	constexpr int kCharacterIndex[static_cast<int>(SelectManager::CharacterNumber::kCharacterNum)] =
	{
		{0},{-1},{1}
	};

	//スカイドームの画像ハンドルの名前
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

	//インプットデータの取得
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	int lastEnemyLevel = m_enemyLevel;

	//AIのレベルを常に変更できるようにする
	if (input->IsTrigger("RB"))
	{
		//RBボタンが押されたらAIのレベルを上げる
		m_enemyLevel++;

		//最大値を超えたら
		if (m_enemyLevel > static_cast<int>(EnemyInput::AiLevel::kHard))
		{
			//最小値に戻す
			m_enemyLevel = 0;
		}

	}
	else if (input->IsTrigger("LB"))
	{
		//LBボタンが押されたらAIのレベルを下げる
		m_enemyLevel--;

		//最小値を下回ったら
		if (m_enemyLevel < 0)
		{
			//最大値に戻す
			m_enemyLevel = static_cast<int>(EnemyInput::AiLevel::kHard);
		}
	}

	//クランプ
	m_enemyLevel = max(m_enemyLevel, 0);
	m_enemyLevel = min(m_enemyLevel, static_cast<int>(EnemyInput::AiLevel::kHard));

	//AIのレベルを設定
	m_pUi->SetLevel(m_enemyLevel);

	//AIのレベルが変わったら
	if (lastEnemyLevel != m_enemyLevel)
	{
		//スカイドームのハンドルを変更する
		m_pUi->SetSkyDomeHandle(GraphManager::GetInstance().GetHandle(kSkyDomeTextureHandleName[m_enemyLevel]));
	}

	//選択している番号をUiに渡す
	m_pUi->SetNumber(m_playerNumber, true);
	m_pUi->SetNumber(m_enemyNumber, false);

	//UIの更新
	m_pUi->Update();
}

void SelectManager::Draw()
{
	m_pUi->Draw();
}

int SelectManager::GetPlayerNumber()
{
	int ans = kCharacterIndex[m_playerNumber];

	//ランダムが選ばれていれば
	while (ans == -1)
	{
		//ランダムで選ぶ
		ans = kCharacterIndex[GetRand(static_cast<int>(SelectManager::CharacterNumber::kCharacterNum))];
	}

	return ans;
}

int SelectManager::GetEnemyNumber()
{
	int ans = kCharacterIndex[m_enemyNumber];

	//ランダムが選ばれていれば
	while (ans == -1)
	{
		//ランダムで選ぶ
		ans = kCharacterIndex[GetRand(static_cast<int>(SelectManager::CharacterNumber::kCharacterNum))];
	}

	return ans;
}

void SelectManager::SelectOnePlayer()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//選択しているアイコンをUiに渡す
	m_pUi->SetIconFrame(m_playerNumber, true);

	//キャラクター選択
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

	//Aボタンを押したら2プレイヤーの選択に移る
	if (input->IsTrigger("A"))
	{
		SoundManager::GetInstance().PlayOnceSound("Ok");
		m_updateSelectFunc = &SelectManager::SelectTwoPlayer;

		m_pUi->ChangeSituation(SelectUi::UiSituation::kSelect2P);

	}
	//Bボタンを押したらメニュー選択に戻る
	else if (input->IsTrigger("B"))
	{
		SoundManager::GetInstance().PlayOnceSound("Cancel");

		m_nextScene = static_cast<int>(Game::Scene::kMenu);
	}
}

void SelectManager::SelectTwoPlayer()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//選択しているアイコンをUiに渡す
	m_pUi->SetIconFrame(m_enemyNumber, false);

	//キャラクターの選択
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

	//Aボタンを押したらゲームシーンに行く
	if (input->IsTrigger("A"))
	{
		SoundManager::GetInstance().PlayOnceSound("Confirm");

		m_nextScene = static_cast<int>(Game::Scene::kGame);
	}
	//Bボタンを押したら1プレイヤーのキャラクター選択画面に戻る
	else if (input->IsTrigger("B"))
	{
		SoundManager::GetInstance().PlayOnceSound("Cancel");
		m_updateSelectFunc = &SelectManager::SelectOnePlayer;

		m_pUi->ChangeSituation(SelectUi::UiSituation::kSelect1P);
	}
}