#include "SelectUi.h"
#include "SelectManager.h"
#include "GraphManager.h"
#include "DxLib.h"
#include "Game.h"
#include "EnemyInput.h"

namespace
{
	//フォントの名前
	const TCHAR* kFontName = "GN-キルゴUかなNB";

	//フォントのサイズ
	constexpr int kLevelFontSize = 48;
	//曲名を表示するフォントのサイズ
	constexpr int kMusicNameFontSize = 36;
	//曲名を表示する文字列の色(黄色)
	const int kMusicNameColor = GetColor(255, 255, 32);	 
	//曲名のフェードインの速度
	constexpr int kMusicNameFadeInSpeed = 13;
	//曲名の移動速度
	constexpr int kMusicNameMoveSpeed = 10;

	//音符を表示する座標
	constexpr int kMusicNotePosX = 1105;
	constexpr int kMusicNotePosY = 850;
	
	//曲名を表示する座標
	constexpr int kMusicNamePosX = 1152;
	constexpr int kMusicNamePosY = 835;

	//曲名の初期座標
	constexpr int kMusicNameInitPosX = 1300;

	//カメラの設定
	constexpr float kCameraNear = 0.1f;
	constexpr float kCameraFar = 100.0f;

	//スカイドームの回転速度
	constexpr float kSkyDomeRotaSpeed = 0.002f;

	//キャラクターの画像設定
	constexpr int kPlayerCharacterPosX = 320;
	constexpr int kEnemyCharacterPosX = 1280;
	constexpr int kCharacterPosY = 450;
	constexpr double kCharacterScale = 1.0;

	//キャラクターの画像が横にずれる速度
	constexpr int kLastCharacterGraphMoveSpeed = 10;

	//どのくらい横からキャラクターが入ってくるか
	constexpr int kCharacterPopDistance = 120;

	//キャラクターの画像が消えていく速度
	constexpr int kLastCharacterGraphFadeSpeed = 20;

	//アイコンの画像設定
	constexpr int kIconPosX = 630;
	constexpr int kIconDistance = 170;
	constexpr int kIconPosY = 500;

	//選択しているアイコンを示す画像設定
	constexpr double kIconFrameAddScaleMax = 0.1;
	constexpr float kIconFrameScallingSpeed = 0.13f;

	//HeadSetの画像設定
	constexpr int kHeadSetPosX = 800;
	constexpr int kHeadSetPosY = 200;

	//VSの画像設定
	constexpr int kVsPosX = 800;
	constexpr int kVsPosY = 750;

	//RBの画像設定
	constexpr int kRbPosX = Game::kWindowWidth - 130;

	//LBの画像設定
	constexpr int kLbPosX = 130;

	//難易度選択のボタンの座標
	constexpr int kLevelSelectButtonPosY = 75;

	//難易度選択の矢印を揺らす大きさ
	constexpr int kLevelSelectArrowShakeScale = 18;

	//難易度選択の矢印を揺らす速さ
	constexpr float kLevelSelectArrowShakeSpeed = 0.25f;

	//難易度を表示する座標
	constexpr int kRightLevelPosX = kRbPosX - 46;
	constexpr int kLeftLevelPosX = kLbPosX + 55;
	constexpr int kLevelPosY = kLevelSelectButtonPosY - 20;

	//難易度の文字列
	const std::string kLevelString[static_cast<int>(EnemyInput::AiLevel::kLevelNum)] =
	{
		"EASY",
		"NORMAL",
		"HARD"
	};

	//難易度ごとの文字列の色
	const unsigned int kLevelColor[static_cast<int>(EnemyInput::AiLevel::kLevelNum)] =
	{
		GetColor(255, 255, 255),
		GetColor(32,32, 32),
		GetColor(255, 0, 0)
	};


	const std::string kCharacterNames[static_cast<int>(SelectManager::CharacterNumber::kCharacterNum)] =
	{
		"Character0",
		"Random",
		"Character1"
	};
}

SelectUi::SelectUi() :
	m_lastPlayerNumber(0),
	m_lastEnemyNumber(static_cast<int>(SelectManager::CharacterNumber::kBlueHead)),
	m_playerNumber(0),
	m_enemyNumber(static_cast<int>(SelectManager::CharacterNumber::kBlueHead)),
	m_skyDomeHandle(-1),
	m_iconFrameScalling(0.0f),
	m_time(0.0f),
	m_level(0)
{
	//レベルフォントの読み込み
	m_levelFontHandle = CreateFontToHandle(kFontName, kLevelFontSize, 3, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 2);

	//曲名を表示するフォントの読み込み
	m_musicNameFontHandle = CreateFontToHandle(kFontName, kMusicNameFontSize, 3, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 2);
}

