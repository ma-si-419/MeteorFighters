#include "SceneManager.h"
#include "Input.h"
#include "Game.h"
#include "SceneTitle.h"
#include "DxLib.h"

namespace
{
	constexpr int kFadeSpeed = 3;
}

SceneManager::SceneManager() :
	m_isEnd(false),
	m_isFadeOut(false),
	m_fadeAlpha(0),
	m_isChangeScene(false)
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Init()
{
	m_pScene = std::make_shared<SceneTitle>(*this);
	m_pScene->Init();
}

void SceneManager::Update(MyEngine::Input input)
{

	//シーン切り替えフラグがたっていたら
	if (m_isChangeScene)
	{
		//フェードアウトする
		if (m_isFadeOut)
		{
			m_fadeAlpha += kFadeSpeed;
			if (m_fadeAlpha > 255)
			{
				m_pScene->End();
				m_pScene = m_pNextScene;
				m_pScene->Init();
				m_isFadeOut = false;
			}
		}
		//フェードインする
		else
		{
			m_fadeAlpha -= kFadeSpeed;
			if (m_fadeAlpha < 0)
			{
				m_fadeAlpha = 0;
				m_isChangeScene = false;
			}
		}
		//シーン切り替えが始まったら入力をさせないようにする
		MyEngine::Input none;
		m_pScene->Update(none);
	}
	//シーン切り替えしていないときの処理
	else
	{
		m_pScene->Update(input);
	}

}

void SceneManager::Draw()
{
	m_pScene->Draw();

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeAlpha);
	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, GetColor(0, 0, 0), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
void SceneManager::ChangeScene(std::shared_ptr<SceneBase> next)
{
	if (m_pScene)
	{
		m_isChangeScene = true;
	}
	m_isFadeOut = true;
	m_pNextScene = next;

}
