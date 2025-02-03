#pragma once
#include "Vector2.h"
#include <string>

class BattleUi
{
public:

	enum class UiSituation
	{
		kMenu,
		kResult,
		kStart1P,
		kStart2P,
		kNone
	};

	enum class MenuItem
	{
		kReturnBattle,
		kRetry,
		kBackCharacterSelect,
		kBackMenu,
		kItemEnd = kBackMenu,
		kItemNum
	};

	enum class ResultItem
	{
		kRetry,
		kCharacterSelect,
		kMenu,
		kItemEnd = kMenu,
		kItemNum
	};

public:

	BattleUi();

	virtual ~BattleUi();

	void Update();

	void Draw();

	void ChangeSituation(UiSituation situation);

	/// <summary>
	/// 勝ったかどうかを設定する
	/// </summary>
	/// <param name="flag">勝っているならtrue負けているならfalseを入れる</param>
	void SetResult(bool flag) { m_isWin = flag; }

	/// <summary>
	/// どの項目を選択したかを取得する
	/// </summary>
	/// <returns>選択した項目,何も選択していなければ-1</returns>
	int GetDecisionItem() { return m_decisionItem; }


	/// <summary>
	/// リザルトメニューが表示されているかどうかを設定する
	///	</summary>
	/// <param name= "flag">リザルトメニューを表示するならtrue</param>
	void SetResultMenuDisplay(bool flag) { m_isResultMenuDisplay = flag; }

private:

	void UpdateMenu();

	void UpdateResult();

	void UpdateStart1P();

	void UpdateStart2P();

	void DrawMenu();

	void DrawResult();

	void DrawStart1P();

	void DrawStart2P();

	/// <summary>
	/// なにもしない
	/// </summary>
	void None() {};

	void DrawStringCenter(std::string string, MyEngine::Vector2 centerPos, int font, int color, int edgeColor);

private:

	using UpdateSituationFunc = void (BattleUi::*)();
	using DrawSituationFunc = void (BattleUi::*)();

	UpdateSituationFunc m_updateFunc;
	DrawSituationFunc m_drawFunc;

	//リザルトのメニューのフォントハンドル
	int m_menuFontHandle;

	//リザルト画面の描画を始めてからの時間を計測する
	int m_resultTime;

	//リザルト画面のロゴの拡大率
	double m_resultLogoScale;

	//リザルトのメニューのアルファ値
	int m_resultMenuAlpha;

	//メニューのアルファ値
	int m_menuAlpha;

	//何フレーム揺らしたか
	int m_shakeTime;

	//選択している項目
	int m_selectItem;

	//決定を押した項目
	int m_decisionItem;

	//勝ったかどうかを設定する
	bool m_isWin;

	//リザルトメニューが表示されているかどうか
	bool m_isResultMenuDisplay;

	//前回カーソルを動かしてから何フレーム立ったか
	int m_selectItemMoveTime;

	//選択している項目の後ろの画像の大きさ
	double m_selectItemBoxScale;

};