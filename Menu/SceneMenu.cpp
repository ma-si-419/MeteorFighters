#include "SceneMenu.h"
#include "SceneSelect.h"
#include "SceneTitle.h"
#include "SceneTutorial.h"
#include "DxLib.h"
#include "Input.h"
#include "SoundManager.h"
#include "GraphManager.h"
#include "MenuManager.h"
#include "Game.h"

SceneMenu::SceneMenu(SceneManager& sceneManager) :
	SceneBase(sceneManager)
{
	m_pMenuManager = std::make_shared<MenuManager>();
}

SceneMenu::~SceneMenu()
{
}

void SceneMenu::Init()
{
	GraphManager::GetInstance().LoadSceneGraph("Menu");
	SoundManager::GetInstance().LoadSceneSound("Menu");
	m_pMenuManager->Init();

	//Bgmを再生
	SoundManager::GetInstance().PlayLoopSound("Bgm");
}

void SceneMenu::Update()
{
	m_pMenuManager->Update();

	//シーン遷移中なら何もしない
	if (m_sceneManager.IsChangeScene()) return;

	//次のシーン
	if (m_pMenuManager->GetNextScene() == static_cast<int>(Game::Scene::kSelect))
	{
		std::shared_ptr<SceneSelect> next = std::make_shared<SceneSelect>(m_sceneManager);
		m_sceneManager.ChangeScene(next);
	}
	else if (m_pMenuManager->GetNextScene() == static_cast<int>(Game::Scene::kTitle))
	{
		std::shared_ptr<SceneTitle> next = std::make_shared<SceneTitle>(m_sceneManager);
		m_sceneManager.ChangeScene(next);
	}
	else if (m_pMenuManager->GetNextScene() == static_cast<int>(Game::Scene::kTutorial))
	{
		std::shared_ptr<SceneTutorial> next = std::make_shared<SceneTutorial>(m_sceneManager);
		m_sceneManager.ChangeScene(next);
	}
	else if (m_pMenuManager->GetNextScene() == static_cast<int>(Game::Scene::kEnd))
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
	m_pMenuManager->Draw();
}

void SceneMenu::End()
{
	SoundManager::GetInstance().StopLoopSound("Bgm");
}
