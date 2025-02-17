#include "TutorialUi.h"
#include "LoadCsv.h"
#include "TutorialManager.h"
#include "GraphManager.h"
#include "SoundManager.h"
#include "Input.h"

namespace
{
	//フォントの名前
	const TCHAR* kFontName = "GN-キルゴUかなNB";

	//スタート時に表示するフォントのサイズ
	constexpr int kStartFontSize = 84;
	//プレイ中の説明を表示するフォントのサイズ
	constexpr int kPlayingFontSize = 32;
	//プレイ中のメニューの文字のフォントのサイズ
	constexpr int kPlayMenuFontSize = 32;
	//スタートメニューの文字のフォントのサイズ
	constexpr int kStartMenuFontSize = 32;
	//セレクトメニューの文字のフォントのサイズ
	constexpr int kSelectMenuFontSize = 32;
	//セレクトメニューのチュートリアルの種類のフォントのサイズ
	constexpr int kSelectMenuTutorialKindFontSize = 48;


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
	constexpr int kButtonInterval = 100;

	//スタートメニューの初期化(画面外に出すため)
	constexpr int kStartMenuInitDistanceX = 600;

	//スタートメニューを表示する座標
	constexpr int kStartMenuPosX = 500;
	constexpr int kStartMenuPosY = 300;

	//スタートメニューの文字を表示する座標
	constexpr int kStartMenuStringPosX = kStartMenuPosX - 20;
	constexpr int kStartMenuStringPosY = kStartMenuPosY - 60;

	//スタートメニューの文字を表示する間隔
	constexpr int kStartMenuDistanceY = 90;

	//スタートメニューの文字を動かす速度
	constexpr int kStartMenuSelectItemMoveSpeed = 10;

	//スタートメニューの文字を動かす範囲
	constexpr int kStartMenuItemMoveRange = 60;

	//スタートメニューがはいってくるときの速さ
	constexpr int kStartMenuMoveSpeed = 60;

	//セレクトメニューの初期化(画面外に出すため)
	constexpr int kSelectMenuInitDistanceX = 600;

	//セレクトメニューを表示する座標
	constexpr int kSelectMenuPosX = Game::kWindowWidth - 500;
	constexpr int kSelectMenuPosY = Game::kWindowHeight - 300;

	//セレクトメニューの文字を表示する座標
	constexpr int kSelectMenuStringPosX = kSelectMenuPosX;
	constexpr int kSelectMenuStringPosY = kSelectMenuPosY - 60;

	//セレクトメニューの選択しているチュートリアルの種類を表示する座標
	constexpr int kSelectMenuTutorialKindPosX = kSelectMenuPosX - 5;
	constexpr int kSelectMenuTutorialKindPosY = kSelectMenuPosY - 180;

	//セレクトメニューの文字を表示する間隔
	constexpr int kSelectMenuDistanceY = 84;

	//セレクトメニューのRBとLBの文字とメニューの中心との距離
	constexpr int kSelectMenuRbLbDistance = 380;

	//セレクトメニューのRBとLBを表示する座標
	constexpr int kSelectMenuRbLbPosY = kSelectMenuTutorialKindPosY;

	//セレクトメニューがはいってくるときの速さ
	constexpr int kSelectMenuMoveSpeed = 60;

	//メニューが出てくるときのフェードインの速さ
	constexpr int kMenuFadeSpeed = 20;

	//メニューを表示しているときのゲームシーンにかける黒いボックスの最大アルファ値
	constexpr int kBackBoxMaxAlpha = 150;

	//上下入力でリピート入力を受け付ける時間
	constexpr int kRepeatInputTime = 15;

	//プレイメニューの左右入力でリピート入力を受け付ける時間
	constexpr int kPlayMenuChangeTutorialRepeatInputTime = 20;

	//リピート入力の間隔
	constexpr int kRepeatInterval = 5;

	//メニューの文字を表示する座標
	constexpr int kPlayMenuStringPosX = Game::kWindowWidth / 2;

	//左右で変更できるメニューの文字を表示する座標
	constexpr int kMoveMenuStringPosX = 650;

	//メニューでチュートリアルの名前を表示する座標
	constexpr int kPlayMenuTutorialNamePosX = 1005;

	//チュートリアルの左右の矢印とチュートリアルの名前の距離
	constexpr float kPlayMenuTutorialNameSideArrowDistance = 140.0f;

	//チュートリアルの左右の矢印を揺らす速さ
	constexpr float kPlayMenuTutorialNameSideArrowSpeed = 0.2f;

	//チュートリアルの左右の矢印を揺らす大きさ
	constexpr int kPlayMenuTutorialNameSideArrowScale = 4;

	//一つ目の高さ
	constexpr int kPlayMenuStringPosY = 175;

	//メニューの文字を表示する間隔
	constexpr int kPlayMenuStringDistanceY = 110;

	//プレイメニューで表示する文字列
	const std::string kPlayMenuStrings[static_cast<int>(TutorialUi::PlayMenuItem::kItemNum)] =
	{
		"状況をリセットする",
		"チュートリアルを変更",
		"チュートリアルの進め方",
		"チュートリアルセレクトに戻る",
		"メニューを閉じる",
		"チュートリアルを終了する"
	};

	//チュートリアルの進め方で表示する文字列
	const std::string kPlayMenuTutorialModeStrings[static_cast<int>(TutorialManager::TutorialMode::kModeNum)] =
	{
		"自動進行",
		"リピート",
		"進めない"
	};

	//スタートメニューで表示する文字列
	const std::string kStartMenuStrings[static_cast<int>(TutorialUi::StartMenuItem::kItemNum)] =
	{
		"順番にチュートリアルを行う",
		"チュートリアルを選択",
		"チュートリアルを終了"
	};

	//選択している項目を動かしたとき選択している項目をどこまで小さくするか
	constexpr double kSelectItemBoxMinScale = 0.7;

	//選択している項目が大きくなっていく速度
	constexpr double kSelectItemBoxScalingSpeed = 0.05;

	//クリア時の画像の初期拡大率
	constexpr double kSuccessGraphInitScale = 5.0;

	//クリア時の画像を小さくしていく速度
	constexpr double kSuccessGraphScalingSpeed = 0.3;

	//クリア時の画像の最終的な大きさ
	constexpr double kSuccessGraphFinalScale = 1.0;

	//クリア時の画像を揺らす時間
	constexpr int kSuccessGraphShakeTime = 4;

	//クリア時の画像を揺らす大きさ
	constexpr int kSuccessGraphShakeScale = 4;
	constexpr int kSuccessGraphShakeHalfScale = 2;

	//クリア後どのくらいまつか
	constexpr int kSuccessEndTime = 60;

	//スタートメニューの画像の名前
	const std::string kStartMenuGraphName = "StartMenu";

	//スタートメニューの選択している項目の画像の名前
	const std::string kStartMenuSelectItemGraphName = "StartMenuSelectItem";

	//スタートメニューの選択できる項目の画像の名前
	const std::string kStartMenuItemGraphName = "StartMenuItem";

