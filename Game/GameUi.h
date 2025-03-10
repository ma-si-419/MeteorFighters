#pragma once
#include "Vector2.h"
#include <string>

class GameUi
{
public:

	GameUi();

	~GameUi();

	/// <summary>
	/// ���g���C����ۂ̏��������s��
	/// </summary>
	void RetryInit();

	/// <summary>
	/// �̗̓o�[��\������
	/// </summary>
	/// <param name="hp">�̗͗�</param>
	/// <param name="isLeft">�E�ƍ��ǂ���ɕ\�����邩�A��:true</param>
	void DrawHpBar(float hp, bool isLeft);

	/// <summary>
	/// �C�̓o�[��\������
	/// </summary>
	/// <param name="mp">�C�͗�</param>
	/// <param name="isLeft">�E�ƍ��ǂ���ɕ\�����邩</param>
	void DrawMpBar(float mp, bool isLeft);

	/// <summary>
	/// �R���{���̍X�V���s��
	/// </summary>
	void UpdateComboUI();

	/// <summary>
	/// �_���[�W�̍X�V���s��
	/// </summary>
	void UpdateDamageUI();

	/// <summary>
	/// �R���{����`�悷��
	/// </summary>
	void DrawCombo();

	/// <summary>
	/// �_���[�W����`�悷��
	/// </summary>
	void DrawDamage();

	/// <summary>
	/// �R���{����ݒ肷��
	/// </summary>
	/// <param name="combo">�R���{��</param>
	/// <param name="isLeft">1P���Ȃ�true</param>
	void SetCombo(int combo, bool isLeft);

	/// <summary>
	/// �_���[�W��ݒ肷��
	/// </summary>
	/// <param name="damage">�^�����_���[�W</param>
	/// <param name="isLeft">1P���Ȃ�true</param>
	void SetDamage(int damage, bool isLeft);

	/// <summary>
	/// ��ʑS�̂̃t�F�[�h��\������
	/// </summary>
	/// <param name="color">�F</param>
	/// <param name="alpha">�A���t�@�l</param>
	void DrawFade(int color, int alpha);

	/// <summary>
	/// �A�ł���{�^����\������
	/// </summary>
	/// <param name="button">�A�ł���{�^���̖��O</param>
	void DrawBashButton(std::string button);
private:

	/// <summary>
	/// �������擾����
	/// </summary>
	/// <param name="num">�擾����������</param>
	/// <returns>�����œn���������̌���</returns>
	int GetDigit(int num);

	/// <summary>
	/// ������`�悷��
	/// </summary>
	/// <param name="number">�`�悵��������</param>
	/// <param name="posX">X���W</param>
	/// <param name="posY">Y���W</param>
	/// <param name="interval">�`�悷��Ԋu</param>
	/// <param name="fontHandle">�t�H���g�n���h��</param>
	/// <param name="color">�F</param>
	/// <param name="isLeft">�����낦�Ȃ�true</param>
	void DrawNumber(int number, int posX, int posY, int interval, int fontHandle, int color, bool isLeft);



private:

	//�_���[�W��\������Ƃ��̃t�H���g�n���h��
	int m_damageFontHandle;

	//1P��2P�̓��Hp�o�[������̂œ�����Ă���
	MyEngine::Vector2 m_shakePos[2];

	//1P��2P�̑O�̃t���[����Hp��ۑ����Ă���
	float m_lastHp[2];

	//1P��2P�̑O�̃t���[����Hp�o�[�̐���ۑ����Ă���
	float m_lastHpBarNum[2];

	//�Ԃ��_���[�W�o�[��\�����邽�߂Ƀ_���[�W���󂯂�����HP��\������
	float m_onHitDamageHp[2];

	//�Ԃ�Mp�o�[��\�����邽�߂ɋC�͂�������ۂ�Mp��ۑ�����
	float m_onSubMp[2];

	//�U�����󂯂Ă���̃t���[����ۑ�����
	int m_hitDamageTime[2];

	//�A�ł���{�^����؂�ւ��鎞��
	int m_bashButtonChangeTime;

	//�R���{��
	int m_comboNum[2];

	//�R���{�����؂�ւ���Ă���̎���
	int m_comboTime[2];

	//�R���{��UI��X���W
	int m_comboPosX[2];

	//�R���{�̐����̊g�嗦
	double m_comboScale[2];

	//�R���{�̃A���t�@�l
	int m_comboAlpha[2];

	//���_���[�W��
	int m_damage[2];

	//�\�����Ă���_���[�W
	int m_showDamage[2];

	//1�t���[���ő��₷�_���[�W
	int m_showDamageAddNum[2];

	//�_���[�W��\��������W
	int m_damagePosX[2];

	//�_���[�W�̃A���t�@�l
	int m_damageAlpha[2];

	//�_���[�W�̕\������
	int m_damageShowTime[2];
};
