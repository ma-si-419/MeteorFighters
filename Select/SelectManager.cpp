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

	//BGMの数
	constexpr int kBgmNum = 2;

	//BGMの番号の最小値
	constexpr int kBgmMin = 0;

	//BGMのなまえ
	const std::string kBgmName[kBgmNum + 1] =
	{
		"最強のデカヘッド",
		"デカヘッドの決戦",
		"デカヘッドは止まらない"
	};

	//BGMの音量
	constexpr int kBgmVolume = 220;
}

SelectManager::SelectManager() :
	m_playerNumber(0),
	m_enemyNumber(static_cast<int>(CharacterNumber::kBlueHead)),
	m_nextScene(-1),
	m_enemyLevel(0),
	m_bgmNumber(kBgmMin)
{
	m_updateSelectFunc = &SelectManager::SelectOnePlayer;
	m_pUi = std::make_shared<SelectUi>();
}

SelectManager::~SelectManager()
{
	//BGMの停止
	SoundManager::GetInstance().StopLoopSound("Bgm" + std::to_string(m_bgmNumber));
}

void SelectManager::Init()
{
	m_pUi->Init();
	m_pUi->SetSkyDomeHandle(GraphManager::GetInstance().GetHandle(kSkyDomeTextureHandleName[m_enemyLevel]));

	//BGMの再生
	SoundManager::GetInstance().PlayLoopSound("Bgm0");

	//BGMの音量を設定
	SoundManager::GetInstance().SetSoundVolume("Bgm0", kBgmVolume);


	//UIにBGMの名前を渡す
	m_pUi->SetMusicName(kBgmName[m_bgmNumber]);
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
	
		//レベルを切り替えた時の音を再生する
		SoundManager::GetInstance().PlayOnceSound("LevelChange");
			
	}

	//YボタンでBGMを変更する
	if (input->IsTrigger("Y"))
	{
		//BGMを変更する
		SoundManager::GetInstance().StopLoopSound("Bgm" + std::to_string(m_bgmNumber));
		
		m_bgmNumber++;

		//最大値を超えたら
		if (m_bgmNumber > kBgmNum)
		{
			//最小値に戻す
			m_bgmNumber = kBgmMin;
		}

		//切り替えサウンドを再生
		SoundManager::GetInstance().PlayOnceSound("MusicChange");

		//BGMを再生
		SoundManager::GetInstance().PlayLoopSound("Bgm" + std::to_string(m_bgmNumber));

		//BGMの音量を設定
		SoundManager::GetInstance().SetSoundVolume("Bgm" + std::to_string(m_bgmNumber), kBgmVolume);

		//UIにBGMの名前を渡す
		m_pUi->SetMusicName(kBgmName[m_bgmNumber]);

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

	//前のフレームで選択していたキャラクターの番号
	int lastPlayerNumber = m_playerNumber;

	//キャラクター選択
	if (input->IsTrigger("Right"))
	{
		m_playerNumber++;

		//最大値を超えたら
		if (m_playerNumber >= static_cast<int>(CharacterNumber::kCharacterNum))
		{
			//最小値に戻す
			m_playerNumber = 0;
		}
	}
	else if (input->IsTrigger("Left"))
	{
		m_playerNumber--;

		//最小値よりも小さくなったら
		if (m_playerNumber < 0)
		{
			//最大値に戻す
			m_playerNumber = static_cast<int>(CharacterNumber::kCharacterNum) - 1;
		}
	}

		m_playerNumber = max(m_playerNumber, 0);
		m_playerNumber = min(m_playerNumber, static_cast<int>(CharacterNumber::kCharacterNum) - 1);

	//キャラクターが変わったら
	if (lastPlayerNumber!= m_playerNumber)
	{
		//サウンドを再生
		SoundManager::GetInstance().PlayOnceSound("Select");
	}

	//Aボタンを押したら2プレイヤーの選択に移る
	if (input->IsTrigger("A"))
	{
		SoundManager::GetInstance().PlayOnceSound("CharacterConfirm");
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

	int lastEnemyNumber = m_enemyNumber;

	//キャラクターの選択
	if (input->IsTrigger("Right"))
	{
		m_enemyNumber++;

		//最大値を超えたら
		if (m_enemyNumber >= static_cast<int>(CharacterNumber::kCharacterNum))
		{
			//最小値に戻す
			m_enemyNumber = 0;
		}

	}
	else if (input->IsTrigger("Left"))
	{
		m_enemyNumber--;

		//最小値よりも小さくなったら
		if (m_enemyNumber < 0)
		{
			//最大値に戻す
			m_enemyNumber = static_cast<int>(CharacterNumber::kCharacterNum) - 1;
		}

	}

		m_enemyNumber = max(m_enemyNumber, 0);
		m_enemyNumber = min(m_enemyNumber, static_cast<int>(CharacterNumber::kCharacterNum) - 1);

	//キャラクターが変わったら
	if (lastEnemyNumber != m_enemyNumber)
	{
		//サウンドを再生
		SoundManager::GetInstance().PlayOnceSound("Select");
	}


	//Aボタンを押したらゲームシーンに行く
	if (input->IsTrigger("A"))
	{
		SoundManager::GetInstance().PlayOnceSound("CharacterConfirm");

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