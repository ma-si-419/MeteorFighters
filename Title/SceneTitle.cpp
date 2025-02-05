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

	//BGMの音量
	constexpr int kBgmVolume = 255;
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

	//BGMを再生
	SoundManager::GetInstance().PlayLoopSound("Bgm");

	//BGMの音量を設定
	SoundManager::GetInstance().SetSoundVolume("Bgm", kBgmVolume);
}

void SceneTitle::Update()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);
	
	//タイトルUIの更新
	m_pTitleUi->Update();

	//何かキーが押されたら
	if (input->IsAnyPress())
	{
		//シーン遷移中なら何もしない
		if (m_sceneManager.IsChangeScene()) return;
		auto& soundManager = SoundManager::GetInstance();
		//SE再生
		soundManager.PlayOnceSound("Ok");
		//BGMを止める
		soundManager.StopLoopSound("Bgm");
		//シーン遷移
		m_sceneManager.ChangeScene(std::make_shared<SceneMenu>(m_sceneManager));
	}

}

void SceneTitle::UpdateAsyncLoad()
{
}


void SceneTitle::Draw()
{
	m_pTitleUi->Draw();
}

void SceneTitle::End()
{
}
