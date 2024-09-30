#include "SceneSelect.h"
#include "SceneGame.h"
#include "DxLib.h"
#include "Input.h"

namespace
{
	const int kWhiteColor = GetColor(255, 255, 255);
}

SceneSelect::SceneSelect(SceneManager& sceneManager) :
	SceneBase(sceneManager)
{
}

SceneSelect::~SceneSelect()
{
}

void SceneSelect::Init()
{
}

void SceneSelect::Update()
{
	if (MyEngine::Input::GetInstance().IsTrigger("A"))
	{
		m_sceneManager.ChangeScene(std::make_shared<SceneGame>(m_sceneManager));
	}
}

void SceneSelect::Draw()
{
#ifdef _DEBUG

	DrawString(0, 0, "SceneSelect", kWhiteColor);

#endif // _DEBUG
}

void SceneSelect::End()
{
}
