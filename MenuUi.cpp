#include "MenuUi.h"
#include "SoundManager.h"
#include "GraphManager.h"
#include "Input.h"
#include "Game.h"

namespace
{
	//文字列の基本的な表示座標
	constexpr int kStringUiPosX = 30;
	constexpr int kStringUiPosY[static_cast<int>(MenuUi::SelectItem::kItemNum)] =
	{
		170,
		350,
		530,
		710
	};

	constexpr int kItemBoxUiPosX = 240;
	constexpr int kItemBoxUiPosY[static_cast<int>(MenuUi::SelectItem::kItemNum)] =
	{
		200,
		380,
		560,
		740
	};

	//選択しているボックスのアルファ値を変更する速度
	constexpr int kSelectItemBoxBlendSpeed = 20;

	//Uiを選択した時に動かす範囲
	constexpr int kShowUiMoveRange = 130;

	//Uiを動かす速さ
	constexpr int kShowUiMoveSpeed = 18;

	//カメラのNearFar
	constexpr float kCameraNear = 0.1f;
	constexpr float kCameraFar = 100.0f;

	//カメラを回す速度
	constexpr float kCameraRotaSpeed = 0.001f;

	//表示する文字列
	const std::string kUiString[static_cast<int>(MenuUi::SelectItem::kItemNum)] =
	{
		"CPUとバトル",
		"チュートリアル",
		"タイトルに戻る",
		"ゲームを終了する",
	};

	//フォントの名前
	const TCHAR* kFontName = "GN-キルゴUかなNB";

}


MenuUi::MenuUi():
	m_selectItemFontHandle(-1),
	m_selectItem(SelectItem::kBattle)
{
	m_selectItemFontHandle = CreateFontToHandle(kFontName,64,0,DX_FONTTYPE_ANTIALIASING_EDGE,-1,3);
	m_skyDomeHandle = MV1LoadModel("data/model/Dome.mv1");
}

MenuUi::~MenuUi()
{
	DeleteFontToHandle(m_selectItemFontHandle);
}

void MenuUi::Init()
{
	//カメラの設定
	SetCameraPositionAndTarget_UpVecY(VGet(0,0,0),VGet(0,0,1));

	//NearFarの設定
	SetCameraNearFar(kCameraNear,kCameraFar);


	//文字列を設定する
	for (int i = 0; i < static_cast<int>(SelectItem::kItemNum); i++)
	{
		m_stringUi[i].showPosX = kStringUiPosX;
		m_stringUi[i].showPosY = kStringUiPosY[i];
		m_stringUi[i].showString = kUiString[i];
	}

	//選択できる項目のボックスの画像を設定する
	for (int i = 0; i < static_cast<int>(SelectItem::kItemNum); i++)
	{
		std::string name = "ItemBox";
		name += std::to_string(i);

		GraphUiStatus status;

		status.handle = GraphManager::GetInstance().GetHandle("ItemBox");
		status.showPosX = kItemBoxUiPosX;
		status.showPosY = kItemBoxUiPosY[i];

		EntryGraph(name, status);
	}

	//選択している項目のボックスの画像を設定する
	std::string name = "SelectItemBox";

	GraphUiStatus status;

	status.handle = GraphManager::GetInstance().GetHandle("SelectItemBox");
	status.showPosX = kItemBoxUiPosX;
	status.showPosY = kItemBoxUiPosY[0];

	EntryGraph(name, status);

	//スカイドームの設定
	MV1SetPosition(m_skyDomeHandle,VGet(0,0,0));

}

