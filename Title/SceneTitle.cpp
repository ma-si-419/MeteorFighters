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
	//������\��������W
	constexpr int kStartStringPosX = 768;
	constexpr int kStartStringPosY = 432;


	const int kRedColor = GetColor(255, 0, 0);

	//BGM�̉���
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
	//�摜�̃��[�h
	GraphManager::GetInstance().LoadSceneGraph("Title");
	SoundManager::GetInstance().LoadSceneSound("Title");

	m_pTitleUi->Init();

	//BGM���Đ�
	SoundManager::GetInstance().PlayLoopSound("Bgm");

	//BGM�̉��ʂ�ݒ�
	SoundManager::GetInstance().SetSoundVolume("Bgm", kBgmVolume);
}

void SceneTitle::Update()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);
	
	//�^�C�g��UI�̍X�V
	m_pTitleUi->Update();

	//�����L�[�������ꂽ��
	if (input->IsAnyPress())
	{
		//�V�[���J�ڒ��Ȃ牽�����Ȃ�
		if (m_sceneManager.IsChangeScene()) return;
		auto& soundManager = SoundManager::GetInstance();
		//SE�Đ�
		soundManager.PlayOnceSound("Ok");
		//BGM���~�߂�
		soundManager.StopLoopSound("Bgm");
		//�V�[���J��
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
