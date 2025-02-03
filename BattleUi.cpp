#include "BattleUi.h"
#include "BattleManager.h"
#include "GraphManager.h"
#include "SoundManager.h"
#include "Input.h"

namespace
{
	//メニューを表示する座標
	constexpr int kMenuPosX = Game::kWindowWidth / 2;
	constexpr int kMenuPosY = Game::kWindowHeight / 2;

	//リザルトのwinとloseを表示する座標
	constexpr int kResultLogoPosX = Game::kWindowWidth / 2;
	constexpr int kResultLogoPosY = 700;

	//リザルトのロゴを表示するまでの時間
	constexpr int kResultDisplayStartTime = 90;

	//リザルトのメニューを表示するまでの時間
	constexpr int kResultMenuDisplayStartTime = 360;

	//リザルトのメニューのフェードインの速度
	constexpr int kMenuFadeinSpeed = 20;

	//リザルトの後ろの黒いボックスの最大アルファ値
	constexpr int kMenuBackBoxMaxAlpha = 125;

	//リザルトのロゴの最初の拡大率
	constexpr double kResultLogoInitScale = 3.0;

	//リザルトのロゴの最後の拡大率
	constexpr double kResultLogoFinalScale = 1.0;

	//リザルトのロゴの縮小速度
	constexpr double kResultLogoScallingSpeed = 0.12;

	//リザルトのロゴを揺らす時間
	constexpr int kResultLogoShakeTime = 6;

	//リザルトのロゴを揺らす大きさ
	constexpr int kResultLogoShakeScale = 18;

	//上下入力でリピート入力を受け付ける時間
	constexpr int kRepeatInputTime = 15;

	//リピート入力の間隔
	constexpr int kRepeatInterval = 5;

	//フォントの名前
	const TCHAR* kFontName = "GN-キルゴUかなNB";

	//フォントのサイズ
	constexpr int kResultMenuFontSize = 50;

	//メニューの文字を表示する座標
	constexpr int kPlayMenuStringPosX = Game::kWindowWidth / 2;
	
	//メニューの文字
	const std::string kMenuString[static_cast<int>(BattleUi::MenuItem::kItemNum)] =
	{
		"バトルに戻る",
		"最初からやり直す",
		"キャラクターを選びなおす",
		"メニューに戻る"
	};

	constexpr int kPlayMenuStringPosY[static_cast<int>(BattleUi::MenuItem::kItemNum)] =
	{
		200,
		380,
		560,
		740
	};

	//リザルトの文字
	const std::string kResultString[static_cast<int>(BattleUi::ResultItem::kItemNum)] =
	{
		"もう一度戦う",
		"キャラクターを選びなおす",
		"メニューに戻る"
	};
	
	//リザルトの文字を表示する座標
	constexpr int kResultStringPosY[static_cast<int>(BattleUi::ResultItem::kItemNum)] =
	{
		250,
		430,
		610
	};



	//選択している項目を動かしたとき選択している項目をどこまで小さくするか
	constexpr double kSelectItemBoxMinScale = 0.7;

	//どこまで大きくするか
	constexpr double kSelectItemBoxMaxScale = 1.25;


	//選択している項目が大きくなっていく速度
	constexpr double kSelectItemBoxScalingSpeed = 0.06;

}

BattleUi::BattleUi() :
	m_selectItemMoveTime(0),
	m_decisionItem(-1),
	m_isWin(true),
	m_resultLogoScale(kResultLogoInitScale),
	m_resultTime(0),
	m_shakeTime(0),
	m_selectItem(0),
	m_resultMenuAlpha(0),
	m_menuFontHandle(-1),
	m_selectItemBoxScale(kSelectItemBoxMaxScale),
	m_menuAlpha(0),
	m_isResultMenuDisplay(false)
{
	//最初は何もしないように設定しておく
	m_updateFunc = &BattleUi::None;
	m_drawFunc = &BattleUi::None;

	m_menuFontHandle = CreateFontToHandle(kFontName, kResultMenuFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 3);
}

BattleUi::~BattleUi()
{

}

void BattleUi::Update()
{
	(this->*m_updateFunc)();
}

void BattleUi::Draw()
{
	(this->*m_drawFunc)();
}

void BattleUi::ChangeSituation(UiSituation situation)
{
	//初期化しておく
	m_decisionItem = -1;
	m_selectItem = 0;
	m_menuAlpha = 0;
	m_resultMenuAlpha = 0;
	m_resultTime = 0;
	m_resultLogoScale = kResultLogoInitScale;
	m_shakeTime = 0;
	m_selectItemBoxScale = kSelectItemBoxMaxScale;
	m_isResultMenuDisplay = false;

	switch (situation)
	{
	case BattleUi::UiSituation::kMenu:

		//メニュー
		m_updateFunc = &BattleUi::UpdateMenu;
		m_drawFunc = &BattleUi::DrawMenu;

		break;

	case BattleUi::UiSituation::kResult:

		//リザルト中
		m_updateFunc = &BattleUi::UpdateResult;
		m_drawFunc = &BattleUi::DrawResult;

		break;

	case BattleUi::UiSituation::kNone:

		//何もしない場合
		m_updateFunc = &BattleUi::None;
		m_drawFunc = &BattleUi::None;

		break;


	default:
		break;
	}
}

