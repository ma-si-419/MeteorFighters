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
	/// バトル後のリザルトの更新
	/// </summary>
	/// <returns>何も選択していないときは-1を返す</returns>
	int UpdateResult();

	/// <summary>
	/// 勝敗を表示する
	/// </summary>
	/// <param name="isWin">1Pが勝ったかどうか</param>
	void DrawResult(bool isWin);

private:

	//1Pと2Pの二つのHpバーがあるので二つ持っておく
	MyEngine::Vector2 m_shakePos[2];

	//1Pと2Pの前のフレームのHpを保存しておく
	float m_lastHp[2];

	//1Pと2Pの前のフレームのHpバーの数を保存しておく
	float m_lastHpBarNum[2];

	//赤いダメージバーを表示するためにダメージを受けた時のHPを表示する
	float m_onHitDamageHp[2];

	//攻撃を受けてからのフレームを保存する
	int m_hitDamageTime[2];

	int m_selectItem;

	//リザルト画面の描画を始めてからの時間を計測する
	int m_resultTime;

	//リザルト画面のロゴのアルファ値
	int m_resultLogoAlpha;

	//リザルト画面のロゴの拡大率
	double m_resultLogoScale;

	//何フレーム揺らしたか
	int m_shakeTime;
};
