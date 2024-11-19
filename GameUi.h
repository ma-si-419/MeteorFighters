#pragma once
#include "Vector2.h"

class GameUi
{
public:

	void DrawHpBar(float hp, bool isLeft);

private:

	//1Pと2Pの二つのHpバーがあるので二つ持っておく
	MyEngine::Vector2 m_shakePos[2];

	//1Pと2Pの前のフレームのHpを保存しておく
	float m_lastHp[2] = {-1,-1};

	//1Pと2Pの前のフレームのHpバーの数を保存しておく
	float m_lastHpBarNum[2] = {-1,-1};

	//赤いダメージバーを表示するためにダメージを受けた時のHPを表示する
	float m_onHitDamageHp[2] = { 0,0 };

	//攻撃を受けてからのフレームを保存する
	int m_hitDamageTime[2] = {0,0};
};
