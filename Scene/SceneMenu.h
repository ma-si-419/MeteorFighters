#pragma once
#include "SceneBase.h"
#include <memory>

class MenuUi;
class SceneMenu : public SceneBase
{
public:

	SceneMenu(SceneManager& sceneManager);
	virtual ~SceneMenu();
	//����������
	virtual void Init() override;
	//�X�V����
	virtual void Update() override;
	//�`�揈��
	virtual void Draw() override;
	//�I������
	virtual void End() override;

private:

	std::shared_ptr<MenuUi> m_pMenuUi;

};

