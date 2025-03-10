#pragma once
#include "Vector2.h"
#include <string>

class GameUi
{
public:

	GameUi();

	~GameUi();

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
	/// コンボ数の更新を行う
	/// </summary>
	void UpdateComboUI();

	/// <summary>
	/// ダメージの更新を行う
	/// </summary>
	void UpdateDamageUI();

	/// <summary>
	/// コンボ数を描画する
	/// </summary>
	void DrawCombo();

	/// <summary>
	/// ダメージ数を描画する
	/// </summary>
	void DrawDamage();

	/// <summary>
	/// コンボ数を設定する
	/// </summary>
	/// <param name="combo">コンボ数</param>
	/// <param name="isLeft">1P側ならtrue</param>
	void SetCombo(int combo, bool isLeft);

	/// <summary>
	/// ダメージを設定する
	/// </summary>
	/// <param name="damage">与えたダメージ</param>
	/// <param name="isLeft">1P側ならtrue</param>
	void SetDamage(int damage, bool isLeft);

	/// <summary>
	/// 画面全体のフェードを表示する
	/// </summary>
	/// <param name="color">色</param>
	/// <param name="alpha">アルファ値</param>
	void DrawFade(int color, int alpha);

	/// <summary>
	/// 連打するボタンを表示する
	/// </summary>
	/// <param name="button">連打するボタンの名前</param>
	void DrawBashButton(std::string button);
private:

	/// <summary>
	/// 桁数を取得する
	/// </summary>
	/// <param name="num">取得したい数字</param>
	/// <returns>引数で渡した数字の桁数</returns>
	int GetDigit(int num);

	/// <summary>
	/// 数字を描画する
	/// </summary>
	/// <param name="number">描画したい数字</param>
	/// <param name="posX">X座標</param>
	/// <param name="posY">Y座標</param>
	/// <param name="interval">描画する間隔</param>
	/// <param name="fontHandle">フォントハンドル</param>
	/// <param name="color">色</param>
	/// <param name="isLeft">左ぞろえならtrue</param>
	void DrawNumber(int number, int posX, int posY, int interval, int fontHandle, int color, bool isLeft);



private:

	//ダメージを表示するときのフォントハンドル
	int m_damageFontHandle;

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

	//コンボ数
	int m_comboNum[2];

	//コンボ数が切り替わってからの時間
	int m_comboTime[2];

	//コンボのUIのX座標
	int m_comboPosX[2];

	//コンボの数字の拡大率
	double m_comboScale[2];

	//コンボのアルファ値
	int m_comboAlpha[2];

	//総ダメージ量
	int m_damage[2];

	//表示しているダメージ
	int m_showDamage[2];

	//1フレームで増やすダメージ
	int m_showDamageAddNum[2];

	//ダメージを表示する座標
	int m_damagePosX[2];

	//ダメージのアルファ値
	int m_damageAlpha[2];

	//ダメージの表示時間
	int m_damageShowTime[2];
};
