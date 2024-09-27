#include "SceneSelect.h"
#include "SceneGame.h"
#include "DxLib.h"

namespace
{
	const int kWhiteColor = GetColor(255, 255, 255);
}

SceneSelect::SceneSelect(SceneManager& sceneManager):
	SceneBase(sceneManager)
{
}

SceneSelect::~SceneSelect()
{
}

void SceneSelect::Init()
{
}

void SceneSelect::Update(MyEngine::Input input)
{
	if (input.IsTrigger("A"))
	{
		m_sceneManager.ChangeScene(std::make_shared<SceneGame>(m_sceneManager));
	}
}

void SceneSelect::Draw()
{
	DrawString(0, 0, "SceneMenu", kWhiteColor);
}

void SceneSelect::End()
{
}
