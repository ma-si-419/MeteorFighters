#pragma once
#include <memory>
#include "SceneBase.h"

class SelectManager;
class SceneSelect : public SceneBase
{
public:
	SceneSelect(SceneManager& sceneManager);
	virtual ~SceneSelect();
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

	std::shared_ptr<SelectManager> m_selectManager;

};

