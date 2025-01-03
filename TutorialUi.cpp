#include "TutorialUi.h"
#include "LoadCsv.h"
#include "TutorialManager.h"
#include "GraphManager.h"
#include "Input.h"

namespace
{
	//フォントの名前
	const TCHAR* kFontName = "GN-キルゴUかなNB";

	//スタート時に表示するフォントのサイズ
	constexpr int kStartFontSize = 84;
	//プレイ中の説明を表示するフォントのサイズ
	constexpr int kPlayingFontSize = 32;
	//メニューの文字のフォントのサイズ
	constexpr int kMenuFontSize = 32;

	//チュートリアルのボタンを表示する座標のフレーム
	constexpr int kButtonFramePosX = Game::kWindowWidth / 2;
	constexpr int kButtonFramePosY = 750;

	//プレイ中のチュートリアルの説明を表示する座標Y
	constexpr int kPlayingStringPosX = Game::kWindowWidth / 2;
	constexpr int kPlayingStringPosY = 810;

	//何のボタンを押せばいいのかを表示する座標
	constexpr int kButtonPosX = Game::kWindowWidth / 2;
	constexpr int kButtonPosY = 745;

	//ボタンを表示する間隔
	constexpr int kButtonInterval = 80;

	//メニューが出てくるときのフェードインの速さ
	constexpr int kMenuFadeSpeed = 20;

	//メニューを表示しているときのゲームシーンにかける黒いボックスの最大アルファ値
	constexpr int kBackBoxMaxAlpha = 150;

	//上下入力でリピート入力を受け付ける時間
	constexpr int kRepeatInputTime = 15;

	//リピート入力の間隔
	constexpr int kRepeatInterval = 5;

	//メニューの文字を表示する座標
	constexpr int kMenuStringPosX = Game::kWindowWidth / 2;

	//左右で変更できるメニューの文字を表示する座標
	constexpr int kMoveMenuStringPosX = 500;

	//メニューでチュートリアルの名前を表示する座標
	constexpr int kMenuTutorialNamePosX = 1100;

	//一つ目の高さ
	constexpr int kMenuStringPosY = 250;

	//メニューの文字を表示する間隔
	constexpr int kMenuStringDistanceY = 130;

	//表示する文字列
	const std::string kMenuStrings[static_cast<int>(TutorialUi::MenuItem::kItemNum)] =
	{
		"状況をリセットする",
		"チュートリアルを変更",
		"メニューを閉じる",
		"チュートリアルを終了する"
	};

	//選択している項目を動かしたとき選択している項目をどこまで小さくするか
	constexpr double kSelectItemBoxMinScale = 0.7;

	//選択している項目が大きくなっていく速度
	constexpr double kSelectItemBoxScalingSpeed = 0.05;
}

TutorialUi::TutorialUi() :
	m_drawButtonNum(0),
	m_selectItem(static_cast<MenuItem>(0)),
	m_tutorialNumber(0),
	m_selectItemMoveTime(0)
{
	LoadCsv load;

	m_tutorialData = load.LoadFile("data/csv/tutorialData.csv");

	m_updateFunc = &TutorialUi::UpdateMenu;

	m_startFontHandle = CreateFontToHandle(kFontName, kStartFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 3);
	m_playingFontHandle = CreateFontToHandle(kFontName, kPlayingFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 3);
	m_menuFontHandle = CreateFontToHandle(kFontName, kMenuFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 2);
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
	DeleteFontToHandle(m_playingFontHandle);
	DeleteFontToHandle(m_menuFontHandle);
}

void TutorialUi::InitMenu()
{
	//登録している画像をすべて消す
	m_drawGraphs.clear();

	//画像マネージャー
	auto& manager = GraphManager::GetInstance();

	//メニュー画面の画像を登録する
	GraphData menu;
	menu.handle = manager.GetHandle("Menu");
	menu.pos = MyEngine::Vector2(Game::kWindowWidth / 2, Game::kWindowHeight / 2);
	menu.alpha = 0;
	m_drawGraphs["Menu"] = menu;

	//アップデートを変更する
	m_updateFunc = &TutorialUi::UpdateMenu;

	//メニュー画面で選択している項目を示す画像を登録する
	GraphData selectItemBox;
	selectItemBox.handle = manager.GetHandle("SelectItem");
	selectItemBox.pos = MyEngine::Vector2(kMenuStringPosX, kMenuStringPosY);
	m_drawGraphs["SelectItemBox"] = selectItemBox;

	//選択している項目を一番上にする
	m_selectItem = static_cast<MenuItem>(0);

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

			buttonData.pos = MyEngine::Vector2(posX, kButtonPosY);

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
	GraphData menu = m_drawGraphs["Menu"];

	//画面全体を覆う黒いボックスのアルファ値
	int backBoxAlpha = menu.alpha;

	backBoxAlpha = min(backBoxAlpha, kBackBoxMaxAlpha);

	//ブレンドモード変更
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, backBoxAlpha);

	//画面全体を覆う黒いボックス表示
	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, GetColor(0, 0, 0), true);

	//アルファ値変更
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, menu.alpha);

	DrawRotaGraph(menu.pos.x, menu.pos.y, menu.scale, 0.0, menu.handle, true);

	//ブレンドモードを元に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//選択している項目の後ろのボックスを表示する
	GraphData itemBox = m_drawGraphs["SelectItemBox"];

	DrawRotaGraph(itemBox.pos.x,itemBox.pos.y,itemBox.scale,0.0,itemBox.handle,true);

	//文字列を表示する
	for (int i = 0; i < static_cast<int>(TutorialUi::MenuItem::kItemNum); i++)
	{
		//一番上の文字の座標
		MyEngine::Vector2 pos = MyEngine::Vector2(kMenuStringPosX, kMenuStringPosY);

		//文字のY座標をずらす
		pos.y += kMenuStringDistanceY * i;

		//二つ目の文字の座標を横にずらす(別の処理にしたい)

		if (i == static_cast<int>(TutorialUi::MenuItem::kChangeTutorial))
		{
			pos.x = kMoveMenuStringPosX;
		}

		DrawStringCenter(kMenuStrings[i], pos, m_menuFontHandle, GetColor(0, 0, 0), GetColor(255, 255, 255));
	}
}

