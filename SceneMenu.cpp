#include "SceneMenu.h"
#include "SceneSelect.h"
#include "DxLib.h"

namespace
{
	const int kWhiteColor = GetColor(255, 255, 255);
}

SceneMenu::SceneMenu(SceneManager& sceneManager):
	SceneBase(sceneManager)
{
}

SceneMenu::~SceneMenu()
{
}

void SceneMenu::Init()
{
}

void SceneMenu::Update(MyEngine::Input input)
{
	if (input.IsTrigger("A"))
	{
		m_sceneManager.ChangeScene(std::make_shared<SceneSelect>(m_sceneManager));
	}
}

void SceneMenu::Draw()
{
	DrawString(0,0,"SceneMenu",kWhiteColor);
}

void SceneMenu::End()
{
}
