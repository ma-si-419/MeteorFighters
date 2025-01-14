#include "BattleUi.h"
#include "BattleManager.h"
#include "GraphManager.h"
#include "Input.h"

namespace
{
	constexpr int kMenuDrawPosX = Game::kWindowWidth / 2;
	constexpr int kMenuDrawPosY = Game::kWindowHeight / 2;

	//リザルトのwinとloseを表示する座標
	constexpr int kResultLogoPosX = Game::kWindowWidth / 2;
	constexpr int kResultLogoPosY = 700;

	//リザルトのロゴを表示するまでの時間
	constexpr int kResultDisplayStartTime = 90;

	//リザルトのロゴの最初の拡大率
	constexpr double kResultLogoDefaultScale = 3.0;

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
}

BattleUi::BattleUi():
	m_selectItemMoveTime(0)
{
	//最初は何もしないように設定しておく
	m_updateFunc = &BattleUi::None;
	m_drawFunc = &BattleUi::None;
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

}

void BattleUi::UpdateResult()
{
	//リザルトのロゴの拡縮が終了したら操作可能にする
	if (m_resultLogoScale != kResultLogoFinalScale) return;

	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//Aボタンを押したときに選択されている項目を実行する
	if (input->IsTrigger("A"))
	{
		m_decisionItem = m_selectItem;
	}
	//上下入力を受け取る
	if (input->IsTrigger("Down"))
	{
		m_selectItem++;

		m_selectItem = min(m_selectItem, static_cast<int>(ResultItem::kItemNum));
	}
	else if (input->IsTrigger("Up"))
	{
		m_selectItem--;

		m_selectItem = max(m_selectItem, 0);
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


}


void BattleUi::DrawMenu()
{
	auto& graphManager = GraphManager::GetInstance();

	//メニュー表示
	DrawRotaGraph(kMenuDrawPosX, kMenuDrawPosY, 1.0, 0.0, graphManager.GetHandle("Menu"), true);;

	//

}

void BattleUi::DrawResult()
{
	m_resultTime++;

	DrawString(50, 300, "もう一度たたかう", GetColor(255, 255, 255));
	DrawString(50, 350, "キャラクターを選びなおす", GetColor(255, 255, 255));
	DrawString(50, 400, "メインメニューに戻る", GetColor(255, 255, 255));

	int posY = 0;

	if (m_selectItem == static_cast<int>(ResultItem::kRetry))
	{
		posY = 300;
	}
	else if (m_selectItem == static_cast<int>(ResultItem::kCharacterSelect))
	{
		posY = 350;
	}
	else if (m_selectItem == static_cast<int>(ResultItem::kMenu))
	{
		posY = 400;
	}

	DrawString(30, posY, "→", GetColor(255, 255, 255));

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
			DrawRotaGraph(logoPosX, logoPosY, m_resultLogoScale, 0.0, GraphManager::GetInstance().GetHandle("Winner"), true);
		}
		else
		{
			DrawRotaGraph(logoPosX, logoPosY, m_resultLogoScale, 0.0, GraphManager::GetInstance().GetHandle("Loser"), true);
		}
	}

}
