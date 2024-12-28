#include "TutorialUi.h"
#include "LoadCsv.h"
#include "TutorialManager.h"
#include "GraphManager.h"

namespace
{
	//フォントの名前
	const TCHAR* kFontName = "GN-キルゴUかなNB";

	//スタート時に表示するフォントのサイズ
	constexpr int kStartFontSize = 84;
	constexpr int kPlayingFontSize = 32;

	//チュートリアルのボタンを表示する座標のフレーム
	constexpr int kButtonFramePosX = Game::kWindowWidth / 2;
	constexpr int kButtonFramePosY = 150;

	//プレイ中のチュートリアルの説明を表示する座標Y
	constexpr int kPlayingStringPosX = Game::kWindowWidth / 2;
	constexpr int kPlayingStringPosY = 210;

	//何のボタンを押せばいいのかを表示する座標
	constexpr int kButtonPosX = Game::kWindowWidth / 2;
	constexpr int kButtonPosY = 135;

	//ボタンを表示する間隔
	constexpr int kButtonInterval = 80;
}

TutorialUi::TutorialUi()
{
	LoadCsv load;

	m_tutorialData = load.LoadFile("data/csv/tutorialData.csv");

	m_updateFunc = &TutorialUi::UpdateMenu;

	m_startFontHandle = CreateFontToHandle(kFontName, kStartFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 3);
	m_playingFontHandle = CreateFontToHandle(kFontName, kPlayingFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 3);
}

TutorialUi::~TutorialUi()
{
}

void TutorialUi::Init()
{
}

void TutorialUi::Update()
{
	(this->*m_updateFunc)();
}

void TutorialUi::Final()
{
	DeleteFontToHandle(m_startFontHandle);
}

void TutorialUi::InitMenu()
{
	//登録している画像をすべて消す
	m_drawGraphs.clear();

	m_updateFunc = &TutorialUi::UpdateMenu;
}

void TutorialUi::InitStart()
{
	//登録している画像をすべて消す
	m_drawGraphs.clear();

	m_updateFunc = &TutorialUi::UpdateStart;

	auto& manager = GraphManager::GetInstance();

	GraphData data;

	data.pos = MyEngine::Vector2(Game::kWindowWidth / 2, Game::kWindowHeight / 2);

	data.handle = manager.GetHandle("BackBox");

	m_drawGraphs["BackBox"] = data;
}

void TutorialUi::InitPlaying(int number)
{
	//登録している画像をすべて消す
	m_drawGraphs.clear();

	m_updateFunc = &TutorialUi::UpdatePlaying;

	//画像管理クラス
	auto& manager = GraphManager::GetInstance();
	LoadCsv stringFunc;

	GraphData frame;

	frame.pos = MyEngine::Vector2(kButtonFramePosX, kButtonFramePosY);

	frame.handle = manager.GetHandle("ButtonFrame");

	m_drawGraphs["ButtonFrame"] = frame;

	auto buttonString = m_tutorialData[number][static_cast<int>(TutorialManager::TutorialDataIndex::kButton)];

	auto button = stringFunc.Split(buttonString, '/');

	m_drawButtonNum = button.size();

	//ボタンの描画数が1個であれば
	if (m_drawButtonNum == 1)
	{
		GraphData buttonData;

		buttonData.handle = manager.GetHandle(button[0]);

		buttonData.pos = MyEngine::Vector2(kButtonPosX, kButtonPosY);

		m_drawGraphs["Button0"] = buttonData;
	}
	//ボタンの描画数が偶数であれば
	else if (m_drawButtonNum % 2 == 0)
	{
		//描画開始位置を計算する
		int posX = kButtonPosX - (kButtonInterval * m_drawButtonNum / 2);
		posX += kButtonInterval / 2;

		//描画数だけループ
		for (int i = 0; i < m_drawButtonNum; i++)
		{
			//ボタンを描画する
			DrawRotaGraph(posX, kButtonPosY, 1.0, 0.0, manager.GetHandle(button[i]), true);

			GraphData buttonData;

			buttonData.handle = manager.GetHandle(button[i]);

			buttonData.pos = MyEngine::Vector2(posX,kButtonPosY);

			std::string name = "Button" + std::to_string(i);

			m_drawGraphs[name] = buttonData;

			posX += kButtonInterval;
		}
	}
	//奇数の場合
	else
	{

	}

}