SelectUi::~SelectUi()
{
	DeleteFontToHandle(m_levelFontHandle);
	DeleteFontToHandle(m_musicNameFontHandle);
}

void SelectUi::Init()
{
	//スカイドームの設定
	m_skyDomeHandle = MV1LoadModel("data/model/Dome.mv1");

	//カメラの設定
	SetCameraPositionAndTarget_UpVecY(VGet(0, 0, 0), VGet(0, 0, 1));

	//NearFarの設定
	SetCameraNearFar(kCameraNear, kCameraFar);

	//スカイドームの設定
	MV1SetPosition(m_skyDomeHandle, VGet(0, 0, 0));

	//更新関数の設定
	m_updateFunc = &SelectUi::Update1P;

	//グラフマネージャー
	auto& graphManager = GraphManager::GetInstance();

	//プレイヤーの画像設定
	GraphData playerDefault;
	playerDefault.posX = kPlayerCharacterPosX;
	playerDefault.posY = kCharacterPosY;
	playerDefault.scale = kCharacterScale;
	playerDefault.alpha = 0;
	playerDefault.handle = graphManager.GetHandle(kCharacterNames[m_playerNumber]);
	m_drawGraphs[GraphName::kLastPlayer] = playerDefault;
	playerDefault.alpha = 255;
	m_drawGraphs[GraphName::kPlayer] = playerDefault;

	//エネミーの画像設定
	GraphData enemyDefault;
	enemyDefault.posX = kEnemyCharacterPosX;
	enemyDefault.posY = kCharacterPosY;
	enemyDefault.scale = kCharacterScale;
	enemyDefault.alpha = 0;
	enemyDefault.handle = graphManager.GetHandle(kCharacterNames[m_enemyNumber]);
	m_drawGraphs[GraphName::kLastEnemy] = enemyDefault;
	enemyDefault.alpha = 255;
	m_drawGraphs[GraphName::kEnemy] = enemyDefault;

	//アイコンの画像設定

	//一つ目のアイコン
	GraphData icon;
	icon.posX = kIconPosX;
	icon.posY = kIconPosY;
	icon.handle = graphManager.GetHandle("Icon0");
	m_drawGraphs[GraphName::kIcon0] = icon;

	//二つ目のアイコン
	icon.posX += kIconDistance;
	icon.handle = graphManager.GetHandle("IconRandom");
	m_drawGraphs[GraphName::kIcon1] = icon;

	//三つ目のアイコン
	icon.posX += kIconDistance;
	icon.handle = graphManager.GetHandle("Icon1");
	m_drawGraphs[GraphName::kIcon2] = icon;

	//選択している1Pのアイコンのフレーム
	GraphData iconFrame1P;
	iconFrame1P.posX = kIconPosX;
	iconFrame1P.posY = kIconPosY;
	iconFrame1P.handle = graphManager.GetHandle("IconFrame1P");
	m_drawGraphs[GraphName::kIconFrame1P] = iconFrame1P;

	//選択している2Pのアイコンのフレーム
	GraphData iconFrame2P;
	iconFrame2P.posX = kIconPosX;
	iconFrame2P.posY = kIconPosY;
	iconFrame2P.handle = graphManager.GetHandle("IconFrame2P");
	//アルファ値を0にして非表示にする
	iconFrame2P.alpha = 0;
	m_drawGraphs[GraphName::kIconFrame2P] = iconFrame2P;

	//どちらもが選択しているアイコンのフレームは非表示
	GraphData iconFrameBoth;
	iconFrameBoth.posX = kIconPosX;
	iconFrameBoth.posY = kIconPosY;
	iconFrameBoth.handle = graphManager.GetHandle("IconFrameBoth");
	//アルファ値を0にして非表示にする
	iconFrameBoth.alpha = 0;
	m_drawGraphs[GraphName::kIconFrameBoth] = iconFrameBoth;

	//HeadSetの画像設定
	GraphData headSet;
	headSet.posX = kHeadSetPosX;
	headSet.posY = kHeadSetPosY;
	headSet.handle = graphManager.GetHandle("HeadSet");
	m_drawGraphs[GraphName::kHeadSet] = headSet;

	//VSの画像設定
	GraphData vs;
	vs.posX = kVsPosX;
	vs.posY = kVsPosY;
	vs.handle = graphManager.GetHandle("Vs");
	m_drawGraphs[GraphName::kVs] = vs;

	//矢印の画像設定
	GraphData leftArrow;
	leftArrow.posX = kLbPosX;
	leftArrow.posY = kLevelSelectButtonPosY;
	leftArrow.handle = graphManager.GetHandle("Arrow");
	leftArrow.isReverseX = true;
	m_drawGraphs[GraphName::kLeftArrow] = leftArrow;

	GraphData rightArrow;
	rightArrow.posX = kRbPosX;
	rightArrow.posY = kLevelSelectButtonPosY;
	rightArrow.handle = graphManager.GetHandle("Arrow");
	m_drawGraphs[GraphName::kRightArrow] = rightArrow;

	//RBの画像設定
	GraphData rb;
	rb.posX = kRbPosX;
	rb.posY = kLevelSelectButtonPosY;
	rb.handle = graphManager.GetHandle("RB");
	m_drawGraphs[GraphName::kRB] = rb;

	//LBの画像設定
	GraphData lb;
	lb.posX = kLbPosX;
	lb.posY = kLevelSelectButtonPosY;
	lb.handle = graphManager.GetHandle("LB");
	m_drawGraphs[GraphName::kLB] = lb;

	//音符の画像設定
	GraphData musicNote;
	musicNote.posX = kMusicNotePosX;
	musicNote.posY = kMusicNotePosY;
	musicNote.handle = graphManager.GetHandle("MusicNote");
	m_drawGraphs[GraphName::kMusicNote] = musicNote;
}

