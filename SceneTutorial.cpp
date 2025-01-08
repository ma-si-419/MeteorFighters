#include "SceneTutorial.h"
#include "SceneMenu.h"
#include "GameCamera.h"
#include "TutorialManager.h"
#include "LoadManager.h"
#include "LoadCsv.h"
#include "GraphManager.h"

SceneTutorial::SceneTutorial(SceneManager& sceneManager) :
	SceneBase(sceneManager),
	m_isChangeScene(false)
{
	m_pGameManager = std::make_shared<TutorialManager>(std::make_shared<GameCamera>());

	LoadCsv load;

	std::vector<std::vector<std::string>> data = load.LoadFile("data/csv/characterStatus.csv");

	m_pGameManager->SetOnePlayerStatus(0, data[0]);

	m_pGameManager->SetTwoPlayerStatus(1, data[1]);
}

SceneTutorial::~SceneTutorial()
{
}

void SceneTutorial::Init()
{
	//画像のロード
	GraphManager::GetInstance().LoadSceneGraph("Tutorial");

	auto& loadManager = LoadManager::GetInstance();

	m_pCharacters.push_back(m_pGameManager->GetOnePlayerPointer());
	m_pCharacters.push_back(m_pGameManager->GetTwoPlayerPointer());

	m_pGameManager->Init();

	for (auto& actor : m_pCharacters)
	{
		actor->Init();
	}

	for (auto character : m_pCharacters)character->ChangeSituationUpdate(static_cast<int>(GameManagerBase::BattleSituation::kBattle));

	loadManager.StartAsyncLoad();

	loadManager.LoadHandle("Player1", m_pCharacters[0]->GetModelPath(), LoadManager::FileKind::kModel);
	loadManager.LoadHandle("Player2", m_pCharacters[1]->GetModelPath(), LoadManager::FileKind::kModel);
	loadManager.LoadHandle("Stage", m_pGameManager->GetStagePath(), LoadManager::FileKind::kModel);
	loadManager.LoadHandle("SkyDome", m_pGameManager->GetSkyDomePath(), LoadManager::FileKind::kModel);

}

void SceneTutorial::Update()
{
	for (auto& actor : m_pCharacters)
	{
		actor->Update();
	}

	m_pGameManager->Update();

	if (m_pGameManager->GetNextScene() == Game::Scene::kMenu &&
		!m_isChangeScene)
	{
		//メニューシーンに戻る
		m_sceneManager.ChangeScene(std::make_shared<SceneMenu>(m_sceneManager));
	
		m_isChangeScene = true;
	}
}

void SceneTutorial::UpdateAsyncLoad()
{
	m_pGameManager->UpdateAsyncLoad();
}

void SceneTutorial::Draw()
{
	for (auto& actor : m_pCharacters)
	{
		actor->Draw();
	}
	m_pGameManager->Draw();
}

void SceneTutorial::End()
{
	m_pGameManager->Final();
}
