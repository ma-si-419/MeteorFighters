#pragma once
#include <string>
#include "Vector2.h"
#include <vector>
#include <map>


class TutorialUi
{
public:

	enum class PlayMenuItem
	{
		kReset,
		kChangeTutorial,
		kChangeMode,
		kSelectMenu,
		kMenuEnd,
		kTutorialEnd,
		kItemEnd = kTutorialEnd,
		kItemNum
	};

	enum class StartMenuItem
	{
		kStartTutorial,
		kSelectTutorial,
		kEnd,
		kItemEnd = kEnd,
		kItemNum
	};

public:

	TutorialUi();

	virtual ~TutorialUi();

	void Init();

	void Update();

	void Final();

	void InitStartMenu();

	void InitPlayMenu();

	void InitStart();

	void InitSelectMenu();

	void InitPlaying(int number);

	void InitSuccess();

	void DrawStartMenu();

	void DrawSelectMenu();

	void DrawPlayMenu();

	void DrawStart(int number);

	void DrawPlaying(int number);

	void DrawSuccess(int number);


	/// <summary>
	/// 今行っているチュートリアルをクリアしたかどうかを設定する
	/// </summary>
	void SetSuccessTutorial(bool flag) { m_isSuccessTutorial = flag; }
	
	/// <summary>
	/// メニュー画面で選択したものを取得する
	/// </summary>
	/// <returns>なにも選択していないときは-1を返す</returns>
	PlayMenuItem GetPlayMenuSelectItem() { return m_playMenuSelectItem; };

	/// <summary>
	/// スタート画面で選択したものを取得する
	/// </summary>
	/// <returns>何も選択していないときは-1を返す</returns>
	StartMenuItem GetStartMenuSelectItem() { return m_startMenuSelectItem; }

	/// <summary>
	/// 選択したチュートリアル
	/// </summary>
	/// <returns>メニュー画面で選択したチュートリアルを返す</returns>
	int GetTutorialNumber() { return m_selectTutorialNumber; }

	/// <summary>
	/// セレクトメニューで選択しているチュートリアルの名前を取得する
	/// </summary>
	/// <returns>チュートリアルの名前</returns>
	std::string GetSelectTutorialName() { return m_tutorialSelectMenuData[m_selectMenuIndexX + 1][m_selectMenuIndexY]; }

	/// <summary>
	/// 現在選択しているチュートリアル番号を設定する
	/// </summary>
	/// <param name="number">現在行っているチュートリアル番号</param>
	void SetTutorialNumber(int number) { m_selectTutorialNumber = number; }

	/// <summary>
	/// 現在選択しているチュートリアルのモードを取得する
	/// </summary>
	/// <returns></returns>
	int GetNowTutorialMode() { return m_nowTutorialMode; }

	/// <summary>
	/// クリア演出が終わったかどうかを返す
	/// </summary>
	bool IsSuccessEnd() { return m_isSuccessEnd; }

	/// <summary>
	/// スタートメニューで選択できるかどうかを取得する
	/// </summary>
	/// <returns>選択できるのならtrue</returns>
	bool IsSelectStartMenu() { return m_isSelectStartMenu; }

	/// <summary>
	/// セレクトメニューで選択できるかどうかを取得する
	/// </summary>
	/// <returns>選択できるのなら</returns>
	bool IsSelectSelectMenu() { return m_isSelectSelectMenu; }

	/// <summary>
	/// チュートリアルのデータを設定する
	/// </summary>
	/// <param name="data">外部から読み込んだデータ</param>
	void SetTutorialPlayData(std::vector<std::vector<std::string>> data) { m_tutorialPlayData = data; }

private:

	void DrawStringCenter(std::string string, MyEngine::Vector2 centerPos, int fontHandle, int color, int edgeColor);

	void UpdateStartMenu();

	void UpdateSelectMenu();

	void UpdatePlayMenu();

	void UpdateStart();

	void UpdatePlaying();

	void UpdateSuccess();

private:

	struct GraphData
	{
		int handle = -1;
		MyEngine::Vector2 pos;
		double scale = 1.0;
		int alpha = 255;
	};

private:

	using UpdateFunc = void (TutorialUi::*)();

	UpdateFunc m_updateFunc;

	//チュートリアルのプレイデータ
	std::vector<std::vector<std::string>> m_tutorialPlayData;

	//チュートリアルのスタートメニューのデータ
	std::vector<std::vector<std::string>> m_tutorialSelectMenuData;

	//開始時の文字のフォントハンドル
	int m_startFontHandle;

	//プレイ中の説明のフォントハンドル
	int m_playingFontHandle;

	//メニューの文字のフォントハンドル
	int m_playMenuFontHandle;

	//スタートメニューの文字のフォントハンドル
	int m_startMenuFontHandle;

	//メニュー画面で選択している項目を示す文字列のフォントハンドル
	int m_selectMenuFontHandle;

	//メニュー画面で選択している項目を示す文字列のフォントハンドル
	int m_selectTutorialKindFontHandle;

	//表示しているボタンの数
	int m_drawButtonNum;

	//選択しているチュートリアルの番号
	int m_selectTutorialNumber;

	//今行っているチュートリアルのモード
	int m_nowTutorialMode;

	//選択しているチュートリアルの左右の矢印の揺れ幅
	float m_selectTutorialStringArrowPos;

	//描画している画像配列
	std::map<std::string, GraphData> m_drawGraphs;

	//メニュー画面で現在選択している項目
	PlayMenuItem m_playMenuSelectItem;

	//スタートメニューで現在選択している項目
	StartMenuItem m_startMenuSelectItem;

	//セレクトメニューで現在選択している項目
	int m_selectMenuIndexX;
	int m_selectMenuIndexY;

	//メニュー画面で前回項目を動かしてからの時間
	int m_selectItemMoveTime;

	//現在行っているチュートリアル
	int m_tutorialNumber;

	//現在行っているチュートリアルをクリアしたらtrueにする
	bool m_isSuccessTutorial;

	//クリア時の画像を揺らしている時間
	int m_successTime;

	//クリアの演出が終わったかどうか
	bool m_isSuccessEnd;

	//スタートメニューで選択できるかどうか
	bool m_isSelectStartMenu;

	//セレクトメニューで選択できるかどうか
	bool m_isSelectSelectMenu;

};

