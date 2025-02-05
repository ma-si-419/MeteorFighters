#include "MenuManager.h"
#include "MenuUi.h"
#include "Game.h"

MenuManager::MenuManager() :
	m_nextScene(-1)
{
}

MenuManager::~MenuManager()
{
}

void MenuManager::Init()
{
	m_pMenuUi = std::make_shared<MenuUi>();
	m_pMenuUi->Init();
}

void MenuManager::Update()
{
	int selectItem = m_pMenuUi->Update();

	if (selectItem == static_cast<int>(MenuUi::SelectItem::kSelect))
	{
		m_nextScene = static_cast<int>(Game::Scene::kSelect);
	}
	else if (selectItem == static_cast<int>(MenuUi::SelectItem::kTutorial))
	{
		m_nextScene = static_cast<int>(Game::Scene::kTutorial);
	}
	else if (selectItem == static_cast<int>(MenuUi::SelectItem::kEndGame))
	{
		m_nextScene = static_cast<int>(Game::Scene::kEnd);
	}
}

void MenuManager::Draw()
{
	m_pMenuUi->DrawModel();
	m_pMenuUi->DrawItem();
}

