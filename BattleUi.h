#pragma once
class BattleUi
{
public:

	enum class UiSituation
	{
		kMenu,
		kResult,
		kNone
	};

	enum class ResultItem
	{
		kRetry,
		kCharacterSelect,
		kMenu,
		kItemNum = kMenu
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

private:

	void UpdateMenu();

	void UpdateResult();

	void DrawMenu();

	void DrawResult();

	/// <summary>
	/// なにもしない
	/// </summary>
	void None() {};

private:

	using UpdateSituationFunc = void (BattleUi::*)();
	using DrawSituationFunc = void (BattleUi::*)();

	UpdateSituationFunc m_updateFunc;
	DrawSituationFunc m_drawFunc;

	//リザルト画面の描画を始めてからの時間を計測する
	int m_resultTime;

	//リザルト画面のロゴの拡大率
	double m_resultLogoScale;

	//何フレーム揺らしたか
	int m_shakeTime;

	//選択している項目
	int m_selectItem;

	//決定を押した項目
	int m_decisionItem;

	//勝ったかどうかを設定する
	bool m_isWin;

	//前回カーソルを動かしてから何フレーム立ったか
	int m_selectItemMoveTime;
};