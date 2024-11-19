#pragma once
#include "Vector2.h"

class GameUi
{
public:

	void DrawHpBar(float hp, bool isLeft);

private:

	//1P��2P�̓��Hp�o�[������̂œ�����Ă���
	MyEngine::Vector2 m_shakePos[2];

	//1P��2P�̑O�̃t���[����Hp��ۑ����Ă���
	float m_lastHp[2] = {-1,-1};

	//1P��2P�̑O�̃t���[����Hp�o�[�̐���ۑ����Ă���
	float m_lastHpBarNum[2] = {-1,-1};

	//�Ԃ��_���[�W�o�[��\�����邽�߂Ƀ_���[�W���󂯂�����HP��\������
	float m_onHitDamageHp[2] = { 0,0 };

	//�U�����󂯂Ă���̃t���[����ۑ�����
	int m_hitDamageTime[2] = {0,0};
};
