#pragma once
#include <string>
#include "Vector2.h"
#include <vector>
#include <map>

class TutorialUi
{
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
		int scale = 1.0;
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

	//表示しているボタンの数
	int m_drawButtonNum;

	//描画している画像配列
	std::map<std::string, GraphData> m_drawGraphs;
};