	//チュートリアルセレクトメニューの画像の名前
	const std::string kSelectMenuGraphName = "SelectMenu";

	//チュートリアルセレクトメニューの選択している項目の画像の名前
	const std::string kSelectMenuSelectItemGraphName = "SelectMenuSelectItem";

	//チュートリアルセレクトメニューの選択できる項目の画像の名前
	const std::string kSelectMenuItemGraphName = "SelectMenuItem";

	//チュートリアルセレクトメニューのRBボタンの画像の名前
	const std::string kSelectMenuRBGraphName = "SelectMenuRB";

	//チュートリアルセレクトメニューのLBボタンの画像の名前
	const std::string kSelectMenuLBGraphName = "SelectMenuLB";

	//プレイメニューの画像の名前
	const std::string kPlayMenuGraphName = "PlayMenu";

	//プレイメニューの選択している項目の画像の名前
	const std::string kPlayMenuSelectItemGraphName = "PlayMenuSelectItem";

	//スタート時の後ろの画像の名前
	const std::string kStartBackGraphName = "StartBackBox";

	//ボタンのフレームの画像の名前
	const std::string kButtonFrameGraphName = "ButtonFrame";

	//ボタンの画像の名前
	const std::string kButtonGraphName = "Button";

	//+の画像の名前
	const std::string kPlusGraphName = "+";

	//成功時の画像の名前
	const std::string kSuccessGraphName = "Success";

}

TutorialUi::TutorialUi() :
	m_drawButtonNum(0),
	m_playMenuSelectItem(static_cast<PlayMenuItem>(0)),
	m_tutorialNumber(0),
	m_selectItemMoveTime(0),
	m_successTime(0),
	m_isSuccessEnd(false),
	m_selectTutorialNumber(0),
	m_selectTutorialStringArrowPos(0),
	m_isSuccessTutorial(false),
	m_isSelectStartMenu(false),
	m_isSelectSelectMenu(false),
	m_selectMenuIndexX(0),
	m_selectMenuIndexY(0),
	m_nowTutorialMode(0)
{
	LoadCsv load;

	m_tutorialSelectMenuData = load.LoadFile("data/csv/tutorialSelectMenuData.csv");

	m_updateFunc = &TutorialUi::UpdateStartMenu;

	m_startFontHandle = CreateFontToHandle(kFontName, kStartFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 3);
	m_playingFontHandle = CreateFontToHandle(kFontName, kPlayingFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 3);
	m_playMenuFontHandle = CreateFontToHandle(kFontName, kPlayMenuFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 2);
	m_startMenuFontHandle = CreateFontToHandle(kFontName, kStartMenuFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 2);
	m_selectMenuFontHandle = CreateFontToHandle(kFontName, kSelectMenuFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 2);
	m_selectTutorialKindFontHandle = CreateFontToHandle(kFontName, kSelectMenuTutorialKindFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 3);
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
	//フォントの削除
	DeleteFontToHandle(m_startFontHandle);
	DeleteFontToHandle(m_playingFontHandle);
	DeleteFontToHandle(m_playMenuFontHandle);
	DeleteFontToHandle(m_startMenuFontHandle);
	DeleteFontToHandle(m_selectMenuFontHandle);
	DeleteFontToHandle(m_selectTutorialKindFontHandle);
}

void TutorialUi::InitStartMenu()
{
	//画像マネージャー
	auto& manager = GraphManager::GetInstance();

	//メニュー画面の画像を登録する
	GraphData menu;
	menu.handle = manager.GetHandle(kStartMenuGraphName);

	//メニュー画面の座標を設定する
	menu.pos = MyEngine::Vector2(-kStartMenuPosX - kStartMenuInitDistanceX, kStartMenuPosY);
	m_drawGraphs[kStartMenuGraphName] = menu;

	//アップデートを変更する
	m_updateFunc = &TutorialUi::UpdateStartMenu;

	//メニュー画面で選択できる項目を示す画像を登録する
	for (int i = 0; i < static_cast<int>(TutorialUi::StartMenuItem::kItemNum); i++)
	{
		std::string name = kStartMenuItemGraphName;
		name += std::to_string(i);

		GraphData itemBox;

		itemBox.handle = manager.GetHandle(kStartMenuItemGraphName);
		itemBox.pos = MyEngine::Vector2(-kStartMenuStringPosX - kStartMenuInitDistanceX, kStartMenuStringPosY + (kStartMenuDistanceY * i));
		m_drawGraphs[name] = itemBox;
	}

	//メニュー画面で選択している項目を示す画像を登録する
	GraphData selectItemBox;
	selectItemBox.handle = manager.GetHandle(kStartMenuSelectItemGraphName);
	selectItemBox.pos = MyEngine::Vector2(-kStartMenuStringPosX - kStartMenuInitDistanceX, kStartMenuStringPosY);
	m_drawGraphs[kStartMenuSelectItemGraphName] = selectItemBox;

	//選択している項目を一番上にする
	m_startMenuSelectItem = static_cast<StartMenuItem>(0);

	//一定の位置にくるまで選択できないようにする
	m_isSelectStartMenu = false;
}

void TutorialUi::InitPlayMenu()
{
	//登録している画像をすべて消す
	m_drawGraphs.clear();

	//画像マネージャー
	auto& manager = GraphManager::GetInstance();

	//メニュー画面の画像を登録する
	GraphData menu;
	menu.handle = manager.GetHandle(kPlayMenuGraphName);
	menu.pos = MyEngine::Vector2(Game::kWindowWidth / 2, Game::kWindowHeight / 2);
	menu.alpha = 0;
	m_drawGraphs[kPlayMenuGraphName] = menu;

	//アップデートを変更する
	m_updateFunc = &TutorialUi::UpdatePlayMenu;

	//メニュー画面で選択している項目を示す画像を登録する
	GraphData selectItemBox;
	selectItemBox.handle = manager.GetHandle(kPlayMenuSelectItemGraphName);
	selectItemBox.pos = MyEngine::Vector2(kPlayMenuStringPosX, kPlayMenuStringPosY);
	m_drawGraphs[kPlayMenuSelectItemGraphName] = selectItemBox;

	//選択している項目を一番上にする
	m_playMenuSelectItem = static_cast<PlayMenuItem>(0);

}

void TutorialUi::InitStart()
{
	//登録している画像をすべて消す
	m_drawGraphs.clear();

	m_updateFunc = &TutorialUi::UpdateStart;

	auto& manager = GraphManager::GetInstance();

	GraphData data;

	data.pos = MyEngine::Vector2(Game::kWindowWidth / 2, Game::kWindowHeight / 2);

	data.handle = manager.GetHandle(kStartBackGraphName);

	m_drawGraphs[kStartBackGraphName] = data;
}

