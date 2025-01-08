#pragma once
#include "SceneManager.h"
class Input;
class SceneBase
{
public:
	SceneBase(SceneManager& sceneManager) : m_sceneManager(sceneManager) {}
	virtual ~SceneBase() {}
	//����������
	virtual void Init() abstract;
	//�X�V����
	virtual void Update() abstract;
	//�񓯊��������s���Ă���Ƃ��̃A�b�v�f�[�g
	virtual void UpdateAsyncLoad() abstract;
	//�`�揈��
	virtual void Draw() abstract;
	//�I������
	virtual void End() abstract;

protected:
	//�V�[���J�ڂȂǂ��Ǘ�����N���X
	SceneManager& m_sceneManager;
};