#include "SceneMenu.h"
#include "SceneSelect.h"
#include "SceneTitle.h"
#include "SceneTutorial.h"
#include "DxLib.h"
#include "Input.h"
#include "SoundManager.h"
#include "GraphManager.h"
#include "MenuUi.h"

SceneMenu::SceneMenu(SceneManager& sceneManager) :
	SceneBase(sceneManager)
{
	m_pMenuUi = std::make_shared<MenuUi>();
}

SceneMenu::~SceneMenu()
{
}

void SceneMenu::Init()
{
	GraphManager::GetInstance().LoadSceneGraph("Menu");
	SoundManager::GetInstance().LoadSceneSound("Menu");
	m_pMenuUi->Init();

	//Bgm‚ðÄ¶
	m_bgmPlayHandle = SoundManager::GetInstance().PlayLoopSound("Bgm");
}

void SceneMenu::Update()
{
	int selectItem = m_pMenuUi->Update();

	if (selectItem == static_cast<int>(MenuUi::SelectItem::kBattle))
	{
		m_sceneManager.ChangeScene(std::make_shared<SceneSelect>(m_sceneManager));
	}
	else if (selectItem == static_cast<int>(MenuUi::SelectItem::kTutorial))
	{
		m_sceneManager.ChangeScene(std::make_shared<SceneTutorial>(m_sceneManager));
	}
	else if (selectItem == static_cast<int>(MenuUi::SelectItem::kTitle))
	{
		m_sceneManager.ChangeScene(std::make_shared<SceneTitle>(m_sceneManager));
	}
	else if(selectItem == static_cast<int>(MenuUi::SelectItem::kEndGame))
	{
		m_sceneManager.GameEnd();
		return;
	}
}

void SceneMenu::UpdateAsyncLoad()
{
}

void SceneMenu::Draw()
{
#ifdef _DEBUG

	DrawString(0, 0, "SceneMenu", GetColor(255,0,0));

#endif // _DEBUG

	m_pMenuUi->DrawModel();

	m_pMenuUi->DrawItem();
}

void SceneMenu::End()
{
	SoundManager::GetInstance().StopLoopSound("Bgm");
}