void TutorialUi::InitSelectMenu()
{
	m_updateFunc = &TutorialUi::UpdateSelectMenu;

	//選択している項目を一番上にする
	m_selectMenuIndexX = 0;

	//画像マネージャー
	auto& manager = GraphManager::GetInstance();

	//メニュー画面の画像を登録する
	GraphData menu;
	menu.handle = manager.GetHandle(kSelectMenuGraphName);
	menu.pos = MyEngine::Vector2(Game::kWindowWidth + kSelectMenuPosX + kSelectMenuInitDistanceX, kSelectMenuPosY);
	m_drawGraphs[kSelectMenuGraphName] = menu;

	//アップデートを変更する
	m_updateFunc = &TutorialUi::UpdateSelectMenu;

	//選択している項目をリセットする
	m_selectMenuIndexX = 0;
	m_selectMenuIndexY = 0;

	//メニュー画面で選択できる項目を示す画像を登録する
	for (int i = 0; i < m_tutorialSelectMenuData[m_selectMenuIndexX].size() + 1; i++)
	{
		std::string name = kSelectMenuItemGraphName;
		name += std::to_string(i);

		GraphData itemBox;

		itemBox.handle = manager.GetHandle(kSelectMenuItemGraphName);
		itemBox.pos = MyEngine::Vector2(Game::kWindowWidth + kSelectMenuStringPosX + kSelectMenuInitDistanceX, kSelectMenuStringPosY + (kSelectMenuDistanceY * i));
		m_drawGraphs[name] = itemBox;
	}

	//RBボタンを登録する
	GraphData rbButton;
	rbButton.handle = manager.GetHandle(kSelectMenuRBGraphName);
	rbButton.pos = MyEngine::Vector2(Game::kWindowWidth + kSelectMenuPosX + kSelectMenuRbLbDistance + kSelectMenuInitDistanceX, kSelectMenuRbLbPosY);
	m_drawGraphs[kSelectMenuRBGraphName] = rbButton;

	//LBボタンを登録する
	GraphData lbButton;
	lbButton.handle = manager.GetHandle(kSelectMenuLBGraphName);
	lbButton.pos = MyEngine::Vector2(Game::kWindowWidth + kSelectMenuPosX - kSelectMenuRbLbDistance + kSelectMenuInitDistanceX, kSelectMenuRbLbPosY);
	m_drawGraphs[kSelectMenuLBGraphName] = lbButton;

	//メニュー画面で選択している項目を示す画像を登録する
	GraphData selectItemBox;
	selectItemBox.handle = manager.GetHandle(kSelectMenuSelectItemGraphName);
	selectItemBox.pos = MyEngine::Vector2(Game::kWindowWidth + kSelectMenuStringPosX + kSelectMenuInitDistanceX, kSelectMenuStringPosY);
	m_drawGraphs[kSelectMenuSelectItemGraphName] = selectItemBox;

	//一定の位置にくるまで選択できないようにする
	m_isSelectSelectMenu = false;
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

	frame.handle = manager.GetHandle(kButtonFrameGraphName);

	m_drawGraphs[kButtonFrameGraphName] = frame;

	auto buttonString = m_tutorialPlayData[number][static_cast<int>(TutorialManager::TutorialPlayDataIndex::kButton)];

	auto button = stringFunc.Split(buttonString, '/');

	m_drawButtonNum = static_cast<int>(button.size());

	//ボタンの描画数が1個であれば
	if (m_drawButtonNum == 1)
	{
		GraphData buttonData;

		buttonData.handle = manager.GetHandle(button[0]);

		buttonData.pos = MyEngine::Vector2(kButtonPosX, kButtonPosY);

		std::string name = kButtonGraphName + "0";

		m_drawGraphs[name] = buttonData;
	}
	//ボタンの描画数が偶数であれば
	else if (m_drawButtonNum % 2 == 0)
	{
		//描画開始位置を計算する
		int startPosX = kButtonPosX - (kButtonInterval * m_drawButtonNum / 2);
		startPosX += kButtonInterval / 2;

		//描画するボタンを見て
		for (auto item : button)
		{
			//+ボタンの場合は座標をずらす
			if (item == kPlusGraphName)
			{
				startPosX += kButtonInterval / 2;
			}
		}


		int buttonNum = 0;

		//描画数だけループ
		for (int i = 0; i < m_drawButtonNum; i++)
		{
			GraphData buttonData;

			buttonData.handle = manager.GetHandle(button[i]);

			int posX = startPosX;

			posX += kButtonInterval * buttonNum;

			//+ボタンの場合は座標をずらす
			if (button[i] == kPlusGraphName)
			{
				posX -= kButtonInterval / 2;

				buttonNum--;
			}

			buttonData.pos = MyEngine::Vector2(posX, kButtonPosY);

			std::string name = kButtonGraphName + std::to_string(i);

			m_drawGraphs[name] = buttonData;

			buttonNum++;
		}
	}
	//奇数の場合
	else
	{
		//描画開始位置を計算する
		int startPosX = kButtonPosX;

		startPosX -= kButtonInterval * ((m_drawButtonNum - 1) / 2);

		//描画するボタンを見て
		for (auto item : button)
		{
			//+ボタンの場合は座標をずらす
			if (item == kPlusGraphName)
			{
				startPosX += kButtonInterval / 2;
			}
		}

		int buttonNum = 0;

		//描画数だけループ
		for (int i = 0; i < m_drawButtonNum; i++)
		{
			GraphData buttonData;

			buttonData.handle = manager.GetHandle(button[i]);

			int posX = startPosX;

			posX += kButtonInterval * buttonNum;

			//+ボタンの場合は座標をずらす
			if (button[i] == kPlusGraphName)
			{
				posX -= kButtonInterval / 2;

				buttonNum--;
			}

			buttonData.pos = MyEngine::Vector2(posX, kButtonPosY);

			std::string name = kButtonGraphName + std::to_string(i);

			m_drawGraphs[name] = buttonData;

			buttonNum++;
		}
	}

}

void TutorialUi::InitSuccess()
{
	auto& manager = GraphManager::GetInstance();

	m_updateFunc = &TutorialUi::UpdateSuccess;

	//クリア時の画像を取得
	GraphData success;
	success.handle = manager.GetHandle(kSuccessGraphName);
	//座標はボタンを表示している座標
	success.pos = MyEngine::Vector2(kButtonPosX, kButtonPosY);
	//最初は大きく
	success.scale = kSuccessGraphInitScale;

	m_drawGraphs[kSuccessGraphName] = success;

	m_isSuccessEnd = false;
	m_successTime = 0;

}

