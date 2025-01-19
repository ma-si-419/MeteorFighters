#pragma once
#include "Vector2.h"
#include <string>

class GameUi
{
public:

	GameUi();

	/// <summary>
	/// リトライする際の初期化を行う
	/// </summary>
	void RetryInit();

	/// <summary>
	/// 体力バーを表示する
	/// </summary>
	/// <param name="hp">体力量</param>
	/// <param name="isLeft">右と左どちらに表示するか、左:true</param>
	void DrawHpBar(float hp, bool isLeft);

	/// <summary>
	/// 気力バーを表示する
	/// </summary>
	/// <param name="mp">気力量</param>
	/// <param name="isLeft">右と左どちらに表示するか</param>
	void DrawMpBar(float mp, bool isLeft);

	/// <summary>
	/// 画面全体のフェードを表示する
	/// </summary>
	/// <param name="color">色</param>
	/// <param name="alpha">アルファ値</param>
	void DrawFade(int color,int alpha);

	/// <summary>
	/// 連打するボタンを表示する
	/// </summary>
	/// <param name="button">連打するボタンの名前</param>
	void DrawBashButton(std::string button);
private:

	//1Pと2Pの二つのHpバーがあるので二つ持っておく
	MyEngine::Vector2 m_shakePos[2];

	//1Pと2Pの前のフレームのHpを保存しておく
	float m_lastHp[2];

	//1Pと2Pの前のフレームのHpバーの数を保存しておく
	float m_lastHpBarNum[2];

	//赤いダメージバーを表示するためにダメージを受けた時のHPを表示する
	float m_onHitDamageHp[2];

	//赤いMpバーを表示するために気力を消費した際のMpを保存する
	float m_onSubMp[2];

	//攻撃を受けてからのフレームを保存する
	int m_hitDamageTime[2];

	//連打するボタンを切り替える時間
	int m_bashButtonChangeTime;
};
