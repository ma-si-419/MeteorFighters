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
		"オプション",
		"ゲームを終了する",
	};

	//それぞれの項目で表示する画像の名前
	const std::string kUiGraphName[static_cast<int>(MenuUi::SelectItem::kItemNum)] =
	{
		"Battle",
		"Tutorial",
		"Option",
		"GameEnd"
	};

	//フォントの名前
	const TCHAR* kFontName = "GN-キルゴUかなNB";

	//選択肢のフォントサイズ
	constexpr int kSelectItemFontSize = 64;

	//オプションのフォントサイズ
	constexpr int kOptionFontSize = 40;

	//オプションの右下の文字のフォントサイズ
	constexpr int kOptionButtonStringFontSize = 28;

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

	//オプションのシークバーの位置
	constexpr int kOptionBarPosX = Game::kWindowWidth / 2;
	constexpr int kOptionBarPosY = 230;

	//シークバーの間隔
	constexpr int kOptionBarInterval = 165;

	//オプションの文字列を表示する座標
	constexpr int kOptionStringPosX = 360;
	constexpr int kOptionStringPosY = 135;

	//オプションの文字列
	const std::string kOptionString[static_cast<int>(SoundManager::OptionSoundKind::kOptionSoundKindNum)] =
	{
		"MASTER",
		"SE",
		"BGM",
		"VOICE"
	};

	//オプションのシークバーの長さ
	constexpr int kOptionBarLength = 764;

	//シークバーの端の画像をどのくらいずらして表示するか
	constexpr int kOptionBarMarkShiftLange = 63;

	//オプションのシークバーを動かす速度
	constexpr float kOptionBarMoveSpeed = 0.01f;

	//シークバーの最大拡大率
	constexpr float kThumbMaxScale = 1.4f;

	//シークバーの最小拡大率
	constexpr float kThumbMinScale = 1.3f;

	//シークバーの拡大率の変化速度
	constexpr float kThumbScaleSpeed = 0.2f;

	//オプションのボタンを表示する座標
	constexpr int kOptionButtonPosX = 1146;
	constexpr int kOptionButtonPosY = 782;

	//オプションのボタンの説明を表示する座標
	constexpr int kOptionButtonStringPosX = 1150;
	constexpr int kOptionButtonStringPosY = 770;

	//オプションのフェードインの速さ
	constexpr int kOptionFadeInSpeed = 20;
}


MenuUi::MenuUi() :
	m_selectItemFontHandle(-1),
	m_selectItem(SelectItem::kSelect),
	m_selectItemMoveTime(0),
	m_skyDomeHandle(-1),
	m_lastSelectItem(SelectItem::kSelect),
	m_rightGraphSrcNum(0),
	m_rightGraphChangeTime(0),
	m_optionSelectItem(0),
	m_isMoveSeekBar(false),
	m_thumbSize(0),
	m_optionFontHandle(-1),
	m_optionAlpha(0),
	m_isEndOption(false)
{
	m_selectItemFontHandle = CreateFontToHandle(kFontName, kSelectItemFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 3);
	m_optionFontHandle = CreateFontToHandle(kFontName, kOptionFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 3);
	m_optionButtonFontHandle = CreateFontToHandle(kFontName, kOptionButtonStringFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 3);


	m_skyDomeHandle = MV1LoadModel("data/model/Dome.mv1");

	m_updateFunc = &MenuUi::UpdateSelect;
	m_drawFunc = &MenuUi::DrawNone;
}

MenuUi::~MenuUi()
{
	DeleteFontToHandle(m_selectItemFontHandle);
	DeleteFontToHandle(m_optionFontHandle);
	DeleteFontToHandle(m_optionButtonFontHandle);
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
		m_selectStringUi[i].showPosX = kStringUiPosX;
		m_selectStringUi[i].showPosY = kStringUiPosY[i];
		m_selectStringUi[i].showString = kUiString[i];
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
	//更新処理
	UpdateCommon();

	//選択肢の更新
	int selectItem = (this->*m_updateFunc)();

	return selectItem;
}

void MenuUi::DrawItem()
{
	//画像をすべて表示する
	for (auto item : m_selectGraphMap)
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
	for (auto item : m_selectStringUi)
	{
		//描画処理
		DrawStringToHandle(item.showPosX, item.showPosY, item.showString.c_str(), GetColor(255, 255, 255), m_selectItemFontHandle, GetColor(0, 0, 0));
	}

	//オプションやゲーム終了の画像を表示する
	(this->*m_drawFunc)();
}

void MenuUi::DrawModel()
{
	MV1DrawModel(m_skyDomeHandle);
}

