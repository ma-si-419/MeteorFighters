#pragma once
#include "SceneBase.h"
class SceneGame : public SceneBase
{
public:
	SceneGame(SceneManager& sceneManager);
	virtual ~SceneGame();
	//����������
	virtual void Init();
	//�X�V����
	virtual void Update(MyEngine::Input input);
	//�`�揈��
	virtual void Draw();
	//�I������
	virtual void End();
};