int MenuUi::Update()
{
	//スカイドームを回転させる
	MV1SetRotationXYZ(m_skyDomeHandle, VGet(0, MV1GetRotationXYZ(m_skyDomeHandle).y + kCameraRotaSpeed, 0));

	//選択している選択肢の裏側
	auto& selectItemBox = GetGraphRef("SelectItemBox");

	//選択している選択肢を右に動かす
	if (m_stringUi[static_cast<int>(m_selectItem)].showPosX < kStringUiPosX + kShowUiMoveRange)
	{
		//右に動かす
		m_stringUi[static_cast<int>(m_selectItem)].showPosX += kShowUiMoveSpeed;
		//行きすぎたら戻す
		m_stringUi[static_cast<int>(m_selectItem)].showPosX = min(m_stringUi[static_cast<int>(m_selectItem)].showPosX, kStringUiPosX + kShowUiMoveRange);

		//裏側のボックスも一緒に動かす

		//青いボックス
		std::string name = "ItemBox";

		name += std::to_string(static_cast<int>(m_selectItem));

		auto& itemBox = GetGraphRef(name);
		itemBox.showPosX += kShowUiMoveSpeed;
		itemBox.showPosX = min(itemBox.showPosX, kItemBoxUiPosX + kShowUiMoveRange);

		//黄色いボックス
		selectItemBox.showPosX += kShowUiMoveSpeed;
		selectItemBox.showPosX = min(selectItemBox.showPosX, kItemBoxUiPosX + kShowUiMoveRange);
		selectItemBox.showPosY = kItemBoxUiPosY[static_cast<int>(m_selectItem)];
	}

	//選択している項目のアルファ値を変更する
	selectItemBox.alpha += kSelectItemBoxBlendSpeed;

	selectItemBox.alpha = min(selectItemBox.alpha, 255);

	//選択されてない選択肢を左に動かす
	for (int i = 0; i < static_cast<int>(SelectItem::kItemNum); i++)
	{
		//選択されているかどうかを確認
		if (static_cast<int>(m_selectItem) != i)
		{
			//左に動かす
			m_stringUi[i].showPosX -= kShowUiMoveSpeed;
			//動かしすぎないように補正
			m_stringUi[i].showPosX = max(m_stringUi[i].showPosX, kStringUiPosX);

			//後ろのボックスも一緒に動かす
			std::string name = "ItemBox";

			name += std::to_string(static_cast<int>(i));

			auto& itemBox = GetGraphRef(name);
			itemBox.showPosX -= kShowUiMoveSpeed;
			itemBox.showPosX = max(itemBox.showPosX, kItemBoxUiPosX);
		}
	}

	//上下入力で選択している項目を変化させる
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	if (input->IsTrigger("Up"))
	{
		auto lastItem = m_selectItem;

		//上にカーソルを動かす
		m_selectItem = static_cast<SelectItem>(static_cast<int>(m_selectItem) - 1);

		//項目がない部分にいかないように
		m_selectItem = static_cast<SelectItem>(max(static_cast<int>(m_selectItem), 0));

		//カーソルが動いていたら
		if (m_selectItem != lastItem)
		{
			//動かしたタイミングで選択している項目の後ろのボックスのX座標を初期位置に戻す
			selectItemBox.showPosX = kItemBoxUiPosX;
			//アルファ値も0にする
			selectItemBox.alpha = 0;
		}
	}
	else if (input->IsTrigger("Down"))
	{
		auto lastItem = m_selectItem;

		//下にカーソルを動かす
		m_selectItem = static_cast<SelectItem>(static_cast<int>(m_selectItem) + 1);

		//項目がない部分に行かないように
		m_selectItem = static_cast<SelectItem>(min(static_cast<int>(m_selectItem), static_cast<int>(SelectItem::kItemNum) - 1));

		//カーソルが動いていたら
		if (m_selectItem != lastItem)
		{
			//動かしたタイミングで選択している項目の後ろのボックスのX座標を初期位置に戻す
			selectItemBox.showPosX = kItemBoxUiPosX;
			//アルファ値も0にする
			selectItemBox.alpha = 0;
		}
	}

	if (input->IsTrigger("A"))
	{
		SoundManager::GetInstance().OncePlaySound("Ok");

		return static_cast<int>(m_selectItem);
	}

	return -1;
}

void MenuUi::DrawItem()
{
	//画像をすべて表示する
	for (auto item : m_graphMap)
	{
		auto data = item.second;

		//アルファ値が設定されていたらブレンドモードを変更する
		if (data.alpha < 255) SetDrawBlendMode(DX_BLENDMODE_ALPHA, data.alpha);

		DrawRotaGraph(data.showPosX, data.showPosY, 1.0, 0.0, data.handle, true);

		if (data.alpha < 255) SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	//選択肢を表示する
	for (auto item : m_stringUi)
	{
		//描画処理
		DrawStringToHandle(item.showPosX, item.showPosY, item.showString.c_str(), GetColor(255, 255, 255),m_selectItemFontHandle,GetColor(0,0,0));
	}

}

void MenuUi::DrawModel()
{
	MV1DrawModel(m_skyDomeHandle);
}

void MenuUi::EntryGraph(std::string name, GraphUiStatus status)
{
	m_graphMap[name] = status;
}

MenuUi::GraphUiStatus& MenuUi::GetGraphRef(std::string name)
{
	MenuUi::GraphUiStatus& ans = m_graphMap[name];

	return ans;
}