void TutorialUi::DrawMenu()
{

}

void TutorialUi::DrawStart(int number)
{
	GraphData data = m_drawGraphs["BackBox"];

	DrawRotaGraph(data.pos.x,data.pos.y, data.scale, 0.0, data.handle, true);

	std::string startWord = m_tutorialData[number][static_cast<int>(TutorialManager::TutorialDataIndex::kStartString)];

	//TODO : ファイルロードと文字列の便利関数を分けるようにする(リファクタリング)
	LoadCsv load;

	std::vector<std::string> splitString = load.Split(startWord, '/');

	//基本的には画面中央に文章を表示する
	MyEngine::Vector2 stringPos = MyEngine::Vector2(Game::kWindowWidth / 2, Game::kWindowHeight / 2);

	for (int i = 0; i < splitString.size(); i++)
	{
		//仮処理
		if (splitString.size() == 2)
		{
			MyEngine::Vector2 shift = MyEngine::Vector2(0, static_cast<int>(-GetFontSizeToHandle(m_startFontHandle) * 0.5f));

			shift.y += GetFontSizeToHandle(m_startFontHandle) * i;

			stringPos += shift;

			DrawStringCenter(splitString[i], stringPos, m_startFontHandle, GetColor(0, 0, 0), GetColor(255, 255, 255));
		}
		else
		{
			DrawStringCenter(splitString[i], stringPos, m_startFontHandle, GetColor(0, 0, 0), GetColor(255, 255, 255));
		}
	}
}

void TutorialUi::DrawPlaying(int number)
{
	GraphData frameData = m_drawGraphs["ButtonFrame"];

	//ボタンを表示するフレーム
	DrawRotaGraph(frameData.pos.x, frameData.pos.y, frameData.scale, 0.0, frameData.handle, true);

	//プレイ中の説明
	auto playingString = m_tutorialData[number][static_cast<int>(TutorialManager::TutorialDataIndex::kPlayingString)];

	//プレイ中の説明を描画
	DrawStringCenter(playingString, MyEngine::Vector2(kPlayingStringPosX, kPlayingStringPosY), m_playingFontHandle, GetColor(255, 255, 255), GetColor(0, 0, 0));

	//押すボタンを表示する
	for (int i = 0; i < m_drawButtonNum; i++)
	{
		std::string name = "Button" + std::to_string(i);

		GraphData buttonData = m_drawGraphs[name];

		DrawRotaGraph(buttonData.pos.x, buttonData.pos.y, buttonData.scale, 0.0, buttonData.handle, true);		
	}
}

void TutorialUi::DrawStringCenter(std::string string, MyEngine::Vector2 centerPos, int font, int color, int edgeColor)
{
	//文字を表示する座標
	MyEngine::Vector2 pos;

	//文字数
	int wordNum = GetStringLength(string.c_str());

	//文字数が偶数の場合
	if (wordNum % 2 == 0)
	{
		//文字の座標を左にずらす
		pos.x = centerPos.x - static_cast<int>(GetFontSizeToHandle(font) * wordNum / 2);

		//文字の大きさの半分だけ上にずらす
		pos.y = centerPos.y - static_cast<int>(GetFontSizeToHandle(font) / 2);
	}
	//奇数の場合
	else
	{
		wordNum++;

		//文字の座標を左にずらす
		pos.x = centerPos.x - static_cast<int>(GetFontSizeToHandle(font) * wordNum / 2);
		pos.x += GetFontEdgeSizeToHandle(font) / 2;

		//文字の大きさの半分だけ上にずらす
		pos.y = centerPos.y - static_cast<int>(GetFontSizeToHandle(font) / 2);
	}

	//文字の描画
	DrawStringToHandle(pos.x, pos.y, string.c_str(), color, font, edgeColor);
}

void TutorialUi::UpdateMenu()
{
}

void TutorialUi::UpdateStart()
{
}

void TutorialUi::UpdatePlaying()
{
}