void MenuUi::EntryGraph(std::string name, GraphUiStatus status)
{
	m_selectGraphMap[name] = status;
}

MenuUi::GraphUiStatus& MenuUi::GetGraphRef(std::string name)
{
	MenuUi::GraphUiStatus& ans = m_selectGraphMap[name];

	return ans;
}

void MenuUi::UpdateCommon()
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
	if (m_selectStringUi[static_cast<int>(m_selectItem)].showPosX < kStringUiPosX + kShowUiMoveRange)
	{
		//右に動かす
		m_selectStringUi[static_cast<int>(m_selectItem)].showPosX += kShowUiMoveSpeed;
		//行きすぎたら戻す
		m_selectStringUi[static_cast<int>(m_selectItem)].showPosX = min(m_selectStringUi[static_cast<int>(m_selectItem)].showPosX, kStringUiPosX + kShowUiMoveRange);

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
			m_selectStringUi[i].showPosX -= kShowUiMoveSpeed;
			//動かしすぎないように補正
			m_selectStringUi[i].showPosX = max(m_selectStringUi[i].showPosX, kStringUiPosX);

			//後ろのボックスも一緒に動かす
			std::string name = "ItemBox";

			name += std::to_string(static_cast<int>(i));

			auto& itemBox = GetGraphRef(name);
			itemBox.showPosX -= kShowUiMoveSpeed;
			itemBox.showPosX = max(itemBox.showPosX, kItemBoxUiPosX);
		}
	}
}

int MenuUi::UpdateSelect()
{
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

	//選択している選択肢の裏側
	auto& selectItemBox = GetGraphRef("SelectItemBox");

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

	int ans = -1;

	//決定ボタンを押したら
	if (input->IsTrigger("A"))
	{
		SoundManager::GetInstance().PlayOnceSound("Ok");

		//オプションを選択していたら
		if (m_selectItem == SelectItem::kOption)
		{
			//オプションの更新関数を設定する
			m_updateFunc = &MenuUi::UpdateOption;
			//オプションの描画関数を設定する
			m_drawFunc = &MenuUi::DrawOption;

			//オプションのアルファ値を初期化する
			m_optionAlpha = 0;
			//シークバーを動かすかどうかを初期化する
			m_isMoveSeekBar = false;

			m_isEndOption = false;

			//音声の音量を取得する
			m_seVolume = SoundManager::GetInstance().GetVolume(SoundManager::OptionSoundKind::kSe);
			m_bgmVolume = SoundManager::GetInstance().GetVolume(SoundManager::OptionSoundKind::kBgm);
			m_voiceVolume = SoundManager::GetInstance().GetVolume(SoundManager::OptionSoundKind::kVoice);
			m_masterVolume = SoundManager::GetInstance().GetVolume(SoundManager::OptionSoundKind::kMaster);
		}
		//ゲーム終了を選択していたら
		else if (m_selectItem == SelectItem::kEndGame)
		{
			//ゲーム終了の更新関数を設定する
			m_updateFunc = &MenuUi::UpdateEndGame;
			//ゲーム終了の描画関数を設定する
			m_drawFunc = &MenuUi::DrawEndGame;
		}
		else
		{
			//選択している項目を返す
			ans = static_cast<int>(m_selectItem);
		}
	}

	return ans;

}

