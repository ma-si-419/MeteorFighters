#pragma once
#include "SceneBase.h"
#include <memory>
#include "Vector2.h"


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

	/// <summary>
	/// �_���[�W��`�悷��
	/// </summary>
	/// <param name="damage">�_���[�W��</param>
	void DrawDamage(int damage);

private:

	////////////////////////////////
	//�f�o�b�O

	//���݂̃R���{��
	int m_combo;

	//�_���[�W��\��������W
	int m_damagePosX;

	//���݂̃_���[�W
	int m_damage;

	//�\�����Ă���_���[�W
	int m_showDamage;

	//�\�����Ă���_���[�W�𑝂₷����
	int m_showDamageAddTime;

	//1�t���[���ő��₷�_���[�W
	int m_showDamageAddNum;

	//�R���{�����؂�ւ���Ă���̎���
	int m_comboTime;

	//�R���{��UI��X���W
	MyEngine::Vector2 m_comboPos;

	//�R���{�̐����̊g�嗦
	double m_comboScale[2];

	//�R���{�̃A���t�@�l
	int m_comboAlpha;

	////////////////////////////////

	int m_damageFontHandle;

	int m_numberGraphHandle[10];

	int m_comboBarGraphHandle;

	int m_hitGraphHandle;

	int m_selectScene;

	int m_selectSceneMoveTime;

};