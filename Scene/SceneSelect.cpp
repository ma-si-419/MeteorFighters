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
		//�Q�[���V�[���̃|�C���^�쐬
		std::shared_ptr<SceneGame> scene = std::make_shared<SceneGame>(m_sceneManager);
		//�Q�[���V�[���ɑI�����Ă���L�����N�^�[����^����
		scene->SetCharacter(m_playerNumber,m_enemyNumber);
		//�Q�[���V�[���Ɉڍs����
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
