#pragma once
#include "Vector2.h"

class GameUi
{
public:
	enum class SelectItem
	{
		kRetry,
		kCharacterSelect,
		kTitle,
		kItemNum = kTitle
	};

public:

	GameUi();

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
	/// ��ʑS�̂̃t�F�[�h��\������
	/// </summary>
	/// <param name="color">�F</param>
	/// <param name="alpha">�A���t�@�l</param>
	void DrawFade(int color,int alpha);

	/// <summary>
	/// �o�g����̃��U���g�̍X�V
	/// </summary>
	/// <returns>�����I�����Ă��Ȃ��Ƃ���-1��Ԃ�</returns>
	int UpdateResult();

	/// <summary>
	/// ���s��\������
	/// </summary>
	/// <param name="isWin">1P�����������ǂ���</param>
	void DrawResult(bool isWin);

private:

	//1P��2P�̓��Hp�o�[������̂œ�����Ă���
	MyEngine::Vector2 m_shakePos[2];

	//1P��2P�̑O�̃t���[����Hp��ۑ����Ă���
	float m_lastHp[2];

	//1P��2P�̑O�̃t���[����Hp�o�[�̐���ۑ����Ă���
	float m_lastHpBarNum[2];

	//�Ԃ��_���[�W�o�[��\�����邽�߂Ƀ_���[�W���󂯂�����HP��\������
	float m_onHitDamageHp[2];

	//�U�����󂯂Ă���̃t���[����ۑ�����
	int m_hitDamageTime[2];

	int m_selectItem;

	//���U���g��ʂ̕`����n�߂Ă���̎��Ԃ��v������
	int m_resultTime;

	//���U���g��ʂ̃��S�̃A���t�@�l
	int m_resultLogoAlpha;

	//���U���g��ʂ̃��S�̊g�嗦
	double m_resultLogoScale;

	//���t���[���h�炵����
	int m_shakeTime;
};
