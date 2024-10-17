#include "SceneGame.h"
#include "DxLib.h"
#include "Player.h"
#include "Enemy.h"
#include "GameManager.h"
#include "Physics.h"
#include "LoadCsv.h"

namespace
{
	const int kWhiteColor = GetColor(255, 255, 255);
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
	m_pActors.push_back(m_pGameManager->GetPlayerPointer());
	m_pActors.push_back(m_pGameManager->GetEnemyPointer());

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
}

void SceneGame::Draw()
{
#ifdef _DEBUG

	DrawString(0, 0, "SceneGame", kWhiteColor);

	Physics::GetInstance().DebugDraw();

#endif // _DEBUG


	for (auto& actor : m_pActors)
	{
		actor->Draw();
	}
}

void SceneGame::End()
{
}

void SceneGame::SetCharacter(int player, int enemy)
{
	m_pGameManager = std::make_shared<GameManager>();

	LoadCsv load;

	std::vector<std::vector<std::string>> data = load.LoadFile("data/csv/characterStatus.csv");

	m_pGameManager->SetPlayerStatus(player,data[player]);

	m_pGameManager->SetEnemyStatus(enemy,data[enemy]);
}
