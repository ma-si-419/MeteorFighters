#pragma once
#include "SceneBase.h"
class SceneTitle : public SceneBase
{
public:
	SceneTitle(SceneManager& sceneManager);
	virtual ~SceneTitle();
	//����������
	virtual void Init() override;
	//�X�V����
	virtual void Update() override;
	//�`�揈��
	virtual void Draw() override;
	//�I������
	virtual void End() override;

};

