#pragma once
#include "SceneBase.h"
#include <memory>

class MenuManager;
class SceneMenu : public SceneBase
{
public:

	SceneMenu(SceneManager& sceneManager);
	virtual ~SceneMenu();
	//����������
	virtual void Init() override;
	//�X�V����
	virtual void Update() override;
	//�񓯊��������s���Ă���Ƃ��̃A�b�v�f�[�g
	virtual void UpdateAsyncLoad() override;
	//�`�揈��
	virtual void Draw() override;
	//�I������
	virtual void End() override;

private:

	std::shared_ptr<MenuManager> m_pMenuManager;
};