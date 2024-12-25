#include "SceneTutorial.h"
#include "SceneMenu.h"
#include "GameCamera.h"
#include "BattleManager.h"
#include "LoadCsv.h"
#include "GraphManager.h"

SceneTutorial::SceneTutorial(SceneManager& sceneManager) :
	SceneBase(sceneManager)
{
	m_pManager = std::make_shared<BattleManager>(std::make_shared<GameCamera>());

	LoadCsv load;

	std::vector<std::vector<std::string>> data = load.LoadFile("data/csv/characterStatus.csv");

	m_pManager->SetOnePlayerStatus(0, data[0]);

	m_pManager->SetTwoPlayerStatus(1, data[1]);
}

SceneTutorial::~SceneTutorial()
{
}

void SceneTutorial::Init()
{
	//画像のロード
	GraphManager::GetInstance().LoadSceneGraph("Game");

	m_pCharacters.push_back(m_pManager->GetOnePlayerPointer());
	m_pCharacters.push_back(m_pManager->GetTwoPlayerPointer());

	m_pManager->Init();

	for (auto& actor : m_pCharacters)
	{
		actor->Init();
	}
}

void SceneTutorial::Update()
{
	for (auto& actor : m_pCharacters)
	{
		actor->Update();
	}

	m_pManager->Update();

	if (m_pManager->GetNextScene() == Game::Scene::kMenu)
	{
		//メニューシーンに戻る
		m_sceneManager.ChangeScene(std::make_shared<SceneMenu>(m_sceneManager));
	}
}

void SceneTutorial::Draw()
{
	for (auto& actor : m_pCharacters)
	{
		actor->Draw();
	}
	m_pManager->Draw();
}

void SceneTutorial::End()
{
	m_pManager->Final();
}
