#pragma once
#include "SceneBase.h"
class SceneSelect : public SceneBase
{
public:
	SceneSelect(SceneManager& sceneManager);
	virtual ~SceneSelect();
	//����������
	virtual void Init();
	//�X�V����
	virtual void Update(MyEngine::Input input);
	//�`�揈��
	virtual void Draw();
	//�I������
	virtual void End();
};