void SelectUi::Update()
{
	(this->*m_updateFunc)();

	m_time += kLevelSelectArrowShakeSpeed;

	//矢印の画像を揺らす
	m_drawGraphs[GraphName::kLeftArrow].posX = kLbPosX - static_cast<int>(sinf(m_time) * kLevelSelectArrowShakeScale);
	m_drawGraphs[GraphName::kRightArrow].posX = kRbPosX + static_cast<int>(sinf(m_time) * kLevelSelectArrowShakeScale);

	//矢印の画像の座標をクランプ
	m_drawGraphs[GraphName::kLeftArrow].posX = min(m_drawGraphs[GraphName::kLeftArrow].posX, kLbPosX);
	m_drawGraphs[GraphName::kRightArrow].posX = max(m_drawGraphs[GraphName::kRightArrow].posX, kRbPosX);

	//スカイドームを回転させる
	MV1SetRotationXYZ(m_skyDomeHandle, VGet(0, MV1GetRotationXYZ(m_skyDomeHandle).y + kSkyDomeRotaSpeed, 0));
}

void SelectUi::Draw()
{
	//スカイドームの描画
	MV1DrawModel(m_skyDomeHandle);

	//画像をまとめて表示する
	for (auto& item : m_drawGraphs)
	{
		auto graph = item.second;

		if (graph.alpha < 255)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, graph.alpha);
		}

		DrawRotaGraph(graph.posX, graph.posY, graph.scale, 0.0, graph.handle, true, graph.isReverseX);

		if (graph.alpha < 255)
		{
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}

	//文字列を表示する
	{
		//右側の難易度
		MyEngine::Vector2 pos = MyEngine::Vector2(kRightLevelPosX, kLevelPosY);

		//一つ次のレベル
		int nextLevel = m_level + 1;

		if (nextLevel > static_cast<int>(EnemyInput::AiLevel::kHard))
		{
			nextLevel = static_cast<int>(EnemyInput::AiLevel::kEasy);
		}


		//文字列の座標を右揃えに設定
		pos.x -= GetFontSizeToHandle(m_levelFontHandle) * kLevelString[nextLevel].size();

		DrawStringToHandle(static_cast<int>(pos.x), static_cast<int>(pos.y), kLevelString[nextLevel].c_str(), kLevelColor[nextLevel], m_levelFontHandle);

		//左側の難易度
		pos = MyEngine::Vector2(kLeftLevelPosX, kLevelPosY);

		//一つ前のレベル
		int prevLevel = m_level - 1;

		if (prevLevel < static_cast<int>(EnemyInput::AiLevel::kEasy))
		{
			prevLevel = static_cast<int>(EnemyInput::AiLevel::kHard);
		}

		DrawStringToHandle(static_cast<int>(pos.x), static_cast<int>(pos.y), kLevelString[prevLevel].c_str(), kLevelColor[prevLevel], m_levelFontHandle);

		//曲名のアルファ値を増やす
		m_musicNameAlpha += kMusicNameFadeInSpeed;

		//曲名の座標を左にずらす
		m_musicNamePosX -= kMusicNameMoveSpeed;

		//曲名の座標をクランプ
		m_musicNamePosX = max(m_musicNamePosX, kMusicNamePosX);

		//ブレンドモードを設定
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, min(m_musicNameAlpha, 255));

		//曲名を表示する
		DrawStringToHandle(m_musicNamePosX, kMusicNamePosY, m_musicName.c_str(), kMusicNameColor, m_musicNameFontHandle);

		//ブレンドモードを元に戻す
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void SelectUi::SetNumber(int number, bool isPlayer)
{
	auto& graphManager = GraphManager::GetInstance();

	if (isPlayer)
	{
		//前の画像を表示する
		if (m_lastPlayerNumber != number)
		{
			m_drawGraphs[GraphName::kLastPlayer].alpha = 255;
			m_drawGraphs[GraphName::kLastPlayer].posX = kPlayerCharacterPosX;
			//ハンドルを更新する
			m_drawGraphs[GraphName::kLastPlayer].handle = graphManager.GetHandle(kCharacterNames[m_lastPlayerNumber]);

			//現在の画像も横にずらす
			m_drawGraphs[GraphName::kPlayer].posX = kPlayerCharacterPosX + kCharacterPopDistance;
			m_drawGraphs[GraphName::kPlayer].alpha = 0;

		}

		m_lastPlayerNumber = m_playerNumber;
		m_playerNumber = number;
		//ハンドルを更新する
		m_drawGraphs[GraphName::kPlayer].handle = graphManager.GetHandle(kCharacterNames[m_playerNumber]);
	}
	else
	{
		if (m_lastEnemyNumber != number)
		{
			m_drawGraphs[GraphName::kLastEnemy].alpha = 255;
			m_drawGraphs[GraphName::kLastEnemy].posX = kEnemyCharacterPosX;
			//ハンドルを更新する
			m_drawGraphs[GraphName::kLastEnemy].handle = graphManager.GetHandle(kCharacterNames[m_lastEnemyNumber]);

			//現在の画像も横にずらす
			m_drawGraphs[GraphName::kEnemy].posX = kPlayerCharacterPosX + kCharacterPopDistance;
			m_drawGraphs[GraphName::kEnemy].alpha = 0;


		}

		m_lastEnemyNumber = m_enemyNumber;
		m_enemyNumber = number;
		//ハンドルを更新する
		m_drawGraphs[GraphName::kEnemy].handle = graphManager.GetHandle(kCharacterNames[m_enemyNumber]);
	}
}