void TutorialUi::DrawStartMenu()
{
	GraphData menu = m_drawGraphs[kStartMenuGraphName];

	//メニューを描画
	DrawRotaGraph(static_cast<int>(menu.pos.x), static_cast<int>(menu.pos.y), menu.scale, 0.0, menu.handle, true);

	//アイテムの数だけループ
	for (int i = 0; i < static_cast<int>(TutorialUi::StartMenuItem::kItemNum); i++)
	{
		//文字列の裏のボックスを表示する
		std::string name = kStartMenuItemGraphName;
		name += std::to_string(i);

		GraphData itemBox = m_drawGraphs[name];

		DrawRotaGraph(static_cast<int>(itemBox.pos.x), static_cast<int>(itemBox.pos.y), itemBox.scale, 0.0, itemBox.handle, true);
	}

	//選択している項目のボックスを表示する
	GraphData selectItemBox = m_drawGraphs[kStartMenuSelectItemGraphName];

	DrawRotaGraph(static_cast<int>(selectItemBox.pos.x), static_cast<int>(selectItemBox.pos.y), selectItemBox.scale, 0.0, selectItemBox.handle, true);

	//アイテムの数だけループ
	for (int i = 0; i < static_cast<int>(TutorialUi::StartMenuItem::kItemNum); i++)
	{
		//ボックスの座標に合わせる
		std::string name = kStartMenuItemGraphName;
		name += std::to_string(i);

		GraphData itemBox = m_drawGraphs[name];

		//文字列の座標を設定
		MyEngine::Vector2 pos = itemBox.pos;

		DrawStringCenter(kStartMenuStrings[i], pos, m_startMenuFontHandle, GetColor(0, 0, 0), GetColor(255, 255, 255));
	}

	//セレクトメニューの画像が登録されていたら描画する
	if (m_drawGraphs.find(kSelectMenuGraphName) != m_drawGraphs.end())
	{
		//更新関数がスタートメニューなら
		if (m_updateFunc == &TutorialUi::UpdateStartMenu)
		{
			DrawSelectMenu();
		}
	}

}

void TutorialUi::DrawPlayMenu()
{
	GraphData menu = m_drawGraphs[kPlayMenuGraphName];

	//画面全体を覆う黒いボックスのアルファ値
	int backBoxAlpha = menu.alpha;

	backBoxAlpha = min(backBoxAlpha, kBackBoxMaxAlpha);

	//ブレンドモード変更
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, backBoxAlpha);

	//画面全体を覆う黒いボックス表示
	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, GetColor(0, 0, 0), true);

	//アルファ値変更
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, menu.alpha);

	DrawRotaGraph(static_cast<int>(menu.pos.x), static_cast<int>(menu.pos.y), menu.scale, 0.0, menu.handle, true);

	//ブレンドモードを元に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//選択している項目の後ろのボックスを表示する
	GraphData itemBox = m_drawGraphs[kPlayMenuSelectItemGraphName];

	DrawRotaGraph(static_cast<int>(itemBox.pos.x), static_cast<int>(itemBox.pos.y), itemBox.scale, 0.0, itemBox.handle, true);

	//ここから下は文字列を表示する処理

	//アルファ値をメニューと合わせる
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, menu.alpha);

	//文字列を表示する
	for (int i = 0; i < static_cast<int>(TutorialUi::PlayMenuItem::kItemNum); i++)
	{
		//一番上の文字の座標
		MyEngine::Vector2 pos = MyEngine::Vector2(kPlayMenuStringPosX, kPlayMenuStringPosY);

		//文字のY座標をずらす
		pos.y += kPlayMenuStringDistanceY * i;

		//左右に動かせる文字の座標を横にずらす(別の処理にしたい)

		if (i == static_cast<int>(TutorialUi::PlayMenuItem::kChangeTutorial) ||
			i == static_cast<int>(TutorialUi::PlayMenuItem::kChangeMode))
		{
			pos.x = kMoveMenuStringPosX;
		}

		//文字列を表示
		DrawStringCenter(kPlayMenuStrings[i], pos, m_playMenuFontHandle, GetColor(0, 0, 0), GetColor(255, 255, 255));

	}

	//選択しているチュートリアルを表示する
	MyEngine::Vector2 selectTutorialPos = MyEngine::Vector2(kPlayMenuTutorialNamePosX, kPlayMenuStringPosY + kPlayMenuStringDistanceY * static_cast<int>(PlayMenuItem::kChangeTutorial));
	std::string selectTutorialName = m_tutorialPlayData[m_selectTutorialNumber][static_cast<int>(TutorialManager::TutorialPlayDataIndex::kTutorialName)];
	DrawStringCenter(selectTutorialName, selectTutorialPos, m_playMenuFontHandle, GetColor(0, 0, 0), GetColor(255, 255, 255));

	//選択しているチュートリアルの左右の矢印を表示する
	float leftGap = kPlayMenuTutorialNameSideArrowDistance + sinf(m_selectTutorialStringArrowPos) * kPlayMenuTutorialNameSideArrowScale;
	float rightGap = kPlayMenuTutorialNameSideArrowDistance + sinf(m_selectTutorialStringArrowPos) * kPlayMenuTutorialNameSideArrowScale;

	//左右の矢印
	MyEngine::Vector2 leftArrowPos = MyEngine::Vector2(selectTutorialPos.x - leftGap - static_cast<float>(kPlayMenuFontSize), selectTutorialPos.y - static_cast<float>(kPlayMenuFontSize / 2));
	MyEngine::Vector2 rightArrowPos = MyEngine::Vector2(selectTutorialPos.x + rightGap, selectTutorialPos.y - static_cast<float>(kPlayMenuFontSize / 2));

	//左右の矢印を描画
	DrawStringToHandle(static_cast<int>(leftArrowPos.x), static_cast<int>(leftArrowPos.y), "＜", GetColor(0, 0, 0), m_playMenuFontHandle, GetColor(255, 255, 255));
	DrawStringToHandle(static_cast<int>(rightArrowPos.x), static_cast<int>(rightArrowPos.y), "＞", GetColor(0, 0, 0), m_playMenuFontHandle, GetColor(255, 255, 255));
	
	//チュートリアルの進め方を表示する
	MyEngine::Vector2 tutorialModePos = MyEngine::Vector2(kPlayMenuTutorialNamePosX, kPlayMenuStringPosY + kPlayMenuStringDistanceY * static_cast<int>(PlayMenuItem::kChangeMode));
	std::string tutorialModeString = kPlayMenuTutorialModeStrings[static_cast<int>(m_nowTutorialMode)];
	DrawStringCenter(tutorialModeString, tutorialModePos, m_playMenuFontHandle, GetColor(0, 0, 0), GetColor(255, 255, 255));

	//左右の矢印をチュートリアルの進め方の横にも表示する
	leftArrowPos.y += kPlayMenuStringDistanceY;
	rightArrowPos.y += kPlayMenuStringDistanceY;

	//左右の矢印を描画
	DrawStringToHandle(static_cast<int>(leftArrowPos.x), static_cast<int>(leftArrowPos.y), "＜", GetColor(0, 0, 0), m_playMenuFontHandle, GetColor(255, 255, 255));
	DrawStringToHandle(static_cast<int>(rightArrowPos.x), static_cast<int>(rightArrowPos.y), "＞", GetColor(0, 0, 0), m_playMenuFontHandle, GetColor(255, 255, 255));

	//ブレンドモードを元に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void TutorialUi::DrawStart(int number)
{
	GraphData backBox = m_drawGraphs[kStartBackGraphName];

	DrawRotaGraph(static_cast<int>(backBox.pos.x), static_cast<int>(backBox.pos.y), backBox.scale, 0.0, backBox.handle, true);

	std::string startWord = m_tutorialPlayData[number][static_cast<int>(TutorialManager::TutorialPlayDataIndex::kDescription)];

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

			shift.y += (GetFontSizeToHandle(m_startFontHandle) * 2) * i;

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
	GraphData frameData = m_drawGraphs[kButtonFrameGraphName];

	//ボタンを表示するフレーム
	DrawRotaGraph(static_cast<int>(frameData.pos.x), static_cast<int>(frameData.pos.y), frameData.scale, 0.0, frameData.handle, true);

	//プレイ中の説明
	auto playingString = m_tutorialPlayData[number][static_cast<int>(TutorialManager::TutorialPlayDataIndex::kPlayingString)];

	//プレイ中の説明を描画
	DrawStringCenter(playingString, MyEngine::Vector2(kPlayingStringPosX, kPlayingStringPosY), m_playingFontHandle, GetColor(255, 255, 255), GetColor(0, 0, 0));

	//押すボタンを表示する
	for (int i = 0; i < m_drawButtonNum; i++)
	{
		std::string name = kButtonGraphName + std::to_string(i);

		GraphData buttonData = m_drawGraphs[name];

		DrawRotaGraph(static_cast<int>(buttonData.pos.x), static_cast<int>(buttonData.pos.y), buttonData.scale, 0.0, buttonData.handle, true);
	}
}

