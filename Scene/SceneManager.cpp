#include "SceneManager.h"
#include "LoadManager.h"
#include "Input.h"
#include "Game.h"
#include "SceneTitle.h"
#include "DxLib.h"

namespace
{
	constexpr int kKnockOutFadeSpeed = 15;
	const int kBlack = GetColor(0, 0, 0);
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

	//�V�[���؂�ւ��t���O�������Ă�����
	if (m_isChangeScene)
	{

		//�t�F�[�h�A�E�g����
		if (m_isFadeOut)
		{

			//�V�[���؂�ւ����n�܂�������͂������Ȃ��悤�ɂ���
			MyEngine::Input::GetInstance().StopAllInput();
			m_pScene->Update();
			m_fadeAlpha += kKnockOutFadeSpeed;
			if (m_fadeAlpha > 255)
			{
				m_pScene->End();
				m_pScene = m_pNextScene;
				m_pScene->Init();
				m_isFadeOut = false;
			}
		}
		else
		{
			//�񓯊����[�h���Ă��Ȃ���΃t�F�[�h�C������
			if (LoadManager::GetInstance().IsEndLoad())
			{
				MyEngine::Input::GetInstance().StopAllInput();
				m_pScene->Update();
				m_fadeAlpha -= kKnockOutFadeSpeed;
				if (m_fadeAlpha < 0)
				{
					m_fadeAlpha = 0;
					m_isChangeScene = false;
				}
			}
			else
			{
				//�񓯊����[�h���̏���
				m_pScene->UpdateAsyncLoad();
			}
		}

	}
	//�V�[���؂�ւ����Ă��Ȃ��Ƃ��̏���
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

void SceneManager::SetAsyncLoad(std::vector<int> handles)
{
	m_handles = handles;
}
