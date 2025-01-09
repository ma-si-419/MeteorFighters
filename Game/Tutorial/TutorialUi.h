#pragma once
#include <string>
#include "Vector2.h"
#include <vector>
#include <map>

class TutorialUi
{
public:

	enum class MenuItem
	{
		kReset,
		kChangeTutorial,
		kMenuEnd,
		kTutorialEnd,
		kItemEnd = kTutorialEnd,
		kItemNum
	};

public:

	TutorialUi();

	virtual ~TutorialUi();

	void Init();

	void Update();

	void Final();

	void InitMenu();

	void InitStart();

	void InitPlaying(int number);

	void InitSuccess();

	void DrawMenu();

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
	MenuItem GetSelectItem() { return m_selectItem; };

	/// <summary>
	/// 選択したチュートリアル
	/// </summary>
	/// <returns>メニュー画面で選択したチュートリアルを返す</returns>
	int GetTutorialNumber() { return m_selectTutorialNumber; }

	/// <summary>
	/// 現在選択しているチュートリアル番号を設定する
	/// </summary>
	/// <param name="number">現在行っているチュートリアル番号</param>
	void SetNowTutorialNumber(int number) { m_selectTutorialNumber = number; }

	/// <summary>
	/// クリア演出が終わったかどうかを返す
	/// </summary>
	bool IsSuccessEnd() { return m_isSuccessEnd; }

private:

	void DrawStringCenter(std::string string, MyEngine::Vector2 centerPos, int fontHandle, int color, int edgeColor);

	void UpdateMenu();

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

	//チュートリアルのUIデータ
	std::vector<std::vector<std::string>> m_tutorialData;

	//開始時の文字のフォントハンドル
	int m_startFontHandle;

	//プレイ中の説明のフォントハンドル
	int m_playingFontHandle;

	//メニューの文字のフォントハンドル
	int m_menuFontHandle;

	//表示しているボタンの数
	int m_drawButtonNum;

	//選択しているチュートリアルの番号
	int m_selectTutorialNumber;

	//選択しているチュートリアルの左右の矢印の揺れ幅
	float m_selectTutorialStringArrowPos;

	//描画している画像配列
	std::map<std::string, GraphData> m_drawGraphs;

	//メニュー画面で現在選択している項目
	MenuItem m_selectItem;

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
};

