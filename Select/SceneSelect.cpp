#include "SceneSelect.h"
#include "SceneGame.h"
#include "SceneMenu.h"
#include "DxLib.h"
#include "Input.h"
#include "SoundManager.h"
#include "GraphManager.h"
#include "SelectManager.h"
#include <string>
#include "Game.h"	

namespace
{
	const int kRedColor = GetColor(255, 0, 0);

}

SceneSelect::SceneSelect(SceneManager& sceneManager) :
	SceneBase(sceneManager)
{
	m_selectManager = std::make_shared<SelectManager>();
}

SceneSelect::~SceneSelect()
{
}

void SceneSelect::Init()
{
	SoundManager::GetInstance().LoadSceneSound("Select");
	GraphManager::GetInstance().LoadSceneGraph("Select");
	m_selectManager->Init();
}

void SceneSelect::Update()
{
	//マネージャーの更新
	m_selectManager->Update();

	//次のシーン
	auto nextScene = static_cast<Game::Scene>(m_selectManager->GetNextScene());

	//シーン変更が行われていなければ
	if (!m_sceneManager.IsChangeScene())
	{
		//ゲームシーンの場合
		if (nextScene == Game::Scene::kBattle)
		{
			std::shared_ptr<SceneGame> next = std::make_shared<SceneGame>(m_sceneManager);

			next->SetCharacter(m_selectManager->GetPlayerNumber(), m_selectManager->GetEnemyNumber(),m_selectManager->GetEnemyLevel());

			next->SetMusicName("Bgm" + std::to_string(m_selectManager->GetBgmNumber()));

			m_sceneManager.ChangeScene(next);
		}
		//メニューシーンの場合
		else if (nextScene == Game::Scene::kMenu)
		{
			std::shared_ptr<SceneMenu> next = std::make_shared<SceneMenu>(m_sceneManager);

			m_sceneManager.ChangeScene(next);
		}
	}
}

void SceneSelect::UpdateAsyncLoad()
{
}

void SceneSelect::Draw()
{
	m_selectManager->Draw();
}

void SceneSelect::End()
{
}
