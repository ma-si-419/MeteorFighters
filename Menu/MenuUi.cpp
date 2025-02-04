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

	//上下入力でリピート入力を受け付ける時間
	constexpr int kRepeatInputTime = 15;

	//リピート入力の間隔
	constexpr int kRepeatInterval = 5;

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

	//それぞれの項目で表示する画像の名前
	const std::string kUiGraphName[static_cast<int>(MenuUi::SelectItem::kItemNum)] =
	{
		"Battle",
		"Tutorial",
		"SelectItemBox",
		"GameEnd"
	};

	//フォントの名前
	const TCHAR* kFontName = "GN-キルゴUかなNB";

	//右側に表示する画像の大きさ
	constexpr int kRightGraphSize = 680;

	//右側に表示する画像の位置
	constexpr int kRightGraphPosX = 1150;
	constexpr int kRightGraphPosY = Game::kWindowHeight / 2;

	//右側に表示する画像がはいってくるときどのくらいずらして表示するか
	constexpr int kRightGraphMoveRange = 150;

	//右側に表示する画像の移動速度
	constexpr int kRightGraphMoveSpeed = 10;

	//右側に表示する画像のフェードインアウトの速度
	constexpr int kRightGraphBlendSpeed = 20;

	//右側の画像を切り替える時間
	constexpr int kRightGraphChangeTime = 15;

	//右側の画像の切り替える数
	constexpr int kRightGraphChangeNum = 1;
}


MenuUi::MenuUi() :
	m_selectItemFontHandle(-1),
	m_selectItem(SelectItem::kBattle),
	m_selectItemMoveTime(0),
	m_skyDomeHandle(-1),
	m_lastSelectItem(SelectItem::kBattle),
	m_rightGraphSrcNum(0),
	m_rightGraphChangeTime(0)
{
	m_selectItemFontHandle = CreateFontToHandle(kFontName, 64, 0, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 3);
	m_skyDomeHandle = MV1LoadModel("data/model/Dome.mv1");
}

MenuUi::~MenuUi()
{
	DeleteFontToHandle(m_selectItemFontHandle);
}

void MenuUi::Init()
{
	//カメラの設定
	SetCameraPositionAndTarget_UpVecY(VGet(0, 0, 0), VGet(0, 0, 1));

	//NearFarの設定
	SetCameraNearFar(kCameraNear, kCameraFar);


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
	{
		std::string selectItemBox = "SelectItemBox";

		GraphUiStatus status;

		status.handle = GraphManager::GetInstance().GetHandle("SelectItemBox");
		status.showPosX = kItemBoxUiPosX;
		status.showPosY = kItemBoxUiPosY[0];

		EntryGraph(selectItemBox, status);
	}


	//ひとつ前に選択していた項目の右側に表示していた画像を設定する
	{
		std::string name = "LastRightGraph";
		GraphUiStatus status;
		status.handle = GraphManager::GetInstance().GetHandle(kUiGraphName[static_cast<int>(m_lastSelectItem)]);
		status.showPosX = kRightGraphPosX;
		status.showPosY = kRightGraphPosY;
		status.graphSizeX = kRightGraphSize;
		status.graphSizeY = kRightGraphSize;
		status.srcPosX = 0;
		status.alpha = 0;
		EntryGraph(name, status);
	}

	//右側に表示する画像を設定する
	{
		std::string name = "RightGraph";
		GraphUiStatus status;
		status.handle = GraphManager::GetInstance().GetHandle(kUiGraphName[static_cast<int>(m_selectItem)]);
		status.showPosX = kRightGraphPosX;
		status.showPosY = kRightGraphPosY;
		status.graphSizeX = kRightGraphSize;
		status.graphSizeY = kRightGraphSize;
		status.srcPosX = 0;
		EntryGraph(name, status);
	}

	//スカイドームの設定
	MV1SetPosition(m_skyDomeHandle, VGet(0, 0, 0));

}

