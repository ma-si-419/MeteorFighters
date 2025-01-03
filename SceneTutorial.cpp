#include "SceneTutorial.h"
#include "SceneMenu.h"
#include "GameCamera.h"
#include "TutorialManager.h"
#include "LoadCsv.h"
#include "GraphManager.h"

SceneTutorial::SceneTutorial(SceneManager& sceneManager) :
	SceneBase(sceneManager),
	m_isChangeScene(false)
{
	m_pManager = std::make_shared<TutorialManager>(std::make_shared<GameCamera>());

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
	GraphManager::GetInstance().LoadSceneGraph("Tutorial");

	m_pCharacters.push_back(m_pManager->GetOnePlayerPointer());
	m_pCharacters.push_back(m_pManager->GetTwoPlayerPointer());

	m_pManager->Init();

	for (auto& actor : m_pCharacters)
	{
		actor->Init();
	}

	for (auto character : m_pCharacters)character->ChangeSituationUpdate(static_cast<int>(GameManagerBase::BattleSituation::kBattle));
}

void SceneTutorial::Update()
{
	for (auto& actor : m_pCharacters)
	{
		actor->Update();
	}

	m_pManager->Update();

	if (m_pManager->GetNextScene() == Game::Scene::kMenu &&
		!m_isChangeScene)
	{
		//メニューシーンに戻る
		m_sceneManager.ChangeScene(std::make_shared<SceneMenu>(m_sceneManager));
	
		m_isChangeScene = true;
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
