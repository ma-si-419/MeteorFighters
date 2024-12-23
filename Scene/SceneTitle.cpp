#include "SceneTitle.h"
#include "SceneMenu.h"
#include "DxLib.h"
#include "Game.h"
#include "GraphManager.h"
#include "SoundManager.h"
#include "Input.h"
#include "TitleUi.h"

namespace
{
	//文字を表示する座標
	constexpr int kStartStringPosX = 768;
	constexpr int kStartStringPosY = 432;


	const int kRedColor = GetColor(255, 0, 0);
}

SceneTitle::SceneTitle(SceneManager& sceneManager) :
	SceneBase(sceneManager)
{
	m_pTitleUi = std::make_shared<TitleUi>();
}

SceneTitle::~SceneTitle()
{
}

void SceneTitle::Init()
{
	//画像のロード
	GraphManager::GetInstance().LoadSceneGraph("Title");
	SoundManager::GetInstance().LoadSceneSound("Title");

	m_pTitleUi->Init();
}

void SceneTitle::Update()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	if (input->IsTrigger("A"))
	{
		SoundManager::GetInstance().OncePlaySound("Ok");
		m_sceneManager.ChangeScene(std::make_shared<SceneMenu>(m_sceneManager));
	}

	m_pTitleUi->Update();
}


void SceneTitle::Draw()
{
	m_pTitleUi->Draw();
}

void SceneTitle::End()
{
}
