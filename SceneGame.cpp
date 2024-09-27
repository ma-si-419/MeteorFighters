#include "SceneGame.h"
#include "DxLib.h"

namespace
{
	const int kWhiteColor = GetColor(255,255,255);
}

SceneGame::SceneGame(SceneManager& sceneManager):
	SceneBase(sceneManager)
{
}

SceneGame::~SceneGame()
{
}

void SceneGame::Init()
{
}

void SceneGame::Update(MyEngine::Input input)
{
}

void SceneGame::Draw()
{
	DrawString(0,0,"SceneGame",kWhiteColor);
}

void SceneGame::End()
{
}
