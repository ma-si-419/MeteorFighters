#pragma once
#include "SceneBase.h"
class SceneMenu : public SceneBase
{
public:
	SceneMenu(SceneManager& sceneManager);
	virtual ~SceneMenu();
	//����������
	virtual void Init();
	//�X�V����
	virtual void Update();
	//�`�揈��
	virtual void Draw();
	//�I������
	virtual void End();

};

