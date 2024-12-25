#include "SceneGame.h"
#include "SceneSelect.h"
#include "SceneMenu.h"
#include "DxLib.h"
#include "Character.h"
#include "BattleManager.h"
#include "GraphManager.h"
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

	m_pCharacters.push_back(m_pBattleManager->GetOnePlayerPointer());
	m_pCharacters.push_back(m_pBattleManager->GetTwoPlayerPointer());
	
	m_pBattleManager->Init();

	for (auto& actor : m_pCharacters)
	{
		actor->Init();
	}
}

void SceneGame::Update()
{

	for (auto& actor : m_pCharacters)
	{
		actor->Update();
	}

	m_pBattleManager->Update();

	if (m_pBattleManager->GetNextScene() == Game::Scene::kSelect)
	{
		//セレクトシーンに戻る
		m_sceneManager.ChangeScene(std::make_shared<SceneSelect>(m_sceneManager));
	}
	else if (m_pBattleManager->GetNextScene() == Game::Scene::kMenu)
	{
		//メニューシーンに戻る
		m_sceneManager.ChangeScene(std::make_shared<SceneMenu>(m_sceneManager));
	}
}

void SceneGame::Draw()
{

#ifdef _DEBUG

	DrawString(0, 0, "SceneGame",kRedColor);

	Physics::GetInstance().DebugDraw();

#endif // _DEBUG

	for (auto& actor : m_pCharacters)
	{
		actor->Draw();
	}
	m_pBattleManager->Draw();
}

void SceneGame::End()
{
	m_pBattleManager->Final();
}

void SceneGame::SetCharacter(int player, int enemy)
{
	m_pBattleManager = std::make_shared<BattleManager>(std::make_shared<GameCamera>());

	LoadCsv load;

	std::vector<std::vector<std::string>> data = load.LoadFile("data/csv/characterStatus.csv");

	m_pBattleManager->SetOnePlayerStatus(player, data[player]);

	m_pBattleManager->SetTwoPlayerStatus(enemy, data[enemy]);
}
