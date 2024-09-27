#include "SceneTitle.h"
#include "SceneMenu.h"
#include "DxLib.h"
namespace
{
	const int kWhiteColor = GetColor(255, 255, 255);
}

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
	if (input.IsTrigger("A"))
	{
		m_sceneManager.ChangeScene(std::make_shared<SceneMenu>(m_sceneManager));
	}
}

void SceneTitle::Draw()
{
	DrawString(0, 0, "SceneTitle",kWhiteColor);
}

void SceneTitle::End()
{
}