int MenuUi::UpdateOption()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);
	auto& soundManager = SoundManager::GetInstance();

	//オプションの更新処理

	//アルファ値をあげていく
	if (!m_isEndOption)
	{
		m_optionAlpha += kOptionFadeInSpeed;
		if (m_optionAlpha >= 255)
		{
			m_optionAlpha = 255;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		m_optionAlpha -= kOptionFadeInSpeed;

		if (m_optionAlpha < 0)
		{
			//セレクトモードに戻る
			m_updateFunc = &MenuUi::UpdateSelect;
			m_drawFunc = &MenuUi::DrawNone;
		}
		return -1;
	}

	//シークバーを動かすかどうかがfalseの時
	if (!m_isMoveSeekBar)
	{
		//上入力がされていたら
		if (MyEngine::Input::GetInstance().GetInputData(0)->IsTrigger("Up"))
		{
			m_optionSelectItem--;

			//最小値よりも小さくなったら
			if (m_optionSelectItem < 0)
			{
				//最大値に戻す
				m_optionSelectItem = static_cast<int>(SoundManager::OptionSoundKind::kOptionSoundKindNum) - 1;
			}
		}
		//下入力がされていたら
		else if (MyEngine::Input::GetInstance().GetInputData(0)->IsTrigger("Down"))
		{
			m_optionSelectItem++;
			//最大値を超えたら
			if (m_optionSelectItem >= static_cast<int>(SoundManager::OptionSoundKind::kOptionSoundKindNum))
			{
				//最小値に戻す
				m_optionSelectItem = 0;
			}
		}

		//Bボタンが押されたら
		if (input->IsTrigger("B"))
		{
			m_isEndOption = true;

			//キャンセルサウンドを鳴らす
			SoundManager::GetInstance().PlayOnceSound("Cancel");
		}
	}
	//シークバーを動かす時
	else
	{
		//左入力がされていたら
		if (input->IsPress("Left"))
		{
			//選択しているシークバーの音量を下げる
			if (m_optionSelectItem == static_cast<int>(SoundManager::OptionSoundKind::kSe))
			{
				m_seVolume -= kOptionBarMoveSpeed;
			}
			else if (m_optionSelectItem == static_cast<int>(SoundManager::OptionSoundKind::kBgm))
			{
				m_bgmVolume -= kOptionBarMoveSpeed;
			}
			else if (m_optionSelectItem == static_cast<int>(SoundManager::OptionSoundKind::kVoice))
			{
				m_voiceVolume -= kOptionBarMoveSpeed;
			}
			else if (m_optionSelectItem == static_cast<int>(SoundManager::OptionSoundKind::kMaster))
			{
				m_masterVolume -= kOptionBarMoveSpeed;
			}
		}
		//右入力がされていたら
		else if (input->IsPress("Right"))
		{
			//選択しているシークバーの音量を上げる
			if (m_optionSelectItem == static_cast<int>(SoundManager::OptionSoundKind::kSe))
			{
				m_seVolume += kOptionBarMoveSpeed;
			}
			else if (m_optionSelectItem == static_cast<int>(SoundManager::OptionSoundKind::kBgm))
			{
				m_bgmVolume += kOptionBarMoveSpeed;
			}
			else if (m_optionSelectItem == static_cast<int>(SoundManager::OptionSoundKind::kVoice))
			{
				m_voiceVolume += kOptionBarMoveSpeed;
			}
			else if (m_optionSelectItem == static_cast<int>(SoundManager::OptionSoundKind::kMaster))
			{
				m_masterVolume += kOptionBarMoveSpeed;
			}
		}

		//音量のクランプ
		m_seVolume = fmax(m_seVolume, 0.0f);
		m_seVolume = fmin(m_seVolume, 1.0f);

		m_bgmVolume = fmax(m_bgmVolume, 0.0f);
		m_bgmVolume = fmin(m_bgmVolume, 1.0f);

		m_voiceVolume = fmax(m_voiceVolume, 0.0f);
		m_voiceVolume = fmin(m_voiceVolume, 1.0f);

		m_masterVolume = fmax(m_masterVolume, 0.0f);
		m_masterVolume = fmin(m_masterVolume, 1.0f);


		//SE音量が変更されているかチェック
		if (soundManager.GetVolume(SoundManager::OptionSoundKind::kSe) != m_seVolume)
		{
			//音量を変更する
			soundManager.SetVolume(SoundManager::OptionSoundKind::kSe, m_seVolume);
		}

		//BGM音量が変更されているかチェック
		if (soundManager.GetVolume(SoundManager::OptionSoundKind::kBgm) != m_bgmVolume)
		{
			//音量を変更する
			soundManager.SetVolume(SoundManager::OptionSoundKind::kBgm, m_bgmVolume);
		}

		//VOICE音量が変更されているかチェック
		if (soundManager.GetVolume(SoundManager::OptionSoundKind::kVoice) != m_voiceVolume)
		{
			//音量を変更する
			soundManager.SetVolume(SoundManager::OptionSoundKind::kVoice, m_voiceVolume);
		}

		//MASTER音量が変更されているかチェック
		if (soundManager.GetVolume(SoundManager::OptionSoundKind::kMaster) != m_masterVolume)
		{
			//音量を変更する
			soundManager.SetVolume(SoundManager::OptionSoundKind::kMaster, m_masterVolume);
		}

		//Bボタンが押されたら
		if (MyEngine::Input::GetInstance().GetInputData(0)->IsTrigger("B"))
		{
			//シークバーを動かすかどうかをfalseにする
			m_isMoveSeekBar = false;

			//キャンセルサウンドを鳴らす
			SoundManager::GetInstance().PlayOnceSound("Cancel");
		}
	}

	//Aボタンが押されたら
	if (MyEngine::Input::GetInstance().GetInputData(0)->IsTrigger("A"))
	{
		m_isMoveSeekBar = true;

		//決定サウンドを鳴らす
		SoundManager::GetInstance().PlayOnceSound("Ok");
	}



	//基本的に-1を返す
	return -1;
}