void TutorialUi::DrawSuccess(int number)
{
	GraphData frameData = m_drawGraphs[kButtonFrameGraphName];

	//ボタンを表示するフレーム
	DrawRotaGraph(static_cast<int>(frameData.pos.x), static_cast<int>(frameData.pos.y), frameData.scale, 0.0, frameData.handle, true);

	//プレイ中の説明
	auto playingString = m_tutorialPlayData[number][static_cast<int>(TutorialManager::TutorialPlayDataIndex::kPlayingString)];

	//プレイ中の説明を描画
	DrawStringCenter(playingString, MyEngine::Vector2(kPlayingStringPosX, kPlayingStringPosY), m_playingFontHandle, GetColor(255, 255, 255), GetColor(0, 0, 0));

	//押すボタンを表示する
	for (int i = 0; i < m_drawButtonNum; i++)
	{
		std::string name = kButtonGraphName + std::to_string(i);

		GraphData buttonData = m_drawGraphs[name];

		DrawRotaGraph(static_cast<int>(buttonData.pos.x), static_cast<int>(buttonData.pos.y), buttonData.scale, 0.0, buttonData.handle, true);
	}

	GraphData success = m_drawGraphs[kSuccessGraphName];

	//クリア時の画像表示
	DrawRotaGraph(static_cast<int>(success.pos.x), static_cast<int>(success.pos.y), success.scale, 0.0, success.handle, true);
}

void TutorialUi::DrawSelectMenu()
{
	//スタートメニューの画像を描画
	if (m_updateFunc == &TutorialUi::UpdateSelectMenu)
	{
		DrawStartMenu();
	}

	//セレクトメニューの画像表示
	{
		GraphData menu = m_drawGraphs[kSelectMenuGraphName];
		//メニューを描画
		DrawRotaGraph(static_cast<int>(menu.pos.x), static_cast<int>(menu.pos.y), menu.scale, 0.0, menu.handle, true);
		//アイテムの数だけループ
		for (int i = 0; i < m_tutorialSelectMenuData[m_selectMenuIndexX + 1].size(); i++)
		{
			//文字列の裏のボックスを表示する
			std::string name = kSelectMenuItemGraphName;
			name += std::to_string(i);
			GraphData itemBox = m_drawGraphs[name];
			DrawRotaGraph(static_cast<int>(itemBox.pos.x), static_cast<int>(itemBox.pos.y), itemBox.scale, 0.0, itemBox.handle, true);
		}
		//選択している項目のボックスを表示する
		GraphData selectItemBox = m_drawGraphs[kSelectMenuSelectItemGraphName];
		DrawRotaGraph(static_cast<int>(selectItemBox.pos.x), static_cast<int>(selectItemBox.pos.y), selectItemBox.scale, 0.0, selectItemBox.handle, true);
		//アイテムの数だけループ
		for (int i = 0; i < m_tutorialSelectMenuData[m_selectMenuIndexX + 1].size(); i++)
		{
			//ボックスの座標に合わせる
			std::string name = kSelectMenuItemGraphName;
			name += std::to_string(i);
			GraphData itemBox = m_drawGraphs[name];
			//文字列の座標を設定
			MyEngine::Vector2 pos = itemBox.pos;
			DrawStringCenter(m_tutorialSelectMenuData[m_selectMenuIndexX + 1][i], pos, m_selectMenuFontHandle, GetColor(0, 0, 0), GetColor(255, 255, 255));
		}
		//選択している大項目を表示する
		MyEngine::Vector2 selectTutorialKindStringPos = menu.pos;

		//Y座標の設定
		selectTutorialKindStringPos.y = kSelectMenuTutorialKindPosY;

		//描画
		DrawStringCenter(m_tutorialSelectMenuData[m_selectMenuIndexX][0], selectTutorialKindStringPos, m_selectTutorialKindFontHandle, GetColor(64, 64, 255), GetColor(255, 255, 16));

		//RBボタンを表示する
		GraphData rbButton = m_drawGraphs[kSelectMenuRBGraphName];
		DrawRotaGraph(static_cast<int>(rbButton.pos.x), static_cast<int>(rbButton.pos.y), rbButton.scale, 0.0, rbButton.handle, true);
		//LBボタンを表示する
		GraphData lbButton = m_drawGraphs[kSelectMenuLBGraphName];
		DrawRotaGraph(static_cast<int>(lbButton.pos.x), static_cast<int>(lbButton.pos.y), lbButton.scale, 0.0, lbButton.handle, true);
	}
}

void TutorialUi::DrawStringCenter(std::string string, MyEngine::Vector2 centerPos, int font, int color, int edgeColor)
{
	// 文字を表示する座標
	MyEngine::Vector2 pos;

	// 文字数
	int wordNum = GetStringLength(string.c_str());

	// 文字の幅を計算
	int charWidth = GetFontSizeToHandle(font) + GetFontEdgeSizeToHandle(font);

	// 文字数が偶数の場合
	if (wordNum % 2 == 0)
	{
		// 文字の座標を左にずらす
		pos.x = centerPos.x - static_cast<float>(charWidth * wordNum / 2);
	}
	// 奇数の場合
	else
	{
		// 文字の座標を左にずらす
		pos.x = centerPos.x - static_cast<float>(charWidth * (wordNum / 2)) - static_cast<float>(charWidth / 2);
	}

	// 文字の大きさの半分だけ上にずらす
	pos.y = centerPos.y - static_cast<float>(charWidth / 2);

	// 文字の描画
	DrawStringToHandle(static_cast<int>(pos.x), static_cast<int>(pos.y), string.c_str(), color, font, edgeColor);
}

