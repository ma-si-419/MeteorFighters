#include "SceneTitle.h"
#include "DxLib.h"

SceneTitle::SceneTitle(SceneManager& sceneManager):
	SceneBase(sceneManager)
{

}

SceneTitle::~SceneTitle()
{
}

void SceneTitle::Init()
{
}

void SceneTitle::Update(MyEngine::Input input)
{
}

void SceneTitle::Draw()
{
	DrawString(0, 0, "SceneTitle",GetColor(255,255,255));
}

void SceneTitle::End()
{
}
