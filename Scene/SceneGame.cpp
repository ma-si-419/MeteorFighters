#include "SceneGame.h"
#include "DxLib.h"
#include "Player.h"
#include "Enemy.h"
#include "GameManager.h"

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
	m_pGameManager = std::make_shared<GameManager>();

	std::shared_ptr<Player> player = std::make_shared<Player>();
	std::shared_ptr<Enemy> enemy = std::make_shared<Enemy>();

	player->SetGameManager(m_pGameManager);
	enemy->SetGameManager(m_pGameManager);

	m_pGameManager->SetPlayer(player);
	m_pGameManager->SetEnemy(enemy);

	m_pActors.push_back(player);
	m_pActors.push_back(enemy);

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
}

void SceneGame::Draw()
{
#ifdef _DEBUG

	DrawString(0, 0, "SceneGame", kWhiteColor);

#endif // _DEBUG


	for (auto& actor : m_pActors)
	{
		actor->Draw();
	}
}

void SceneGame::End()
{
}