void TutorialUi::DrawStart(int number)
{
	GraphData backBox = m_drawGraphs["BackBox"];

	DrawRotaGraph(backBox.pos.x, backBox.pos.y, backBox.scale, 0.0, backBox.handle, true);

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
		pos.x = centerPos.x - static_cast<float>(GetFontSizeToHandle(font) * wordNum / 2);

		//文字の大きさの半分だけ上にずらす
		pos.y = centerPos.y - static_cast<float>(GetFontSizeToHandle(font) / 2);
	}
	//奇数の場合
	else
	{
		wordNum++;

		//文字の座標を左にずらす
		pos.x = centerPos.x - static_cast<float>(GetFontSizeToHandle(font) * wordNum / 2);
		pos.x += GetFontEdgeSizeToHandle(font) / 2;

		//文字の大きさの半分だけ上にずらす
		pos.y = centerPos.y - static_cast<float>(GetFontSizeToHandle(font) / 2);
	}

	//文字の描画
	DrawStringToHandle(static_cast<int>(pos.x), static_cast<int>(pos.y), string.c_str(), color, font, edgeColor);
}

void TutorialUi::UpdateMenu()
{
	//入力
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//選択している項目
	int selectItem = static_cast<int>(m_selectItem);



	//上下入力で選択しているものを変更する
	if (input->IsTrigger("Up"))
	{
		selectItem--;

		//一番上で上入力した場合
		if (selectItem < 0)
		{
			//一番下に移動する
			selectItem = static_cast<int>(TutorialUi::MenuItem::kItemEnd);
		}
	}
	else if (input->IsTrigger("Down"))
	{
		selectItem++;

		//一番下で入力された場合
		if (selectItem > static_cast<int>(TutorialUi::MenuItem::kItemEnd))
		{
			//一番上に移動する
			selectItem = 0;
		}
	}

	//リピート入力
	if (m_selectItemMoveTime > kRepeatInterval)
	{
		if (input->GetPressTime("Up") > kRepeatInputTime)
		{
			selectItem--;
		}
		else if (input->GetPressTime("Down") > kRepeatInputTime)
		{
			selectItem++;
		}
	}

	//最大値と最小値を超えないように
	selectItem = max(selectItem, 0);
	selectItem = min(selectItem, static_cast<int>(MenuItem::kItemEnd));


	//選択している項目を表す画像
	auto& selectItemBox = m_drawGraphs["SelectItemBox"];

	//選択している項目を示す画像を少しずつ大きくする
	selectItemBox.scale += kSelectItemBoxScalingSpeed;

	//大きくなりすぎないようにクランプ
	selectItemBox.scale = min(selectItemBox.scale, 1.0);

	//選択している項目が変化していたら
	if (selectItem != static_cast<int>(m_selectItem))
	{
		//変化してから何フレーム立ったかをリセットする
		m_selectItemMoveTime = 0;

		//選択している項目を示す画像を少し小さくする
		selectItemBox.scale = kSelectItemBoxMinScale;

		//選択している項目を示す画像を動かす
		MyEngine::Vector2 pos = MyEngine::Vector2(kMenuStringPosX, kMenuStringPosY);
		pos.y += kMenuStringDistanceY * selectItem;

		//座標を設定
		selectItemBox.pos = pos;
	}
	//選択している項目が変化していなければ
	else
	{
		m_selectItemMoveTime++;
	}
	
	//選択している項目を更新
	m_selectItem = static_cast<MenuItem>(selectItem);

	//メニュー画像のアルファ値を加算していく
	auto& menu = m_drawGraphs["Menu"];

	menu.alpha += kMenuFadeSpeed;
	//最大値を超えないようにクランプ
	menu.alpha = min(menu.alpha, 255);

}

void TutorialUi::UpdateStart()
{
}

void TutorialUi::UpdatePlaying()
{
}
