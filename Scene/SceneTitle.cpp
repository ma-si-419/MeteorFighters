#include "SceneTitle.h"
#include "SceneMenu.h"
#include "DxLib.h"
#include "Game.h"
#include "GraphManager.h"
#include "Input.h"

namespace
{
	//������\��������W
	constexpr int kStartStringPosX = 768;
	constexpr int kStartStringPosY = 432;


	const int kWhiteColor = GetColor(255, 255, 255);
}

SceneTitle::SceneTitle(SceneManager& sceneManager) :
	SceneBase(sceneManager)
{

}

SceneTitle::~SceneTitle()
{
}

void SceneTitle::Init()
{
	//�摜�̃��[�h
	GraphManager::GetInstance().LoadSceneGraph("Title");
}

void SceneTitle::Update()
{
	if (MyEngine::Input::GetInstance().IsTrigger("A"))
	{
		m_sceneManager.ChangeScene(std::make_shared<SceneMenu>(m_sceneManager));
	}
}


void SceneTitle::Draw()
{
	auto& graphManager = GraphManager::GetInstance();

#ifdef _DEBUG

	DrawGraph(0, 0, graphManager.GetHandle("�^�C�g�����S"), true);

	DrawString(0, 0, "SceneTitle", kWhiteColor);

#endif // _DEBUG
}

void SceneTitle::End()
{
}
