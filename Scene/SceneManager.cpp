#include "SceneManager.h"
#include "Input.h"
#include "Game.h"
#include "SceneTitle.h"
#include "DxLib.h"

namespace
{
	constexpr int kKnockOutFadeSpeed = 15;
	const int kBlack = GetColor(0,0,0);
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

void SceneManager::Update()
{

	//シーン切り替えフラグがたっていたら
	if (m_isChangeScene)
	{
		//フェードアウトする
		if (m_isFadeOut)
		{
			m_fadeAlpha += kKnockOutFadeSpeed;
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
			m_fadeAlpha -= kKnockOutFadeSpeed;
			if (m_fadeAlpha < 0)
			{
				m_fadeAlpha = 0;
				m_isChangeScene = false;
			}
		}
		//シーン切り替えが始まったら入力をさせないようにする
		MyEngine::Input::GetInstance().StopAllInput();
		m_pScene->Update();
	}
	//シーン切り替えしていないときの処理
	else
	{
		m_pScene->Update();
	}

}

void SceneManager::Draw()
{
	m_pScene->Draw();

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeAlpha);
	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, kBlack, true);
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
