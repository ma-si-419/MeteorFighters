#include "SceneTitle.h"
#include "SceneMenu.h"
#include "DxLib.h"
#include "Game.h"
#include "GraphManager.h"
namespace
{
	//文字を表示する座標
	constexpr int kStartStringPosX = 768;
	constexpr int kStartStringPosY = 432;


	const int kWhiteColor = GetColor(255, 255, 255);
}

SceneTitle::SceneTitle(SceneManager& sceneManager) :
	SceneBase(sceneManager)
{

}

SceneTitle::~SceneTitle()
{
}

void SceneTitle::Init()
{
	//画像のロード
	GraphManager::GetInstance().LoadSceneGraph("Title");
}

void SceneTitle::Update(MyEngine::Input input)
{

}


void SceneTitle::Draw()
{
	auto& graphManager = GraphManager::GetInstance();

	DrawGraph(0, 0, graphManager.GetHandle("タイトルロゴ"), true);

	DrawString(0, 0, "SceneTitle", kWhiteColor);
}

void SceneTitle::End()
{
}
