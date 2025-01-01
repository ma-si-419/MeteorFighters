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

	void DrawMenu();

	void DrawStart(int number);

	void DrawPlaying(int number);

	
	/// <summary>
	/// メニュー画面で選択したものを取得する
	/// </summary>
	/// <returns>なにも選択していないときは-1を返す</returns>
	MenuItem GetSelectItem() { return m_selectItem; };

private:

	void DrawStringCenter(std::string string, MyEngine::Vector2 centerPos, int fontHandle, int color, int edgeColor);

	void UpdateMenu();

	void UpdateStart();

	void UpdatePlaying();

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

	//描画している画像配列
	std::map<std::string, GraphData> m_drawGraphs;

	//メニュー画面で現在選択している項目
	MenuItem m_selectItem;

	//メニュー画面で前回項目を動かしてからの時間
	int m_selectItemMoveTime;

	//現在行っているチュートリアル
	int m_tutorialNumber;
};

