#include "SceneMenu.h"
#include "SceneSelect.h"
#include "DxLib.h"
#include "Input.h"

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

void SceneMenu::Update()
{
	if (MyEngine::Input::GetInstance().IsTrigger("A"))
	{
		m_sceneManager.ChangeScene(std::make_shared<SceneSelect>(m_sceneManager));
	}
}

void SceneMenu::Draw()
{
#ifdef _DEBUG
	
	DrawString(0,0,"SceneMenu",kWhiteColor);

#endif // _DEBUG

}

void SceneMenu::End()
{
}