int MenuUi::UpdateEndGame()
{
	//ゲームを終了する
	return static_cast<int>(SelectItem::kEndGame);
}

void MenuUi::DrawOption()
{
	//オプションの描画処理
	int optionMenu = GraphManager::GetInstance().GetHandle("OptionMenu");

	int bar = GraphManager::GetInstance().GetHandle("Bar");

	int thumb = GraphManager::GetInstance().GetHandle("Thumb");

	int thumbSelect = GraphManager::GetInstance().GetHandle("ThumbSelect");

	int minSound = GraphManager::GetInstance().GetHandle("MinSound");

	int maxSound = GraphManager::GetInstance().GetHandle("MaxSound");

	//つまみのサイズを設定する
	m_thumbSize += kThumbScaleSpeed;
	float thumbScale = kThumbMinScale + (kThumbMaxScale - kThumbMinScale) * sin(m_thumbSize);

	//ブレンドモードの設定
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_optionAlpha);

	//オプションの画像を描画する
	DrawRotaGraph(Game::kWindowWidth / 2, Game::kWindowHeight / 2, 1.0, 0.0, optionMenu, true);

	//シークバーを調整できる数分表示する
	for (int i = 0; i < static_cast<int>(SoundManager::OptionSoundKind::kOptionSoundKindNum); i++)
	{
		//シークバーの位置を設定する
		int posX = kOptionBarPosX;
		int posY = kOptionBarPosY + kOptionBarInterval * i;

		//シークバーを描画する
		DrawRotaGraph(posX, posY, 1.0, 0.0, bar, true);

		//シークバーの端の画像を描画する
		DrawRotaGraph(posX - kOptionBarLength / 2 - kOptionBarMarkShiftLange, posY, 1.0, 0.0, minSound, true);

		//シークバーの端の画像を描画する
		DrawRotaGraph(posX + kOptionBarLength / 2 + kOptionBarMarkShiftLange, posY, 1.0, 0.0, maxSound, true);

		//つまみの位置を設定する
		posX -= kOptionBarLength / 2;

		if (i == static_cast<int>(SoundManager::OptionSoundKind::kSe))
		{
			posX += kOptionBarLength * m_seVolume;
		}
		else if (i == static_cast<int>(SoundManager::OptionSoundKind::kBgm))
		{
			posX += kOptionBarLength * m_bgmVolume;
		}
		else if (i == static_cast<int>(SoundManager::OptionSoundKind::kVoice))
		{
			posX += kOptionBarLength * m_voiceVolume;
		}
		else if (i == static_cast<int>(SoundManager::OptionSoundKind::kMaster))
		{
			posX += kOptionBarLength * m_masterVolume;
		}

		if (i == m_optionSelectItem)
		{
			if (m_isMoveSeekBar)
			{
				//つまみの位置を描画する
				DrawRotaGraph(posX, posY, thumbScale, 0.0, thumb, true);
			}
			else
			{

				//つまみの位置を描画する
				DrawRotaGraph(posX, posY, 1.0, 0.0, thumb, true);

				DrawRotaGraph(posX, posY, thumbScale, 0.0, thumbSelect, true);
			}
		}
		else
		{
			//つまみの位置を描画する
			DrawRotaGraph(posX, posY, 1.0, 0.0, thumb, true);
		}
	}

	//オプションの文字列を表示する
	for (int i = 0; i < static_cast<int>(SoundManager::OptionSoundKind::kOptionSoundKindNum); i++)
	{
		DrawStringToHandle(kOptionStringPosX, kOptionStringPosY + kOptionBarInterval * i, kOptionString[i].c_str(), GetColor(255, 255, 255), m_optionFontHandle, GetColor(0, 0, 0));
	}


	if (m_isMoveSeekBar)
	{
		//オプションのボタンを表示する
		int button = GraphManager::GetInstance().GetHandle("B");
		DrawRotaGraph(kOptionButtonPosX, kOptionButtonPosY, 1.0, 0.0, button, true);

		DrawStringToHandle(kOptionButtonStringPosX, kOptionButtonStringPosY, " :戻る", GetColor(255, 255, 255), m_optionButtonFontHandle, GetColor(0, 0, 0));

	}
	else
	{
		//オプションのボタンを表示する
		int button = GraphManager::GetInstance().GetHandle("A");
		DrawRotaGraph(kOptionButtonPosX, kOptionButtonPosY, 1.0, 0.0, button, true);

		DrawStringToHandle(kOptionButtonStringPosX, kOptionButtonStringPosY, " :決定", GetColor(255, 255, 255), m_optionButtonFontHandle, GetColor(0, 0, 0));
	}

	//ブレンドモードを戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}

void MenuUi::DrawEndGame()
{

}
