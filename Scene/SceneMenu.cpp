#include "SceneMenu.h"
#include "SceneSelect.h"
#include "SceneTitle.h"
#include "DxLib.h"
#include "Input.h"

namespace
{
	//Uiの基本的な表示座標
	constexpr int kShowUiPosX = 200;
	constexpr int kShowUiPosY[static_cast<int>(SceneMenu::ItemKind::kItemKindNum)] = { 100,200,300,400,500 };

	//Uiを選択した時に動かす範囲
	constexpr int kShowUiMoveRange = 100;

	//Uiを動かす速さ
	constexpr int kShowUiMoveSpeed = 8;

	const std::string kshowUiString[static_cast<int>(SceneMenu::ItemKind::kItemKindNum)] =
	{
		"1P VS CPU",
		"1P VS 2P",
		"CPU VS CPU",
		"Option",
		"Exit"
	};

	const int kWhiteColor = GetColor(255, 255, 255);
}

SceneMenu::SceneMenu(SceneManager& sceneManager) :
	SceneBase(sceneManager),
	m_selectKind(0)
{
}

SceneMenu::~SceneMenu()
{
}

void SceneMenu::Init()
{
	for (int i = 0; i < static_cast<int>(ItemKind::kItemKindNum); i++)
	{
		m_showUi[i].showPosX = kShowUiPosX;
		m_showUi[i].showPosY = kShowUiPosY[i];
		m_showUi[i].showString = kshowUiString[i];
	}
}

void SceneMenu::Update()
{
	//選択している選択肢を右に動かす
	if (m_showUi[m_selectKind].showPosX < kShowUiPosX + kShowUiMoveRange)
	{
		//右に動かす
		m_showUi[m_selectKind].showPosX += kShowUiMoveSpeed;
		//行きすぎたら戻す
		if (m_showUi[m_selectKind].showPosX > kShowUiPosX + kShowUiMoveRange)
		{
			m_showUi[m_selectKind].showPosX = kShowUiPosX + kShowUiMoveRange;
		}
	}
	//選択されてない選択肢を左に動かす
	for (int i = 0; i < static_cast<int>(ItemKind::kItemKindNum); i++)
	{
		//選択されているかどうかを確認
		if (m_selectKind != i)
		{
			//左に動かす
			m_showUi[i].showPosX -= kShowUiMoveSpeed;
			//動かしすぎないように補正
			if (m_showUi[i].showPosX < kShowUiPosX)
			{
				m_showUi[i].showPosX = kShowUiPosX;
			}
		}
	}

	//上下入力で選択している項目を変化させる
	auto& input = MyEngine::Input::GetInstance();

	if (input.IsTrigger("Up"))
	{
		//項目がない部分にいかないように
		if (m_selectKind > 0)
		{
			m_selectKind--;
		}
	}
	else if (input.IsTrigger("Down"))
	{
		//項目がない部分に行かないように
		if (m_selectKind < static_cast<int>(ItemKind::kSelectEnd))
		{
			m_selectKind++;
		}
	}

	if (MyEngine::Input::GetInstance().IsTrigger("A"))
	{
		if (m_selectKind == static_cast<int>(ItemKind::k1PvsCPU))
		{
			m_sceneManager.ChangeScene(std::make_shared<SceneSelect>(m_sceneManager));
		}
		else if (m_selectKind == static_cast<int>(ItemKind::k1Pvs2P))
		{
			m_sceneManager.ChangeScene(std::make_shared<SceneSelect>(m_sceneManager));
		}
		else if (m_selectKind == static_cast<int>(ItemKind::kCPUvsCPU))
		{
			m_sceneManager.ChangeScene(std::make_shared<SceneSelect>(m_sceneManager));
		}
		else if (m_selectKind == static_cast<int>(ItemKind::kOption))
		{
		}
		else
		{
			m_sceneManager.ChangeScene(std::make_shared<SceneTitle>(m_sceneManager));
		}
	}
}

void SceneMenu::Draw()
{
#ifdef _DEBUG 
	//選択肢を表示する
	for (auto item : m_showUi)
	{
		//描画処理
		DrawString(item.showPosX, item.showPosY, item.showString.c_str(), kWhiteColor);

	}

	DrawString(0, 0, "SceneMenu", kWhiteColor);

#endif // _DEBUG

}

void SceneMenu::End()
{
}