void TutorialUi::UpdateStartMenu()
{
	//入力
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//m_drawGraphsにkSelectMenuGraphNameがあるのなら
	if (m_drawGraphs.find(kSelectMenuGraphName) != m_drawGraphs.end())
	{
		//選択メニューを右に動かす
		GraphData menu = m_drawGraphs[kSelectMenuGraphName];
		menu.pos.x += kStartMenuMoveSpeed;
		//メニュー画面の座標を更新
		m_drawGraphs[kSelectMenuGraphName] = menu;
		//選択できる項目を数だけループ
		for (int i = 0; i < m_tutorialSelectMenuData[m_selectMenuIndexX].size() + 1; i++)
		{
			//選択できる項目を示す画像
			std::string name = kSelectMenuItemGraphName;
			name += std::to_string(i);
			auto& itemBox = m_drawGraphs[name];
			//選択できる項目を右に動かす
			itemBox.pos.x += kStartMenuMoveSpeed;
			//選択できる項目を更新
			m_drawGraphs[name] = itemBox;
		}
		//選択している項目を右に動かす
		auto& selectItemBox = m_drawGraphs[kSelectMenuSelectItemGraphName];
		selectItemBox.pos.x += kStartMenuMoveSpeed;
		//選択している項目を更新
		m_drawGraphs[kSelectMenuSelectItemGraphName] = selectItemBox;

		//RBボタンを右に動かす
		auto& rbButton = m_drawGraphs[kSelectMenuRBGraphName];
		rbButton.pos.x += kStartMenuMoveSpeed;
		m_drawGraphs[kSelectMenuRBGraphName] = rbButton;

		//LBボタンを右に動かす
		auto& lbButton = m_drawGraphs[kSelectMenuLBGraphName];
		lbButton.pos.x += kStartMenuMoveSpeed;
		m_drawGraphs[kSelectMenuLBGraphName] = lbButton;
	}

	//スタートメニューの画像を右に動かす
	if (!m_isSelectStartMenu)
	{
		//メニュー画像を右に動かす
		GraphData menu = m_drawGraphs[kStartMenuGraphName];
		menu.pos.x += kStartMenuMoveSpeed;

		//クランプ
		menu.pos.x = fmin(menu.pos.x, static_cast<float>(kStartMenuPosX));

		//メニューが一定の位置に来たら
		if (menu.pos.x == static_cast<float>(kStartMenuPosX))
		{
			//選択できるようにする
			m_isSelectStartMenu = true;
		}

		//メニュー画面の座標を更新
		m_drawGraphs[kStartMenuGraphName] = menu;

		//選択できる項目を数だけループ
		for (int i = 0; i < static_cast<int>(TutorialUi::StartMenuItem::kItemNum); i++)
		{
			//選択できる項目を示す画像
			std::string name = kStartMenuItemGraphName;
			name += std::to_string(i);
			auto& itemBox = m_drawGraphs[name];
			//選択できる項目を右に動かす
			itemBox.pos.x += kStartMenuMoveSpeed;
			//クランプ
			itemBox.pos.x = fmin(itemBox.pos.x, static_cast<float>(kStartMenuStringPosX));
			//選択できる項目を更新
			m_drawGraphs[name] = itemBox;
		}

		//選択している項目を右に動かす
		auto& selectItemBox = m_drawGraphs[kStartMenuSelectItemGraphName];

		selectItemBox.pos.x += kStartMenuMoveSpeed;

		//クランプ
		selectItemBox.pos.x = fmin(selectItemBox.pos.x, static_cast<float>(kStartMenuStringPosX));

		//選択している項目を更新
		m_drawGraphs[kStartMenuSelectItemGraphName] = selectItemBox;
	}
	else
	{
		//選択している項目
		int selectItem = static_cast<int>(m_startMenuSelectItem);


		//上下入力で選択しているものを変更する
		if (input->IsTrigger("Up"))
		{
			selectItem--;

			//一番上で上入力した場合
			if (selectItem < 0)
			{
				//一番下に移動する
				selectItem = static_cast<int>(StartMenuItem::kItemEnd);
			}
		}
		else if (input->IsTrigger("Down"))
		{
			selectItem++;

			//一番下で入力された場合
			if (selectItem > static_cast<int>(StartMenuItem::kItemEnd))
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
		selectItem = min(selectItem, static_cast<int>(StartMenuItem::kItemEnd));

		//選択している項目を表す画像
		auto& selectItemBox = m_drawGraphs[kStartMenuSelectItemGraphName];

		//選択している項目が変化していたら
		if (selectItem != static_cast<int>(m_startMenuSelectItem))
		{
			//変化してから何フレーム立ったかをリセットする
			m_selectItemMoveTime = 0;

			//選択している項目を示す画像を動かす
			MyEngine::Vector2 pos = MyEngine::Vector2(kStartMenuStringPosX, kStartMenuStringPosY);
			pos.y += kStartMenuDistanceY * selectItem;

			//座標を設定
			selectItemBox.pos = pos;

			//サウンドを再生
			SoundManager::GetInstance().PlayOnceSound("Select");

		}
		//選択している項目が変化していなければ
		else
		{
			m_selectItemMoveTime++;
		}

		//選択している項目を更新
		m_startMenuSelectItem = static_cast<StartMenuItem>(selectItem);

		//選択されている項目を右に動かしていく
		std::string selectItemName = kStartMenuItemGraphName;
		selectItemName += std::to_string(selectItem);

		auto& itemBox = m_drawGraphs[selectItemName];

		itemBox.pos.x += kStartMenuSelectItemMoveSpeed;

		//クランプ
		itemBox.pos.x = fmin(itemBox.pos.x, static_cast<float>(kStartMenuStringPosX + kStartMenuItemMoveRange));

		//選択している項目を表す画像も一緒に動かす
		std::string yellowBoxName = kStartMenuSelectItemGraphName;

		auto& yellowBox = m_drawGraphs[yellowBoxName];

		yellowBox.pos.x = itemBox.pos.x;

		//選択されていない項目を左に動かしていく
		for (int i = 0; i < static_cast<int>(StartMenuItem::kItemNum); i++)
		{
			//選択されている項目はスキップ
			if (i == selectItem)
			{
				continue;
			}
			std::string name = kStartMenuItemGraphName;
			name += std::to_string(i);
			auto& item = m_drawGraphs[name];
			item.pos.x -= kStartMenuSelectItemMoveSpeed;
			//クランプ
			item.pos.x = fmax(item.pos.x, static_cast<float>(kStartMenuStringPosX));
		}
	}
}

void TutorialUi::UpdateSelectMenu()
{
	if (!m_isSelectSelectMenu)
	{

		{
			//スタートメニューの画像を左に動かす
			GraphData menu = m_drawGraphs[kStartMenuGraphName];
			menu.pos.x -= kStartMenuMoveSpeed;

			//選択肢の画像を左に動かす
			for (int i = 0; i < static_cast<int>(TutorialUi::StartMenuItem::kItemNum); i++)
			{
				std::string name = kStartMenuItemGraphName;
				name += std::to_string(i);
				auto& itemBox = m_drawGraphs[name];
				itemBox.pos.x -= kStartMenuMoveSpeed;

				m_drawGraphs[name] = itemBox;
			}

			//選択している項目を示す画像を左に動かす
			auto& selectItemBox = m_drawGraphs[kStartMenuSelectItemGraphName];
			selectItemBox.pos.x -= kStartMenuMoveSpeed;

			//メニュー画像の座標を更新
			m_drawGraphs[kStartMenuGraphName] = menu;

			//選択している項目を示す画像を更新
			m_drawGraphs[kStartMenuSelectItemGraphName] = selectItemBox;
		}

		//セレクトメニューの画像を左に動かす
		GraphData menu = m_drawGraphs[kSelectMenuGraphName];
		menu.pos.x -= kSelectMenuMoveSpeed;

		//クランプ
		menu.pos.x = fmax(menu.pos.x, static_cast<float>(kSelectMenuPosX));

		//一定の座標まで来たら
		if (menu.pos.x == static_cast<float>(kSelectMenuPosX))
		{
			//選択できるようにする
			m_isSelectSelectMenu = true;
		}

		//メニュー画像の座標を更新
		m_drawGraphs[kSelectMenuGraphName] = menu;

		//選択できる項目を数だけループ
		for (int i = 0; i < m_tutorialSelectMenuData[m_selectMenuIndexX].size() + 1; i++)
		{
			//選択できる項目を示す画像
			std::string name = kSelectMenuItemGraphName;
			name += std::to_string(i);
			auto& itemBox = m_drawGraphs[name];
			//選択できる項目を左に動かす
			itemBox.pos.x -= kSelectMenuMoveSpeed;
			//クランプ
			itemBox.pos.x = fmax(itemBox.pos.x, static_cast<float>(kSelectMenuStringPosX));
			//選択できる項目を更新
			m_drawGraphs[name] = itemBox;
		}

		//選択している項目を左に動かす
		auto& selectItemBox = m_drawGraphs[kSelectMenuSelectItemGraphName];
		selectItemBox.pos.x -= kSelectMenuMoveSpeed;

		//クランプ
		selectItemBox.pos.x = fmax(selectItemBox.pos.x, static_cast<float>(kSelectMenuStringPosX));

		//選択している項目を更新
		m_drawGraphs[kSelectMenuSelectItemGraphName] = selectItemBox;

		//LBボタンを左に動かす
		auto& lbButton = m_drawGraphs[kSelectMenuLBGraphName];
		lbButton.pos.x -= kSelectMenuMoveSpeed;

		//クランプ
		lbButton.pos.x = fmax(lbButton.pos.x, static_cast<float>(kSelectMenuPosX - kSelectMenuRbLbDistance));

		//LBボタンを更新
		m_drawGraphs[kSelectMenuLBGraphName] = lbButton;

		//RBボタンを左に動かす
		auto& rbButton = m_drawGraphs[kSelectMenuRBGraphName];
		rbButton.pos.x -= kSelectMenuMoveSpeed;

		//クランプ
		rbButton.pos.x = fmax(rbButton.pos.x, static_cast<float>(kSelectMenuPosX + kSelectMenuRbLbDistance));

		//RBボタンを更新
		m_drawGraphs[kSelectMenuRBGraphName] = rbButton;

	}
	else
	{
		auto input = MyEngine::Input::GetInstance().GetInputData(0);

		//前のフレームで選択している項目
		int lastSelectMenuIndexX = m_selectMenuIndexX;
		int lastSelectMenuIndexY = m_selectMenuIndexY;

		//RBLB入力で選択しているチュートリアルの種類を変更する
		if (input->IsTrigger("RB"))
		{
			//CSVデータの配置の都合上+2する
			m_selectMenuIndexX += 2;
			//一番右で右入力した場合
			if (m_selectMenuIndexX > m_tutorialSelectMenuData.size() - 2)
			{
				//一番左に移動する
				m_selectMenuIndexX = 0;
			}
			//選択している項目をリセットする
			m_selectMenuIndexY = 0;
		}
		else if (input->IsTrigger("LB"))
		{
			//CSVデータの配置の都合上-2する
			m_selectMenuIndexX -= 2;
			//一番左で左入力した場合
			if (m_selectMenuIndexX < 0)
			{
				//一番右に移動する
				m_selectMenuIndexX = m_tutorialSelectMenuData.size() - 2;
			}
			//選択している項目をリセットする
			m_selectMenuIndexY = 0;
		}

		//選択している項目が変化していたら
		if (m_selectMenuIndexX != lastSelectMenuIndexX)
		{
			//サウンドを再生
			SoundManager::GetInstance().PlayOnceSound("Select");
		}



		//上下入力で選択しているものを変更する
		if (input->IsTrigger("Up"))
		{
			m_selectMenuIndexY--;

			//一番上で上入力した場合
			if (m_selectMenuIndexY < 0)
			{
				//一番下に移動する(CSVのデータの配置の都合上Indexに+1する)
				m_selectMenuIndexY = m_tutorialSelectMenuData[m_selectMenuIndexX + 1].size() - 1;
			}
		}
		else if (input->IsTrigger("Down"))
		{
			m_selectMenuIndexY++;

			//一番下で入力された場合(CSVのデータの配置の都合上Indexに+1する)
			if (m_selectMenuIndexY > m_tutorialSelectMenuData[m_selectMenuIndexX + 1].size() - 1)
			{
				//一番上に移動する
				m_selectMenuIndexY = 0;
			}
		}

		//リピート入力
		if (m_selectItemMoveTime > kRepeatInterval)
		{
			if (input->GetPressTime("Up") > kRepeatInputTime)
			{
				m_selectMenuIndexY--;
			}
			else if (input->GetPressTime("Down") > kRepeatInputTime)
			{
				m_selectMenuIndexY++;
			}
		}

		//最大値と最小値を超えないように
		m_selectMenuIndexY = max(m_selectMenuIndexY, 0);
		m_selectMenuIndexY = min(m_selectMenuIndexY, m_tutorialSelectMenuData[m_selectMenuIndexX + 1].size() - 1);

		//選択している項目を表す画像
		auto& selectItemBox = m_drawGraphs[kSelectMenuSelectItemGraphName];

		//選択している項目が変化していたら
		if (m_selectMenuIndexY != lastSelectMenuIndexY)
		{
			//変化してから何フレーム立ったかをリセットする
			m_selectItemMoveTime = 0;

			//選択している項目を示す画像を動かす
			MyEngine::Vector2 pos = MyEngine::Vector2(kSelectMenuStringPosX, kSelectMenuStringPosY);
			pos.y += kSelectMenuDistanceY * m_selectMenuIndexY;

			//座標を設定
			selectItemBox.pos = pos;

			//サウンドを再生
			SoundManager::GetInstance().PlayOnceSound("Select");
		}
		//選択している項目が変化していなければ
		else
		{
			m_selectItemMoveTime++;
		}
	}


}

void TutorialUi::UpdatePlayMenu()
{
	//入力
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//選択している項目
	int selectItem = static_cast<int>(m_playMenuSelectItem);

	//上下入力で選択しているものを変更する
	if (input->IsTrigger("Up"))
	{
		selectItem--;

		//一番上で上入力した場合
		if (selectItem < 0)
		{
			//一番下に移動する
			selectItem = static_cast<int>(TutorialUi::PlayMenuItem::kItemEnd);
		}
	}
	else if (input->IsTrigger("Down"))
	{
		selectItem++;

		//一番下で入力された場合
		if (selectItem > static_cast<int>(TutorialUi::PlayMenuItem::kItemEnd))
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
	selectItem = min(selectItem, static_cast<int>(PlayMenuItem::kItemEnd));


	//選択している項目を表す画像
	auto& selectItemBox = m_drawGraphs[kPlayMenuSelectItemGraphName];

	//選択している項目を示す画像を少しずつ大きくする
	selectItemBox.scale += kSelectItemBoxScalingSpeed;

	//大きくなりすぎないようにクランプ
	selectItemBox.scale = min(selectItemBox.scale, 1.0);

	//選択している項目が変化していたら
	if (selectItem != static_cast<int>(m_playMenuSelectItem))
	{
		//変化してから何フレーム立ったかをリセットする
		m_selectItemMoveTime = 0;

		//選択している項目を示す画像を少し小さくする
		selectItemBox.scale = kSelectItemBoxMinScale;

		//選択している項目を示す画像を動かす
		MyEngine::Vector2 pos = MyEngine::Vector2(kPlayMenuStringPosX, kPlayMenuStringPosY);
		pos.y += kPlayMenuStringDistanceY * selectItem;

		//座標を設定
		selectItemBox.pos = pos;

		//サウンドを再生
		SoundManager::GetInstance().PlayOnceSound("Select");

	}
	//選択している項目が変化していなければ
	else
	{
		m_selectItemMoveTime++;
	}

	//左右に動かせる項目であれば
	if (m_playMenuSelectItem == PlayMenuItem::kChangeTutorial ||
		m_playMenuSelectItem == PlayMenuItem::kChangeMode)
	{
		int last = -1;
		int now = -1;

		//選択している項目
		if (m_playMenuSelectItem == PlayMenuItem::kChangeTutorial)
		{
			last = m_selectTutorialNumber;
			now = m_selectTutorialNumber;
		}
		else if (m_playMenuSelectItem == PlayMenuItem::kChangeMode)
		{
			last = m_nowTutorialMode;
			now = m_nowTutorialMode;
		}

		//右を押したら
		if (input->IsTrigger("Right"))
		{
			now++;


		}
		//左を押したら
		else if (input->IsTrigger("Left"))
		{
			now--;
		}

		if (m_playMenuSelectItem == PlayMenuItem::kChangeTutorial)
		{
			//一番右で右入力した場合
			if (now >= static_cast<int>(TutorialManager::TutorialKind::kTutorialNum))
			{
				//一番左に移動する
				now = 0;
			}
			//一番左で左入力した場合
			else if (now < 0)
			{
				//一番右に移動する
				now = static_cast<int>(TutorialManager::TutorialKind::kTutorialNum) - 1;
			}
		}
		else if (m_playMenuSelectItem == PlayMenuItem::kChangeMode)
		{
			//一番右で右入力した場合
			if (now >= static_cast<int>(TutorialManager::TutorialMode::kModeNum))
			{
				//一番左に移動する
				now = 0;
			}
			//一番左で左入力した場合
			else if (now < 0)
			{
				//一番右に移動する
				now = static_cast<int>(TutorialManager::TutorialMode::kModeNum) - 1;
			}
		}

		//リピート入力
		if (m_selectItemMoveTime > kPlayMenuChangeTutorialRepeatInputTime)
		{
			if (input->GetPressTime("Right") > kPlayMenuChangeTutorialRepeatInputTime)
			{
				now++;
			}
			else if (input->GetPressTime("Left") > kPlayMenuChangeTutorialRepeatInputTime)
			{
				now--;
			}
		}

		if (m_playMenuSelectItem == PlayMenuItem::kChangeTutorial)
		{
			//クランプ
			now = max(now, 0);
			now = min(now, static_cast<int>(TutorialManager::TutorialKind::kTutorialNum) - 1);
		}
		else if (m_playMenuSelectItem == PlayMenuItem::kChangeMode)
		{
			//クランプ
			now = max(now, 0);
			now = min(now, static_cast<int>(TutorialManager::TutorialMode::kModeNum) - 1);
		}

		//選択している項目が変化していたら
		if (last != now)
		{
			//変化してから何フレーム立ったかをリセットする
			m_selectItemMoveTime = 0;

			//サウンドを再生
			SoundManager::GetInstance().PlayOnceSound("Select");
		}

		//選択している項目を更新
		if (m_playMenuSelectItem == PlayMenuItem::kChangeTutorial)
		{
			m_selectTutorialNumber = now;
		}
		else if (m_playMenuSelectItem == PlayMenuItem::kChangeMode)
		{
			m_nowTutorialMode = now;
		}
	}

	//左右の矢印を揺らす
	m_selectTutorialStringArrowPos += kPlayMenuTutorialNameSideArrowSpeed;

	//選択している項目を更新
	m_playMenuSelectItem = static_cast<PlayMenuItem>(selectItem);

	//メニュー画像のアルファ値を加算していく
	auto& menu = m_drawGraphs[kPlayMenuGraphName];

	menu.alpha += kMenuFadeSpeed;
	//最大値を超えないようにクランプ
	menu.alpha = min(menu.alpha, 255);

}

void TutorialUi::UpdateStart()
{
}

void TutorialUi::UpdatePlaying()
{
	//成功していたら
	if (m_isSuccessTutorial)
	{
		InitSuccess();
	}
}

void TutorialUi::UpdateSuccess()
{
	//クリア時の画像を少しずつ小さくしていく
	auto& success = m_drawGraphs[kSuccessGraphName];

	success.scale -= kSuccessGraphScalingSpeed;

	//クランプ
	success.scale = max(success.scale, kSuccessGraphFinalScale);

	//最小まで小さくなったら
	if (success.scale == kSuccessGraphFinalScale)
	{
		//少し揺らす
		if (m_successTime < kSuccessGraphShakeTime)
		{
			success.pos.x = static_cast<float>((kButtonPosX + GetRand(kSuccessGraphShakeScale)) - kSuccessGraphShakeHalfScale);
			success.pos.y = static_cast<float>((kButtonPosY + GetRand(kSuccessGraphShakeScale)) - kSuccessGraphShakeHalfScale);
		}

		if (m_successTime > kSuccessEndTime)
		{
			//演出が終わったフラグを立てる
			m_isSuccessEnd = true;
		}

		m_successTime++;
	}
}
