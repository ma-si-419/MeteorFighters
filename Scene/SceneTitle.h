#pragma once
#include "SceneBase.h"
class SceneTitle : public SceneBase
{
public:
	SceneTitle(SceneManager& sceneManager);
	virtual ~SceneTitle();
	//����������
	virtual void Init();
	//�X�V����
	virtual void Update();
	//�`�揈��
	virtual void Draw();
	//�I������
	virtual void End();

};

