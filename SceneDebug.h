#pragma once
#include "SceneBase.h"
#include <memory>

class SceneDebug : public SceneBase
{
public:
	SceneDebug(SceneManager& sceneManager);
	virtual ~SceneDebug();
	//����������
	virtual void Init() override;
	//�X�V����
	virtual void Update() override;
	//�񓯊��������s���Ă���Ƃ��̃A�b�v�f�[�g
	virtual void UpdateAsyncLoad() override {};
	//�`�揈��
	virtual void Draw() override;
	//�I������
	virtual void End() override;


private:


	/// <summary>
	/// �w�肵���V�[�����쐬����
	/// </summary>
	/// <param name="sceneNum">Game::Scene�̔ԍ����w��</param>
	/// <returns>�w�肵���V�[���̃|�C���^</returns>
	std::shared_ptr<SceneBase> CreateScene(int sceneNum);

private:

	////////////////////////////////
	//�f�o�b�O

	//���݂̃R���{��
	int m_combo;

	//�R���{�����؂�ւ���Ă���̎���
	int m_comboTime;

	//�R���{��UI��X���W
	int m_comboPosX[2];

	//�R���{�̐����̊g�嗦
	double m_comboScale[2];

	//�R���{�̃A���t�@�l
	int m_comboAlpha;

	////////////////////////////////

	int m_numberGraphHandle[10];

	int m_selectScene;

	int m_selectSceneMoveTime;

};