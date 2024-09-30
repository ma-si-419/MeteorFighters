#include "SceneGame.h"
#include "DxLib.h"
#include "Player.h"

namespace
{
	const int kWhiteColor = GetColor(255, 255, 255);
}

SceneGame::SceneGame(SceneManager& sceneManager) :
	SceneBase(sceneManager)
{
	m_pActors.push_back(std::make_shared<Player>());
}

SceneGame::~SceneGame()
{
}

void SceneGame::Init()
{
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
