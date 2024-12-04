#include "SceneGame.h"
#include "SceneSelect.h"
#include "SceneMenu.h"
#include "DxLib.h"
#include "CharacterBase.h"
#include "GameManager.h"
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

	m_pActors.push_back(m_pGameManager->GetOnePlayerPointer());
	m_pActors.push_back(m_pGameManager->GetTwoPlayerPointer());
	
	m_pGameManager->Init();

	for (auto& actor : m_pActors)
	{
		actor->Init();
	}
}

void SceneGame::Update()
{

	for (auto& actor : m_pActors)
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

void SceneGame::Draw()
{

#ifdef _DEBUG

	DrawString(0, 0, "SceneGame",kRedColor);

	Physics::GetInstance().DebugDraw();

#endif // _DEBUG

	for (auto& actor : m_pActors)
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
	m_pGameManager = std::make_shared<GameManager>(std::make_shared<GameCamera>());

	LoadCsv load;

	std::vector<std::vector<std::string>> data = load.LoadFile("data/csv/characterStatus.csv");

	m_pGameManager->SetOnePlayerStatus(player, data[player]);

	m_pGameManager->SetTwoPlayerStatus(enemy, data[enemy]);
}