int MenuUi::Update()
{
	//スカイドームを回転させる
	MV1SetRotationXYZ(m_skyDomeHandle, VGet(0, MV1GetRotationXYZ(m_skyDomeHandle).y + kCameraRotaSpeed, 0));

	//右側の画像を切り替える時間
	m_rightGraphChangeTime++;

	//右側に表示している画像のアルファ値の設定
	auto& lastRightGraph = GetGraphRef("LastRightGraph");
	auto& rightGraph = GetGraphRef("RightGraph");

	//選択している項目の右側に表示していた画像
	if (lastRightGraph.alpha > 0)
	{
		lastRightGraph.alpha -= kRightGraphBlendSpeed;
		lastRightGraph.alpha = max(lastRightGraph.alpha, 0);
	}

	//選択している項目の右側に表示する画像
	if (rightGraph.alpha < 255)
	{
		rightGraph.alpha += kRightGraphBlendSpeed;
		rightGraph.alpha = min(rightGraph.alpha, 255);
	}

	//前選択していた項目が選択している項目よりも小さい場合
	if (m_lastSelectItem < m_selectItem)
	{
		//右側に表示していた画像を上に動かす
		lastRightGraph.showPosY -= kRightGraphMoveSpeed;
				 
		//右側に表示する画像を上に動かす
		rightGraph.showPosY -= kRightGraphMoveSpeed;
		//動かしすぎないように補正
		rightGraph.showPosY = max(rightGraph.showPosY, kRightGraphPosY);
	}
	//前選択していた項目が選択している項目よりも大きい場合
	else if (m_lastSelectItem > m_selectItem)
	{
		//右側に表示していた画像を下に動かす
		lastRightGraph.showPosY += kRightGraphMoveSpeed;
		//右側に表示する画像を下に動かす
		rightGraph.showPosY += kRightGraphMoveSpeed;
		//動かしすぎないように補正
		rightGraph.showPosY = min(rightGraph.showPosY, kRightGraphPosY);
	}

	//右側の画像を切り替える時間になったら
	if (m_rightGraphChangeTime > kRightGraphChangeTime)
	{
		m_rightGraphChangeTime = 0;

		m_rightGraphSrcNum++;

		//一定値を超えたら
		if (m_rightGraphSrcNum > kRightGraphChangeNum)
		{
			m_rightGraphSrcNum = 0;
		}

		//右側に表示していた画像の切り替え
		lastRightGraph.srcPosX = lastRightGraph.graphSizeX * m_rightGraphSrcNum;
		//右側に表示する画像の切り替え
		rightGraph.srcPosX = rightGraph.graphSizeX * m_rightGraphSrcNum;
	}

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

	//前のフレームの情報
	auto lastItem = m_selectItem;

	//動かしやすいようにint型にキャスト
	auto selectItem = static_cast<int>(m_selectItem);

	//トリガー入力
	if (input->IsTrigger("Up"))
	{

		//上にカーソルを動かす
		selectItem--;

		//一番上にカーソルがあった場合
		if (selectItem < 0)
		{
			//一番下に移動する
			selectItem = static_cast<int>(SelectItem::kItemEnd);
		}
	}
	else if (input->IsTrigger("Down"))
	{
		//下にカーソルを動かす
		selectItem++;

		if (selectItem > static_cast<int>(SelectItem::kItemEnd))
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

	//クランプ
	selectItem = max(selectItem, 0);
	selectItem = min(selectItem, static_cast<int>(SelectItem::kItemNum) - 1);

	m_selectItem = static_cast<SelectItem>(selectItem);

	//カーソルが動いていたら
	if (m_selectItem != lastItem)
	{
		//動かしたタイミングで選択している項目の後ろのボックスのX座標を初期位置に戻す
		selectItemBox.showPosX = kItemBoxUiPosX;
		//アルファ値も0にする
		selectItemBox.alpha = 0;

		//動いて何フレーム立ったかリセット
		m_selectItemMoveTime = 0;

		//ひとつ前選択していた項目を記憶する
		m_lastSelectItem = lastItem;

		//選択していた項目の右側に表示していた画像を設定する
		{
			std::string name = "LastRightGraph";
			auto& lastRightGraph = GetGraphRef(name);
			lastRightGraph.handle = GraphManager::GetInstance().GetHandle(kUiGraphName[static_cast<int>(m_lastSelectItem)]);
			lastRightGraph.alpha = 255;
			lastRightGraph.showPosX = kRightGraphPosX;
			lastRightGraph.showPosY = kRightGraphPosY;
		}

		//右側に表示する画像を設定する
		{
			std::string name = "RightGraph";
			auto& rightGraph = GetGraphRef(name);
			rightGraph.handle = GraphManager::GetInstance().GetHandle(kUiGraphName[static_cast<int>(m_selectItem)]);
			rightGraph.alpha = 0;
			rightGraph.showPosX = kRightGraphPosX;

			//下に動かしていた場合
			if (static_cast<int>(m_selectItem) > static_cast<int>(m_lastSelectItem))
			{
				rightGraph.showPosY = kRightGraphPosY + kRightGraphMoveRange;
			}
			//上に動かしていた場合
			else
			{
				rightGraph.showPosY = kRightGraphPosY - kRightGraphMoveRange;
			}
		}

		//選択音を鳴らす
		SoundManager::GetInstance().PlayOnceSound("Select");
	}
	else
	{
		m_selectItemMoveTime++;
	}

	if (input->IsTrigger("A"))
	{
		SoundManager::GetInstance().PlayOnceSound("Ok");

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

		//画像のサイズが指定されていたら
		if (data.graphSizeX != 0 && data.graphSizeY != 0)
		{
			//切り取って描画
			DrawRectRotaGraph(data.showPosX, data.showPosY, data.srcPosX, 0, data.graphSizeX, data.graphSizeY, 1.0, 0.0, data.handle, true);
		}
		else
		{
			//そのまま描画
			DrawRotaGraph(data.showPosX, data.showPosY, 1.0, 0.0, data.handle, true);
		}
		if (data.alpha < 255) SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	//選択肢を表示する
	for (auto item : m_stringUi)
	{
		//描画処理
		DrawStringToHandle(item.showPosX, item.showPosY, item.showString.c_str(), GetColor(255, 255, 255), m_selectItemFontHandle, GetColor(0, 0, 0));
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