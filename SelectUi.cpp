#include "SelectUi.h"
#include "SelectManager.h"
#include "GraphManager.h"
#include "DxLib.h"

namespace
{
	//カメラの設定
	constexpr float kCameraNear = 0.1f;
	constexpr float kCameraFar = 100.0f;


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

	const std::string kCharacterNames[static_cast<int>(SelectManager::CharacterNumber::kCharacterNum)] =
	{
		"Character0",
		"Random",
		"Character1"
	};
}

SelectUi::SelectUi() :
	m_lastPlayerNumber(0),
	m_lastEnemyNumber(0),
	m_playerNumber(0),
	m_enemyNumber(0),
	m_skyDomeHandle(-1),
	m_iconFrameScalling(0.0f)
{

}

SelectUi::~SelectUi()
{
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

	//選択しているアイコンのフレーム
	GraphData iconFrame;
	iconFrame.posX = kIconPosX;
	iconFrame.posY = kIconPosY;
	iconFrame.handle = graphManager.GetHandle("IconFrame");
	m_drawGraphs[GraphName::kIconFrame] = iconFrame;

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

}

void SelectUi::Update()
{
	auto& graphManager = GraphManager::GetInstance();

	//前のフレーム選んでいた画像を徐々にフェードアウトさせる
	m_drawGraphs[GraphName::kLastPlayer].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kLastPlayer].alpha -= kLastCharacterGraphFadeSpeed;
	m_drawGraphs[GraphName::kLastEnemy].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kLastEnemy].alpha -= kLastCharacterGraphFadeSpeed;

	//現在選んでいる画像をフェードインさせる
	m_drawGraphs[GraphName::kPlayer].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kPlayer].posX = max(m_drawGraphs[GraphName::kPlayer].posX, kPlayerCharacterPosX);

	m_drawGraphs[GraphName::kPlayer].alpha += kLastCharacterGraphFadeSpeed;

	//2P側
	m_drawGraphs[GraphName::kEnemy].posX = kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kEnemy].posX = max(m_drawGraphs[GraphName::kEnemy].posX, kEnemyCharacterPosX);

	m_drawGraphs[GraphName::kEnemy].alpha += kLastCharacterGraphFadeSpeed;

	//選択しているアイコンのフレームを拡縮させる
	m_iconFrameScalling += kIconFrameScallingSpeed;
	m_drawGraphs[GraphName::kIconFrame].scale = (sinf(m_iconFrameScalling) * kIconFrameAddScaleMax) + 1.0f;

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

		DrawRotaGraph(graph.posX, graph.posY, graph.scale, 0.0, graph.handle, true);

		if (graph.alpha < 255)
		{
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}

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

void SelectUi::SetIconFrame(int number)
{
	m_drawGraphs[GraphName::kIconFrame].posX = kIconPosX + (kIconDistance * number);
}

void SelectUi::SetDrawIconFrame(bool flag)
{
	if (flag)
	{
		m_drawGraphs[GraphName::kIconFrame].alpha = 255;
	}
	else
	{
		m_drawGraphs[GraphName::kIconFrame].alpha = 0;
	}
}