void BattleUi::UpdateMenu()
{

	//メニューのフェードインが終わるまで操作不可
	if (m_menuAlpha < 255) return;

	m_selectItemBoxScale += kSelectItemBoxScalingSpeed;

	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	auto lastItem = m_selectItem;

	//Aボタンを押したときに選択されている項目を実行する
	if (input->IsTrigger("A"))
	{
		m_decisionItem = m_selectItem;

		//サウンドを再生
		SoundManager::GetInstance().PlayOnceSound("Ok");
	}
	//上下入力を受け取る
	if (input->IsTrigger("Down"))
	{
		m_selectItem++;

	}
	else if (input->IsTrigger("Up"))
	{
		m_selectItem--;

	}

	//リピート入力
	if (m_selectItemMoveTime > kRepeatInterval)
	{
		if (input->GetPressTime("Up") > kRepeatInputTime)
		{
			m_selectItem--;
		}
		else if (input->GetPressTime("Down") > kRepeatInputTime)
		{
			m_selectItem++;
		}
	}

	//クランプ
	m_selectItem = min(m_selectItem, static_cast<int>(MenuItem::kItemEnd));
	m_selectItem = max(m_selectItem, 0);

	//前のフレームとからカーソルを動かしていたら
	if (lastItem != m_selectItem)
	{
		m_selectItemMoveTime = 0;

		//選択している項目の後ろのボックスのサイズを縮小する
		m_selectItemBoxScale = kSelectItemBoxMinScale;

		//サウンドを再生
		SoundManager::GetInstance().PlayOnceSound("Select");

	}
	//動かしていなければ
	else
	{
		m_selectItemMoveTime++;
	}

	//後ろのボックスのサイズのクランプ
	m_selectItemBoxScale = min(m_selectItemBoxScale, kSelectItemBoxMaxScale);
}

void BattleUi::UpdateResult()
{
	//リザルトのロゴの拡縮が終了したら操作可能にする
	if (m_resultLogoScale != kResultLogoFinalScale) return;

	//メニューのフェードインが終わるまで操作不可
	if (m_resultMenuAlpha < 255) return;

	m_selectItemBoxScale += kSelectItemBoxScalingSpeed;

	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	auto lastItem = m_selectItem;


	//Aボタンを押したときに選択されている項目を実行する
	if (input->IsTrigger("A"))
	{
		m_decisionItem = m_selectItem;

		//サウンドを再生
		SoundManager::GetInstance().PlayOnceSound("Ok");

	}
	//上下入力を受け取る
	if (input->IsTrigger("Down"))
	{
		m_selectItem++;

	}
	else if (input->IsTrigger("Up"))
	{
		m_selectItem--;

	}

	//リピート入力
	if (m_selectItemMoveTime > kRepeatInterval)
	{
		if (input->GetPressTime("Up") > kRepeatInputTime)
		{
			m_selectItem--;
		}
		else if (input->GetPressTime("Down") > kRepeatInputTime)
		{
			m_selectItem++;
		}
	}

	//クランプ
	m_selectItem = min(m_selectItem, static_cast<int>(ResultItem::kItemEnd));
	m_selectItem = max(m_selectItem, 0);

	//前のフレームとからカーソルを動かしていたら
	if (lastItem != m_selectItem)
	{
		m_selectItemMoveTime = 0;

		//選択している項目の後ろのボックスのサイズを縮小する
		m_selectItemBoxScale = kSelectItemBoxMinScale;

		//サウンドを再生する
		SoundManager::GetInstance().PlayOnceSound("Select");

	}
	//動かしていなければ
	else
	{
		m_selectItemMoveTime++;
	}

	//後ろのボックスのサイズのクランプ
	m_selectItemBoxScale = min(m_selectItemBoxScale, kSelectItemBoxMaxScale);

}

void BattleUi::UpdateStart1P()
{
}

void BattleUi::UpdateStart2P()
{
}


