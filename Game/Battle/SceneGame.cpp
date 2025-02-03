#include "SceneGame.h"
#include "SceneSelect.h"
#include "SceneMenu.h"
#include "DxLib.h"
#include "Character.h"
#include "BattleManager.h"
#include "GraphManager.h"
#include "SoundManager.h"
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
	//�摜�̃��[�h
	GraphManager::GetInstance().LoadSceneGraph("Game");
	//�����̃��[�h
	SoundManager::GetInstance().LoadSceneSound("Game");

	//�񓯊����[�h�}�l�[�W���[
	auto& loadManager = LoadManager::GetInstance();

	m_pCharacters.push_back(m_pGameManager->GetOnePlayerPointer());
	m_pCharacters.push_back(m_pGameManager->GetTwoPlayerPointer());

	//���f���̃p�X�ݒ�
	for (auto& actor : m_pCharacters)
	{
		actor->Init();
	}

	loadManager.StartAsyncLoad();

	//�Ԃ�l���󂯎��}�b�v�쐬
	m_pGameManager->AddLoadModel("Player1");
	m_pGameManager->AddLoadModel("Player2");
	m_pGameManager->AddLoadModel("Stage");
	m_pGameManager->AddLoadModel("SkyDome");
	m_pGameManager->AddLoadModel("Rock1");
	m_pGameManager->AddLoadModel("Rock2");
	m_pGameManager->AddLoadModel("Rock3");

	//���f���̃��[�h���}�l�[�W���[�ɂ��肢����
	loadManager.LoadHandle("Player1", m_pCharacters[0]->GetModelPath(), LoadManager::FileKind::kModel);
	loadManager.LoadHandle("Player2", m_pCharacters[1]->GetModelPath(), LoadManager::FileKind::kModel);
	loadManager.LoadHandle("Stage", m_pGameManager->GetStagePath(), LoadManager::FileKind::kModel);
	loadManager.LoadHandle("SkyDome", m_pGameManager->GetSkyDomePath(), LoadManager::FileKind::kModel);
	loadManager.LoadHandle("Rock1", "data/model/Rock1.mv1", LoadManager::FileKind::kModel);
	loadManager.LoadHandle("Rock2", "data/model/Rock2.mv1", LoadManager::FileKind::kModel);
	loadManager.LoadHandle("Rock3", "data/model/Rock3.mv1", LoadManager::FileKind::kModel);

}

void SceneGame::Update()
{

	for (auto& actor : m_pCharacters)
	{
		actor->Update();
	}

	m_pGameManager->Update();

	//�V�[���ړ������Ă��Ȃ���Ύ��s����
	if (!m_sceneManager.IsChangeScene())
	{
		//�V�[���ړ�
		if (m_pGameManager->GetNextScene() == Game::Scene::kSelect)
		{
			//�Z���N�g�V�[���ɖ߂�
			m_sceneManager.ChangeScene(std::make_shared<SceneSelect>(m_sceneManager));
		}
		else if (m_pGameManager->GetNextScene() == Game::Scene::kMenu)
		{
			//���j���[�V�[���ɖ߂�
			m_sceneManager.ChangeScene(std::make_shared<SceneMenu>(m_sceneManager));
		}
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

	//Physics::GetInstance().DebugDraw();

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

void SceneGame::SetCharacter(int player, int enemy, int enemyLevel)
{
	m_pGameManager = std::make_shared<BattleManager>(std::make_shared<GameCamera>());

	LoadCsv load;

	std::vector<std::vector<std::string>> data = load.LoadFile("data/csv/characterStatus.csv");

	m_pGameManager->SetOnePlayerStatus(player, data[player]);

	m_pGameManager->SetTwoPlayerStatus(enemy, data[enemy], enemyLevel);
}

void SceneGame::SetMusicName(std::string musicName)
{
	auto battleManager = std::dynamic_pointer_cast<BattleManager>(m_pGameManager);

	battleManager->SetMusicName(musicName);
}
