#pragma once
#include "Vector2.h"
#include <string>

class GameUi
{
public:

	GameUi();

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
	/// ��ʑS�̂̃t�F�[�h��\������
	/// </summary>
	/// <param name="color">�F</param>
	/// <param name="alpha">�A���t�@�l</param>
	void DrawFade(int color,int alpha);

	/// <summary>
	/// �A�ł���{�^����\������
	/// </summary>
	/// <param name="button">�A�ł���{�^���̖��O</param>
	void DrawBashButton(std::string button);
private:

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
};
