#include "SceneSelect.h"
#include "SceneGame.h"
#include "DxLib.h"
#include "Input.h"

namespace
{
	const int kRedColor = GetColor(255, 0, 0);
}

SceneSelect::SceneSelect(SceneManager& sceneManager) :
	SceneBase(sceneManager),
	m_playerNumber(0),
	m_enemyNumber(0)
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
		//ゲームシーンのポインタ作成
		std::shared_ptr<SceneGame> scene = std::make_shared<SceneGame>(m_sceneManager);
		//ゲームシーンに選択しているキャラクター情報を与える
		scene->SetCharacter(m_playerNumber,m_enemyNumber);
		//ゲームシーンに移行する
		m_sceneManager.ChangeScene(scene);
	}
}

void SceneSelect::Draw()
{
#ifdef _DEBUG

	DrawString(0, 0, "SceneSelect", kRedColor);

#endif // _DEBUG
}

void SceneSelect::End()
{
}