void SelectUi::SetIconFrame(int number, bool isPlayer)
{
	if (isPlayer)
	{
		m_drawGraphs[GraphName::kIconFrame1P].posX = kIconPosX + (kIconDistance * number);
	}
	else
	{
		m_drawGraphs[GraphName::kIconFrame2P].posX = kIconPosX + (kIconDistance * number);
	}
}

void SelectUi::ChangeSituation(UiSituation situation)
{
	switch (situation)
	{
	case UiSituation::kSelect1P:

		//1Pのアイコンのフレームを表示する
		m_drawGraphs[GraphName::kIconFrame1P].alpha = 255;

		//2Pのアイコンのフレームを非表示にする
		m_drawGraphs[GraphName::kIconFrame2P].alpha = 0;

		//どちらとも選択しているアイコンのフレームを非表示にする
		m_drawGraphs[GraphName::kIconFrameBoth].alpha = 0;

		m_updateFunc = &SelectUi::Update1P;
		break;
	case UiSituation::kSelect2P:

		//2Pのアイコンのフレームを表示する
		m_drawGraphs[GraphName::kIconFrame2P].alpha = 255;

		m_updateFunc = &SelectUi::Update2P;
		break;
	}
}

void SelectUi::SetSkyDomeHandle(int handle)
{
	MV1SetTextureGraphHandle(m_skyDomeHandle, 0, handle, FALSE);
}

