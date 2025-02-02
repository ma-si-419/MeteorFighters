#include "SceneTutorial.h"
#include "SceneMenu.h"
#include "GameCamera.h"
#include "TutorialManager.h"
#include "LoadManager.h"
#include "SoundManager.h"
#include "LoadCsv.h"
#include "GraphManager.h"
#include "Physics.h"

SceneTutorial::SceneTutorial(SceneManager& sceneManager) :
	SceneBase(sceneManager),
	m_isChangeScene(false)
{
	m_pGameManager = std::make_shared<TutorialManager>(std::make_shared<GameCamera>());

	LoadCsv load;

	std::vector<std::vector<std::string>> data = load.LoadFile("data/csv/characterStatus.csv");

	m_pGameManager->SetOnePlayerStatus(0, data[0]);

	m_pGameManager->SetTwoPlayerStatus(1, data[1],0);
}

SceneTutorial::~SceneTutorial()
{
}

void SceneTutorial::Init()
{
	//画像のロード
	GraphManager::GetInstance().LoadSceneGraph("Tutorial");

	//音声のロード
	SoundManager::GetInstance().LoadSceneSound("Tutorial");

	//Physicsを動かす
	Physics::GetInstance().StartUpdate();

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

	//返り値を受け取るマップ作成
	m_pGameManager->AddLoadModel("Player1");
	m_pGameManager->AddLoadModel("Player2");
	m_pGameManager->AddLoadModel("Stage");
	m_pGameManager->AddLoadModel("SkyDome");
	m_pGameManager->AddLoadModel("Rock1");
	m_pGameManager->AddLoadModel("Rock2");
	m_pGameManager->AddLoadModel("Rock3");

	loadManager.LoadHandle("Player1", m_pCharacters[0]->GetModelPath(), LoadManager::FileKind::kModel);
	loadManager.LoadHandle("Player2", m_pCharacters[1]->GetModelPath(), LoadManager::FileKind::kModel);
	loadManager.LoadHandle("Stage", m_pGameManager->GetStagePath(), LoadManager::FileKind::kModel);
	loadManager.LoadHandle("SkyDome", m_pGameManager->GetSkyDomePath(), LoadManager::FileKind::kModel);
	loadManager.LoadHandle("Rock1", "data/model/Rock1.mv1", LoadManager::FileKind::kModel);
	loadManager.LoadHandle("Rock2", "data/model/Rock2.mv1", LoadManager::FileKind::kModel);
	loadManager.LoadHandle("Rock3", "data/model/Rock3.mv1", LoadManager::FileKind::kModel);
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
