#include "SceneGame.h"
#include "SceneSelect.h"
#include "SceneMenu.h"
#include "DxLib.h"
#include "Character.h"
#include "BattleManager.h"
#include "GraphManager.h"
#include "LoadManager.h"
#include "Physics.h"
#include "LoadCsv.h"
#include "Game.h"
#include "GameCamera.h"

namespace
{
	const int kRedColor = GetColor(255, 0, 0);
}

SceneGame::SceneGame(SceneManager& sceneManager) :
	SceneBase(sceneManager)
{

}

SceneGame::~SceneGame()
{
}

void SceneGame::Init()
{
	//画像のロード
	GraphManager::GetInstance().LoadSceneGraph("Game");

	//非同期ロードマネージャー
	auto& manager = LoadManager::GetInstance();

	m_pGameManager->Init();

	m_pCharacters.push_back(m_pGameManager->GetOnePlayerPointer());
	m_pCharacters.push_back(m_pGameManager->GetTwoPlayerPointer());

	//モデルのパス設定
	for (auto& actor : m_pCharacters)
	{
		actor->Init();

	}

	manager.StartAsyncLoad();

	manager.LoadHandle("Player1", m_pCharacters[0]->GetModelPath(), LoadManager::FileKind::kModel);
	manager.LoadHandle("Player2", m_pCharacters[1]->GetModelPath(), LoadManager::FileKind::kModel);
	manager.LoadHandle("Stage", m_pGameManager->GetStagePath(), LoadManager::FileKind::kModel);
	manager.LoadHandle("SkyDome", m_pGameManager->GetSkyDomePath(), LoadManager::FileKind::kModel);

}

void SceneGame::Update()
{

	for (auto& actor : m_pCharacters)
	{
		actor->Update();
	}

	m_pGameManager->Update();

	if (m_pGameManager->GetNextScene() == Game::Scene::kSelect)
	{
		//セレクトシーンに戻る
		m_sceneManager.ChangeScene(std::make_shared<SceneSelect>(m_sceneManager));
	}
	else if (m_pGameManager->GetNextScene() == Game::Scene::kMenu)
	{
		//メニューシーンに戻る
		m_sceneManager.ChangeScene(std::make_shared<SceneMenu>(m_sceneManager));
	}
}

void SceneGame::UpdateAsyncLoad()
{
	m_pGameManager->UpdateAsyncLoad();
}

void SceneGame::Draw()
{

#ifdef _DEBUG

	DrawString(0, 0, "SceneGame", kRedColor);

	Physics::GetInstance().DebugDraw();

#endif // _DEBUG

	for (auto& actor : m_pCharacters)
	{
		actor->Draw();
	}
	m_pGameManager->Draw();
}

void SceneGame::End()
{
	m_pGameManager->Final();
}

void SceneGame::SetCharacter(int player, int enemy)
{
	m_pGameManager = std::make_shared<BattleManager>(std::make_shared<GameCamera>());

	LoadCsv load;

	std::vector<std::vector<std::string>> data = load.LoadFile("data/csv/characterStatus.csv");

	m_pGameManager->SetOnePlayerStatus(player, data[player]);

	m_pGameManager->SetTwoPlayerStatus(enemy, data[enemy]);
}