void SelectUi::SetMusicName(std::string musicName)
{
	m_musicName = musicName;

	//曲名のアルファ値を0にする
	m_musicNameAlpha = 0;

	//曲名の座標を初期化
	m_musicNamePosX = kMusicNameInitPosX;
}

void SelectUi::Update1P()
{
	auto& graphManager = GraphManager::GetInstance();

	//前のフレーム選んでいた画像を徐々にフェードアウトさせる
	m_drawGraphs[GraphName::kLastPlayer].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kLastPlayer].alpha -= kLastCharacterGraphFadeSpeed;

	//現在選んでいる画像をフェードインさせる
	m_drawGraphs[GraphName::kPlayer].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kPlayer].posX = max(m_drawGraphs[GraphName::kPlayer].posX, kPlayerCharacterPosX);

	m_drawGraphs[GraphName::kPlayer].alpha += kLastCharacterGraphFadeSpeed;

	//前のフレーム選んでいた画像を徐々にフェードアウトさせる
	m_drawGraphs[GraphName::kLastEnemy].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kLastEnemy].alpha -= kLastCharacterGraphFadeSpeed;
	//現在選んでいる画像をフェードインさせる
	m_drawGraphs[GraphName::kEnemy].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kEnemy].posX = max(m_drawGraphs[GraphName::kEnemy].posX, kEnemyCharacterPosX);
	m_drawGraphs[GraphName::kEnemy].alpha += kLastCharacterGraphFadeSpeed;

	//選択しているアイコンのフレームを拡縮させる
	m_iconFrameScalling += kIconFrameScallingSpeed;
	m_drawGraphs[GraphName::kIconFrame1P].scale = (sinf(m_iconFrameScalling) * kIconFrameAddScaleMax) + 1.0f;
}

void SelectUi::Update2P()
{
	auto& graphManager = GraphManager::GetInstance();

	//前のフレーム選んでいた画像を徐々にフェードアウトさせる
	m_drawGraphs[GraphName::kLastPlayer].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kLastPlayer].alpha -= kLastCharacterGraphFadeSpeed;

	//現在選んでいる画像をフェードインさせる
	m_drawGraphs[GraphName::kPlayer].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kPlayer].posX = max(m_drawGraphs[GraphName::kPlayer].posX, kPlayerCharacterPosX);

	m_drawGraphs[GraphName::kPlayer].alpha += kLastCharacterGraphFadeSpeed;

	//前のフレーム選んでいた画像を徐々にフェードアウトさせる
	m_drawGraphs[GraphName::kLastEnemy].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kLastEnemy].alpha -= kLastCharacterGraphFadeSpeed;
	//現在選んでいる画像をフェードインさせる
	m_drawGraphs[GraphName::kEnemy].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kEnemy].posX = max(m_drawGraphs[GraphName::kEnemy].posX, kEnemyCharacterPosX);
	m_drawGraphs[GraphName::kEnemy].alpha += kLastCharacterGraphFadeSpeed;
	//選択しているアイコンのフレームを拡縮させる
	m_iconFrameScalling += kIconFrameScallingSpeed;
	m_drawGraphs[GraphName::kIconFrame2P].scale = (sinf(m_iconFrameScalling) * kIconFrameAddScaleMax) + 1.0f;

	//もし選択が被っていたら
	if (m_playerNumber == m_enemyNumber)
	{
		m_drawGraphs[GraphName::kIconFrameBoth].alpha = 255;
		m_drawGraphs[GraphName::kIconFrameBoth].posX = kIconPosX + (kIconDistance * m_playerNumber);
		m_drawGraphs[GraphName::kIconFrameBoth].scale = m_drawGraphs[GraphName::kIconFrame2P].scale;

		//1Pの表示を消す
		m_drawGraphs[GraphName::kIconFrame1P].alpha = 0;
	}
	else
	{
		m_drawGraphs[GraphName::kIconFrameBoth].alpha = 0;
		//1Pのフレームの拡縮を元に戻す
		m_drawGraphs[GraphName::kIconFrame1P].scale = 1.0f;
		m_drawGraphs[GraphName::kIconFrame1P].alpha = 255;
	}
}