void BattleUi::DrawMenu()
{
	auto& graphManager = GraphManager::GetInstance();

	//メニューをフェードインさせる
	m_menuAlpha += kMenuFadeinSpeed;

	//画面全体を暗くする
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, min(m_menuAlpha, kMenuBackBoxMaxAlpha));

	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, GetColor(0, 0, 0), true);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_menuAlpha);

	//メニューを表示する
	DrawRotaGraph(kMenuPosX, kMenuPosY, 1.0, 0.0, graphManager.GetHandle("Menu"), true);

	//選択している項目を示すボックスを表示
	DrawRotaGraph(kPlayMenuStringPosX, kPlayMenuStringPosY[m_selectItem], m_selectItemBoxScale, 0.0, graphManager.GetHandle("SelectItem"), true);

	//メニューの文字を表示する
	for (int i = 0; i < static_cast<int>(BattleUi::MenuItem::kItemNum); i++)
	{
		MyEngine::Vector2 pos(kPlayMenuStringPosX, kPlayMenuStringPosY[i]);

		DrawStringCenter(kMenuString[i], pos, m_menuFontHandle, GetColor(255, 255, 255), GetColor(0, 0, 0));;
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}

void BattleUi::DrawResult()
{

	auto& graphManager = GraphManager::GetInstance();

	m_resultTime++;

	//リザルト画面になって一定時間たったら
	if (m_resultTime > kResultDisplayStartTime)
	{
		//ロゴの大きさを小さくしていく
		m_resultLogoScale -= kResultLogoScallingSpeed;

		//一定以上小さくならないようにする
		m_resultLogoScale = max(m_resultLogoScale, kResultLogoFinalScale);

		int logoPosX = kResultLogoPosX;
		int logoPosY = kResultLogoPosY;

		//一定以上小さくなったら少し揺らす
		if (m_resultLogoScale == kResultLogoFinalScale)
		{
			if (m_shakeTime < kResultLogoShakeTime)
			{
				//ロゴの座標を揺らす(ランダムがプラスしか出ないので半分減らす)
				logoPosX += GetRand(kResultLogoShakeScale) + static_cast<int>(kResultLogoShakeScale * 0.5);
				logoPosY += GetRand(kResultLogoShakeScale) + static_cast<int>(kResultLogoShakeScale * 0.5);
			}
			m_shakeTime++;
		}

		if (m_isWin)
		{
			DrawRotaGraph(logoPosX, logoPosY, m_resultLogoScale, 0.0, graphManager.GetHandle("Winner"), true);
		}
		else
		{
			DrawRotaGraph(logoPosX, logoPosY, m_resultLogoScale, 0.0, graphManager.GetHandle("Loser"), true);
		}
	}
	//さらに一定時間たったら
	if (m_resultTime > kResultMenuDisplayStartTime || m_isResultMenuDisplay)
	{
		//メニューをフェードインさせる
		m_resultMenuAlpha += kMenuFadeinSpeed;

		//画面全体を暗くする
		SetDrawBlendMode(DX_BLENDMODE_ALPHA,min(m_resultMenuAlpha,kMenuBackBoxMaxAlpha));

		DrawBox(0,0,Game::kWindowWidth,Game::kWindowHeight,GetColor(0,0,0),true);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA,m_resultMenuAlpha);

		//メニューを表示する
		DrawRotaGraph(kMenuPosX,kMenuPosY,1.0,0.0,graphManager.GetHandle("Menu"),true);
		
		//選択している項目を示すボックスを表示
		DrawRotaGraph(kPlayMenuStringPosX, kResultStringPosY[m_selectItem], m_selectItemBoxScale, 0.0, graphManager.GetHandle("SelectItem"), true);

		//メニューの文字を表示する
		for (int i = 0; i < static_cast<int>(BattleUi::ResultItem::kItemNum); i++)
		{
			MyEngine::Vector2 pos(kPlayMenuStringPosX, kResultStringPosY[i]);

			DrawStringCenter(kResultString[i], pos, m_menuFontHandle, GetColor(255, 255, 255), GetColor(0, 0, 0));;
		}

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	}


}

void BattleUi::DrawStart1P()
{

}

void BattleUi::DrawStart2P()
{
}

void BattleUi::DrawStringCenter(std::string string, MyEngine::Vector2 centerPos, int font, int color, int edgeColor)
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
		pos.x = centerPos.x;

		//文字数が1文字じゃなければ
		if (wordNum > 1)
		{
			//文字の座標を左にずらす
			pos.x -= static_cast<float>(GetFontSizeToHandle(font) * (wordNum + 1) / 2);
		}
		//文字数が一文字であれば
		else
		{
			//文字の座標を一文字分左にずらす
			pos.x -= static_cast<float>(GetFontSizeToHandle(font) * wordNum);
		}

		//半角分右に動かす
		pos.x += GetFontSizeToHandle(font) / 2;

		//文字の大きさの半分だけ上にずらす
		pos.y = centerPos.y - static_cast<float>(GetFontSizeToHandle(font) / 2);
	}

	//文字の描画
	DrawStringToHandle(static_cast<int>(pos.x), static_cast<int>(pos.y), string.c_str(), color, font, edgeColor);
}
