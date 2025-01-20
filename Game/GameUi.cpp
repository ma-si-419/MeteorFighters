#include "GameUi.h"
#include "GraphManager.h"
#include "DxLib.h"
#include <cmath>
#include "Game.h"
#include "GameSceneConstant.h"
#include "Input.h"

namespace
{
	//一本のHPバーの体力量
	constexpr int kMaxHp = 10000;

	//Hpバーの座標
	constexpr int kHpBarPosX[2] = { 350,Game::kWindowWidth - 350 };
	constexpr int kHpBarPosY = 80;

	//Hpバーの後ろのボックスの大きさ
	constexpr int kBackBoxHalfWidth = 275;
	constexpr int kBackBoxHalfHeight = 40;

	//Hpバーの後ろのボックスの色
	const int kBackBoxColor = GetColor(36, 36, 36);

	//Hpバーの右側のボックス座標
	const int kBigHpBarStartPosX[2] = { kHpBarPosX[0] - 5 ,kHpBarPosX[1] + 5 };
	constexpr int kBigHpBarPosY = kHpBarPosY - 30;

	//Hpバーの左側のボックス座標
	constexpr int kSmallHpBarPosY = kHpBarPosY + 3;

	//大きいHpバーの大きさ
	constexpr int kBigHpBoxHeight = 75;

	//小さいHpバーの縦の大きさ
	constexpr int kSmallHpBoxHeight = 40;

	//Hpバーの視点
	const int kHpBarStartPosX[2] = { kHpBarPosX[0] - 263 ,kHpBarPosX[1] + 263 };
	//Hpバーの終点
	constexpr int kHpBarEndPosX[2] = { kHpBarPosX[0] + 258,kHpBarPosX[1] - 258 };

	//Hpバーの数
	constexpr int kHpBarMaxNum = 5;

	//Hpバーを揺らす大きさ
	constexpr int kHpBarShakeScale = 10;

	//Hpバーの色の種類
	const unsigned int kHpBarColor[kHpBarMaxNum] =
	{
		GetColor(255,255,24),//1本
		GetColor(0,255,136),//2本
		GetColor(0,128,68),//3本
		GetColor(51,170,255),//4本
		GetColor(0,85,255),//5本
	};

	//Hpの残り本数を表すボックスの座標
	constexpr int kRightHpBarNumBoxPosX[kHpBarMaxNum] =
	{
		-999,//0個の時は表示しないのでマイナスの値を入れておく
		88,
		121,
		154,
		188
	};

	//Hpの残り本数を表すボックスの座標
	const int kLeftHpBarNumBoxPosX[kHpBarMaxNum] =
	{
		-999,//0個の時は表示しないのでマイナスの値を入れておく
		Game::kWindowWidth - 88,
		Game::kWindowWidth - 121,
		Game::kWindowWidth - 154,
		Game::kWindowWidth - 188
	};

	constexpr int kHpBarNumBoxPosY = 47;

	//残り本数を表すボックスの大きさ
	constexpr int kHpBarNumBoxWidth = 18;
	constexpr int kHpBarNumBoxHeight = 30;

	//赤いゲージが無くなるまでの時間
	constexpr int kRedBarSubStartTime = 60;

	//赤いゲージの色
	const int kRedBarColor = GetColor(255, 12, 12);

	//赤いゲージを減らす速さ
	constexpr float kRedBarSubSpeed = 250;

	//Mpバーの色
	const COLOR_U8 kMpBarColor = GetColorU8(255, 255, 0, 255);

	//消費した分を表すMpバーの色
	const COLOR_U8 kSubMpBarColor = GetColorU8(255, 0, 0, 255);

	//消費したMpを減らしていく速度
	constexpr float kRedMpBarSubSpeed = 300.0f;

	//Mpバーの画像を表示する座標
	constexpr int kMpBarGraphPosX[2] = { 350,Game::kWindowWidth - 350 };
	constexpr int kMpBarGraphPosY = 150;

	//Mpバーの開始座標
	constexpr int kMpBarStartPosX[2] = { 75,Game::kWindowWidth - 75 };
	constexpr int kMpBarUpPosY = 140;
	constexpr int kMpBarUnderPosY = 160;

	//Mpバーの長さ
	constexpr int kMpBarMaxLength = 365;

	//Mpバーの上と下のX座標のずれ
	constexpr int kMpBarGapX = 58;

	//Mpバーの下に表示する灰色のボックスの大きさ
	constexpr int kMpBarBackBoxHalfWidth = 280;
	constexpr int kMpBarBackBoxHalfHeight = 30;

	//連打するボタンを表示する座標
	constexpr int kBashButtonPosX = Game::kWindowWidth / 2;
	constexpr int kBashButtonPosY = 650;

	//連打するボタンの画像の大きさ
	constexpr int kBashButtonGraphWidth = 96;
	constexpr int kBashButtonGraphHeight = 84;

	//連打するボタンを切り替える時間
	constexpr int kBashButtonChangeTime = 10;
}

GameUi::GameUi() :
	m_lastHp(),
	m_lastHpBarNum(),
	m_onHitDamageHp(),
	m_hitDamageTime(),
	m_onSubMp(),
	m_bashButtonChangeTime(0)
{
}

void GameUi::RetryInit()
{
	//値をすべて初期化する
	for (auto& num : m_lastHp)			 num = 0.0f;
	for (auto& num : m_lastHpBarNum)	 num = 0.0f;
	for (auto& num : m_onHitDamageHp)	 num = 0.0f;
	for (auto& num : m_onSubMp)			 num = 0.0f;
	for (auto& num : m_hitDamageTime)	 num = 0;

	for (auto& pos : m_shakePos) pos = MyEngine::Vector2(0, 0);
}

void GameUi::DrawHpBar(float hp, bool isLeft)
{
	int barGraphHandle = GraphManager::GetInstance().GetHandle("HpBar");
	int barNumGraphHandle = GraphManager::GetInstance().GetHandle("HpBarNum");

	if (isLeft)
	{
		//この関数を初めて読んだタイミングならば
		if (m_lastHp[0] == 0)
		{
			//初期化する
			m_lastHp[0] = hp;
			m_lastHpBarNum[0] = (hp / kMaxHp) - 1;
		}

		//ダメージを受けていた時は
		if (m_lastHp[0] > hp)
		{
			//Hpバーを揺らす
			m_shakePos[0].x = static_cast<float>(GetRand(kHpBarShakeScale));
			m_shakePos[0].y = static_cast<float>(GetRand(kHpBarShakeScale));
			if (m_hitDamageTime[0] < 0)
			{
				m_onHitDamageHp[0] = m_lastHp[0];
			}
			//赤いバーが減るまでの時間を設定
			m_hitDamageTime[0] = kRedBarSubStartTime;
		}
		else
		{
			m_shakePos[0].x = 0;
			m_shakePos[0].y = 0;
		}

		//後ろの黒いボックス描画
		DrawBox(static_cast<int>(kHpBarPosX[0] - kBackBoxHalfWidth + m_shakePos[0].x), static_cast<int>(kHpBarPosY - kBackBoxHalfHeight + m_shakePos[0].y),//始点
			static_cast<int>(kHpBarPosX[0] + kBackBoxHalfWidth + m_shakePos[0].x), static_cast<int>(kHpBarPosY + kBackBoxHalfHeight + m_shakePos[0].y),//終点
			kBackBoxColor, true);

		//現在の体力を1万で区切る
		int nowHp = static_cast<int>(hp) % kMaxHp;
		int hpBarNum = static_cast<int>(hp / kMaxHp);

		//体力がちょうど10000で割り切れた場合の処理
		if (hp > 0 && nowHp == 0)
		{
			nowHp = kMaxHp;
			hpBarNum--;
		}

		//Hpバーの数
		hpBarNum = std::min<int>(hpBarNum, kHpBarMaxNum - 1);

		//Hpバーの数が減っていたら
		if (m_lastHpBarNum[0] > hpBarNum)
		{
			//赤いバーをリセットする
			m_onHitDamageHp[0] = m_lastHpBarNum[0] * kMaxHp;

			m_hitDamageTime[0] = kRedBarSubStartTime;
		}


		//Hpバーの数を保存しておく
		m_lastHpBarNum[0] = static_cast<float>(hpBarNum);

		//Hpバーの色
		int barColor = kHpBarColor[hpBarNum];

		//体力の割合
		float hpRate = nowHp / static_cast<float>(kMaxHp);

		//0以下にならないように
		hpRate = std::fmax(hpRate, 0.0f);

		//Hpバーの終点
		int hpBarEndPos = static_cast<int>((kHpBarEndPosX[0] - kHpBarStartPosX[0]) * hpRate) + kHpBarStartPosX[0];


		//Hpバーが2本以上あれば
		if (hpBarNum > 0)
		{
			//一つ下のバーの殻を使用する
			int backBarColor = kHpBarColor[hpBarNum - 1];

			//一つ下のHpバーを表示しておく
			DrawBox(static_cast<int>(kHpBarStartPosX[0] + m_shakePos[0].x), static_cast<int>(kSmallHpBarPosY + m_shakePos[0].y),//始点
				static_cast<int>(kHpBarEndPosX[0] + m_shakePos[0].x), static_cast<int>(kSmallHpBarPosY + kSmallHpBoxHeight + m_shakePos[0].y),//終点
				backBarColor, true);

			DrawBox(static_cast<int>(kBigHpBarStartPosX[0] + m_shakePos[0].x), static_cast<int>(kBigHpBarPosY + m_shakePos[0].y),//始点
				static_cast<int>(kHpBarEndPosX[0] + m_shakePos[0].x), static_cast<int>(kBigHpBarPosY + kBigHpBoxHeight + m_shakePos[0].y),//終点
				backBarColor, true);
		}


		//赤いゲージを減らすタイミングを設定
		m_hitDamageTime[0]--;

		int lastHp = static_cast<int>(m_onHitDamageHp[0]) % kMaxHp;

		//体力がちょうど10000で割り切れた場合の処理
		if (hp > 0 && lastHp == 0)
		{
			lastHp = kMaxHp;
		}

		float redBarRate = lastHp / static_cast<float>(kMaxHp);

		redBarRate = std::fmax(redBarRate, 0.0f);

		int damageBarEndPos = static_cast<int>((kHpBarEndPosX[0] - kHpBarStartPosX[0]) * redBarRate) + kHpBarStartPosX[0];

		//小さいほうのゲージの表示
		DrawBox(static_cast<int>(hpBarEndPos + m_shakePos[0].x), static_cast<int>(kSmallHpBarPosY + m_shakePos[0].y),//始点
			static_cast<int>(damageBarEndPos + m_shakePos[0].x), static_cast<int>(kSmallHpBarPosY + kSmallHpBoxHeight + m_shakePos[0].y),
			kRedBarColor, true);

		//終点が始点よりも左側にあれば
		if (damageBarEndPos > kBigHpBarStartPosX[0])
		{
			//大きいほうのゲージの表示
			DrawBox(static_cast<int>(kBigHpBarStartPosX[0] + m_shakePos[0].x), static_cast<int>(kBigHpBarPosY + m_shakePos[0].y),//始点
				static_cast<int>(damageBarEndPos + m_shakePos[0].x), static_cast<int>(kBigHpBarPosY + kBigHpBoxHeight + m_shakePos[0].y),//終点
				kRedBarColor, true);
		}

		//ダメージを受けてから一定時間たったら
		if (m_hitDamageTime[0] < 0)
		{
			m_onHitDamageHp[0] -= kRedBarSubSpeed;
			m_onHitDamageHp[0] = std::fmax(m_onHitDamageHp[0], hp);
		}

		//小さいほうの体力の表示
		DrawBox(static_cast<int>(kHpBarStartPosX[0] + m_shakePos[0].x), static_cast<int>(kSmallHpBarPosY + m_shakePos[0].y),//始点
			static_cast<int>(hpBarEndPos + m_shakePos[0].x), static_cast<int>(kSmallHpBarPosY + kSmallHpBoxHeight + m_shakePos[0].y),//終点
			barColor, true);

		//終点が始点よりも左側にあれば
		if (hpBarEndPos > kBigHpBarStartPosX[0])
		{
			//大きいほうの体力の表示
			DrawBox(static_cast<int>(kBigHpBarStartPosX[0] + m_shakePos[0].x), static_cast<int>(kBigHpBarPosY + m_shakePos[0].y),//始点
				static_cast<int>(hpBarEndPos + m_shakePos[0].x), static_cast<int>(kBigHpBarPosY + kBigHpBoxHeight + m_shakePos[0].y),//終点
				barColor, true);
		}

		//残っている体力の本数を表示する
		for (int i = 0; i <= hpBarNum; i++)
		{
			DrawBox(static_cast<int>(kRightHpBarNumBoxPosX[i] + m_shakePos[0].x), static_cast<int>(kHpBarNumBoxPosY + m_shakePos[0].y),
				static_cast<int>(kRightHpBarNumBoxPosX[i] + kHpBarNumBoxWidth + m_shakePos[0].x), static_cast<int>(kHpBarNumBoxPosY + kHpBarNumBoxHeight + m_shakePos[0].y),//終点
				GetColor(32, 255, 32), true);
		}


		//枠の表示
		DrawRotaGraph(static_cast<int>(kHpBarPosX[0] + m_shakePos[0].x), static_cast<int>(kHpBarPosY + m_shakePos[0].y), 1.0, 0.0, barGraphHandle, true, false);

		m_lastHp[0] = hp;
	}
	else
	{
		//この関数を初めて読んだタイミングならば
		if (m_lastHp[1] == 0)
		{
			//初期化する
			m_lastHp[1] = hp;
			m_lastHpBarNum[1] = (hp / kMaxHp) - 1;
		}

		//ダメージを受けていた時は
		if (m_lastHp[1] > hp)
		{
			//Hpバーを揺らす
			m_shakePos[1].x = static_cast<float>(GetRand(kHpBarShakeScale));
			m_shakePos[1].y = static_cast<float>(GetRand(kHpBarShakeScale));
			if (m_hitDamageTime[1] < 0)
			{
				m_onHitDamageHp[1] = m_lastHp[1];
			}
			//赤いバーが減るまでの時間を設定
			m_hitDamageTime[1] = kRedBarSubStartTime;
		}
		else
		{
			m_shakePos[1].x = 0;
			m_shakePos[1].y = 0;
		}

		//後ろの黒いボックス描画
		DrawBox(static_cast<int>(kHpBarPosX[1] + kBackBoxHalfWidth + m_shakePos[1].x), static_cast<int>(kHpBarPosY - kBackBoxHalfHeight + m_shakePos[1].y),//始点
			static_cast<int>(kHpBarPosX[1] - kBackBoxHalfWidth + m_shakePos[1].x), static_cast<int>(kHpBarPosY + kBackBoxHalfHeight + m_shakePos[1].y),//終点
			kBackBoxColor, true);

		//現在の体力を1万で区切る
		int nowHp = static_cast<int>(hp) % kMaxHp;
		int hpBarNum = static_cast<int>(hp / kMaxHp);

		//体力がちょうど10000で割り切れた場合の処理
		if (hp > 0 && nowHp == 0)
		{
			nowHp = kMaxHp;
			hpBarNum--;
		}

		//Hpバーの数
		hpBarNum = std::min<int>(hpBarNum, kHpBarMaxNum - 1);

		//Hpバーの数が減っていたら
		if (m_lastHpBarNum[1] > hpBarNum)
		{
			//赤いバーをリセットする
			m_onHitDamageHp[1] = m_lastHpBarNum[1] * kMaxHp;

			m_hitDamageTime[1] = kRedBarSubStartTime;
		}


		//Hpバーの数を保存しておく
		m_lastHpBarNum[1] = static_cast<float>(hpBarNum);

		//Hpバーの色
		int barColor = kHpBarColor[hpBarNum];

		//体力の割合
		float hpRate = nowHp / static_cast<float>(kMaxHp);

		//0以下にならないように
		hpRate = std::fmax(hpRate, 0.0f);

		//Hpバーの終点
		int hpBarEndPos = kHpBarStartPosX[1] - static_cast<int>((kHpBarStartPosX[1] - kHpBarEndPosX[1]) * hpRate);


		//Hpバーが2本以上あれば
		if (hpBarNum > 0)
		{
			//一つ下のバーの殻を使用する
			int backBarColor = kHpBarColor[hpBarNum - 1];

			//一つ下のHpバーを表示しておく
			DrawBox(static_cast<int>(kHpBarStartPosX[1] + m_shakePos[1].x), static_cast<int>(kSmallHpBarPosY + m_shakePos[1].y),//始点
				static_cast<int>(kHpBarEndPosX[1] + m_shakePos[1].x), static_cast<int>(kSmallHpBarPosY + kSmallHpBoxHeight + m_shakePos[1].y),//終点
				backBarColor, true);

			DrawBox(static_cast<int>(kBigHpBarStartPosX[1] + m_shakePos[1].x), static_cast<int>(kBigHpBarPosY + m_shakePos[1].y),//始点
				static_cast<int>(kHpBarEndPosX[1] + m_shakePos[1].x), static_cast<int>(kBigHpBarPosY + kBigHpBoxHeight + m_shakePos[1].y),//終点
				backBarColor, true);
		}


		//赤いゲージを減らすタイミングを設定
		m_hitDamageTime[1]--;

		int lastHp = static_cast<int>(m_onHitDamageHp[1]) % kMaxHp;

		//体力がちょうど10000で割り切れた場合の処理
		if (hp > 0 && lastHp == 0)
		{
			lastHp = kMaxHp;
		}

		float redBarRate = lastHp / static_cast<float>(kMaxHp);

		redBarRate = std::fmax(redBarRate, 0.0f);

		int damageBarEndPos = kHpBarStartPosX[1] - static_cast<int>((kHpBarStartPosX[1] - kHpBarEndPosX[1]) * redBarRate);

		//小さいほうのゲージの表示
		DrawBox(static_cast<int>(hpBarEndPos + m_shakePos[1].x), static_cast<int>(kSmallHpBarPosY + m_shakePos[1].y),//始点
			static_cast<int>(damageBarEndPos + m_shakePos[1].x), static_cast<int>(kSmallHpBarPosY + kSmallHpBoxHeight + m_shakePos[1].y),//終点
			kRedBarColor, true);

		//終点が始点よりも左側にあれば
		if (damageBarEndPos < kBigHpBarStartPosX[1])
		{
			//大きいほうのゲージの表示
			DrawBox(static_cast<int>(kBigHpBarStartPosX[1] + m_shakePos[1].x), static_cast<int>(kBigHpBarPosY + m_shakePos[1].y),//始点
				static_cast<int>(damageBarEndPos + m_shakePos[1].x), static_cast<int>(kBigHpBarPosY + kBigHpBoxHeight + m_shakePos[1].y),//終点
				kRedBarColor, true);
		}

		//ダメージを受けてから一定時間たったら
		if (m_hitDamageTime[1] < 0)
		{
			m_onHitDamageHp[1] -= kRedBarSubSpeed;
			m_onHitDamageHp[1] = std::fmax(m_onHitDamageHp[1], hp);
		}

		//小さいほうの体力の表示
		DrawBox(static_cast<int>(kHpBarStartPosX[1] + m_shakePos[1].x), static_cast<int>(kSmallHpBarPosY + m_shakePos[1].y),//始点
			static_cast<int>(hpBarEndPos + m_shakePos[1].x), static_cast<int>(kSmallHpBarPosY + kSmallHpBoxHeight + m_shakePos[1].y),//終点
			barColor, true);

		//終点が始点よりも右側にあれば
		if (hpBarEndPos < kBigHpBarStartPosX[1])
		{
			//大きいほうの体力の表示
			DrawBox(static_cast<int>(kBigHpBarStartPosX[1] + m_shakePos[1].x), static_cast<int>(kBigHpBarPosY + m_shakePos[1].y),//始点
				static_cast<int>(hpBarEndPos + m_shakePos[1].x), static_cast<int>(kBigHpBarPosY + kBigHpBoxHeight + m_shakePos[1].y),//終点
				barColor, true);
		}

		//残っている体力の本数を表示する
		for (int i = 0; i <= hpBarNum; i++)
		{
			DrawBox(static_cast<int>(kLeftHpBarNumBoxPosX[i] + m_shakePos[1].x), static_cast<int>(kHpBarNumBoxPosY + m_shakePos[1].y),//始点
				static_cast<int>(kLeftHpBarNumBoxPosX[i] - kHpBarNumBoxWidth + m_shakePos[1].x), static_cast<int>(kHpBarNumBoxPosY + kHpBarNumBoxHeight + m_shakePos[1].y),//終点
				GetColor(32, 255, 32), true);
		}


		//枠の表示
		DrawRotaGraph(static_cast<int>(kHpBarPosX[1] + m_shakePos[1].x), static_cast<int>(kHpBarPosY + m_shakePos[1].y), 1.0, 0.0, barGraphHandle, true, true);

		m_lastHp[1] = hp;
	}
}

void GameUi::DrawMpBar(float mp, bool isLeft)
{
	int barGraphHandle = GraphManager::GetInstance().GetHandle("MpBar");

	if (isLeft)
	{
		//mp量が増えていたら
		if (mp >= m_onSubMp[0])
		{
			//初期化する
			m_onSubMp[0] = mp;
		}
		//mp量が減っていたら
		else
		{
			m_onSubMp[0] -= kRedMpBarSubSpeed;
			//下がりすぎないようにクランプ
			m_onSubMp[0] = std::fmax(m_onSubMp[0], mp);
		}

		//Mpバーの長さ
		float subMpBarLength = kMpBarMaxLength * (m_onSubMp[0] / GameSceneConstant::kMaxMp);

		VERTEX2D vertex[6];

		//赤いゲージの座標(減った分の気力量を表示)

		//左上のポリゴン
		vertex[0].pos = VGet(kMpBarStartPosX[0] + kMpBarGapX, kMpBarUpPosY, 0);
		vertex[0].dif = kSubMpBarColor;
		vertex[0].rhw = 1.0f;
		vertex[0].u = 0.0f;
		vertex[0].v = 0.0f;

		//右上
		vertex[1].pos = VGet(kMpBarStartPosX[0] + kMpBarGapX + subMpBarLength, kMpBarUpPosY, 0);
		vertex[1].dif = kSubMpBarColor;
		vertex[1].rhw = 1.0f;
		vertex[1].u = 0.0f;
		vertex[1].v = 0.0f;

		//左下
		vertex[2].pos = VGet(kMpBarStartPosX[0], kMpBarUnderPosY, 0);
		vertex[2].dif = kSubMpBarColor;
		vertex[2].rhw = 1.0f;
		vertex[2].u = 0.0f;
		vertex[2].v = 0.0f;


		//右下
		vertex[3].pos = VGet(kMpBarStartPosX[0] + subMpBarLength, kMpBarUnderPosY, 0);
		vertex[3].dif = kSubMpBarColor;
		vertex[3].rhw = 1.0f;
		vertex[3].u = 0.0f;
		vertex[3].v = 0.0f;

		//2個目の三角形の第2頂点は左下の頂点なのでコピー
		vertex[4] = vertex[2];

		//2ポリゴン目の第3頂点は右上の頂点なのでコピー
		vertex[5] = vertex[1];

		//Mpバーの下の灰色のボックス表示
		DrawBox(kMpBarGraphPosX[0] - kMpBarBackBoxHalfWidth, kMpBarUpPosY, kMpBarGraphPosX[0] + kMpBarBackBoxHalfWidth, kMpBarUnderPosY, GetColor(192, 192, 192), true);

		//Mpバーが減った量を表す赤いバーを表示
		DrawPolygon2D(vertex, 2, DX_NONE_GRAPH, true);

		for (auto& poly : vertex)
		{
			poly.dif = kMpBarColor;
		}

		//通常の座標を設定

		float mpBarLength = kMpBarMaxLength * (mp / GameSceneConstant::kMaxMp);

		//左上のポリゴン
		vertex[0].pos = VGet(kMpBarStartPosX[0] + kMpBarGapX, kMpBarUpPosY, 0);

		//右上
		vertex[1].pos = VGet(kMpBarStartPosX[0] + kMpBarGapX + mpBarLength, kMpBarUpPosY, 0);

		//左下
		vertex[2].pos = VGet(kMpBarStartPosX[0], kMpBarUnderPosY, 0);

		//右下
		vertex[3].pos = VGet(kMpBarStartPosX[0] + mpBarLength, kMpBarUnderPosY, 0);


		//2個目の三角形の第2頂点は左下の頂点なのでコピー
		vertex[4].pos = vertex[2].pos;

		//2ポリゴン目の第3頂点は右上の頂点なのでコピー
		vertex[5].pos = vertex[1].pos;

		//Mpバーを表示
		DrawPolygon2D(vertex, 2, DX_NONE_GRAPH, true);

		DrawRotaGraph(kMpBarGraphPosX[0], kMpBarGraphPosY, 1.0, 0.0, barGraphHandle, true);
	}
	else
	{
		//mp量が増えていたら
		if (mp >= m_onSubMp[1])
		{
			//初期化する
			m_onSubMp[1] = mp;
		}
		//mp量が減っていたら
		else
		{
			m_onSubMp[1] -= kRedMpBarSubSpeed;
			//下がりすぎないようにクランプ
			m_onSubMp[1] = std::fmax(m_onSubMp[1], mp);
		}

		//Mpバーの長さ
		float subMpBarLength = kMpBarMaxLength * (m_onSubMp[1] / GameSceneConstant::kMaxMp);

		VERTEX2D vertex[6];

		//赤いゲージの座標(減った分の気力量を表示)

		//右上のポリゴン
		vertex[0].pos = VGet(kMpBarStartPosX[1] - kMpBarGapX, kMpBarUpPosY, 0);
		vertex[0].dif = kSubMpBarColor;
		vertex[0].rhw = 1.0f;
		vertex[0].u = 0.0f;
		vertex[0].v = 0.0f;

		//左上
		vertex[1].pos = VGet(kMpBarStartPosX[1] - kMpBarGapX - subMpBarLength, kMpBarUpPosY, 0);
		vertex[1].dif = kSubMpBarColor;
		vertex[1].rhw = 1.0f;
		vertex[1].u = 0.0f;
		vertex[1].v = 0.0f;

		//左下
		vertex[2].pos = VGet(kMpBarStartPosX[1], kMpBarUnderPosY, 0);
		vertex[2].dif = kSubMpBarColor;
		vertex[2].rhw = 1.0f;
		vertex[2].u = 0.0f;
		vertex[2].v = 0.0f;


		//右下
		vertex[3].pos = VGet(kMpBarStartPosX[1] - subMpBarLength, kMpBarUnderPosY, 0);
		vertex[3].dif = kSubMpBarColor;
		vertex[3].rhw = 1.0f;
		vertex[3].u = 0.0f;
		vertex[3].v = 0.0f;

		//2個目の三角形の第2頂点は左下の頂点なのでコピー
		vertex[4] = vertex[2];

		//2ポリゴン目の第3頂点は右上の頂点なのでコピー
		vertex[5] = vertex[1];

		//Mpバーの下の灰色のボックス表示
		DrawBox(kMpBarGraphPosX[1] - kMpBarBackBoxHalfWidth, kMpBarUpPosY, kMpBarGraphPosX[1] + kMpBarBackBoxHalfWidth, kMpBarUnderPosY, GetColor(192, 192, 192), true);

		//Mpバーが減った量を表す赤いバーを表示
		DrawPolygon2D(vertex, 2, DX_NONE_GRAPH, true);

		for (auto& poly : vertex)
		{
			poly.dif = kMpBarColor;
		}

		//通常の座標を設定

		float mpBarLength = kMpBarMaxLength * (mp / GameSceneConstant::kMaxMp);

		//左上のポリゴン
		vertex[0].pos = VGet(kMpBarStartPosX[1] - kMpBarGapX, kMpBarUpPosY, 0);

		//右上
		vertex[1].pos = VGet(kMpBarStartPosX[1] - kMpBarGapX - mpBarLength, kMpBarUpPosY, 0);

		//左下
		vertex[2].pos = VGet(kMpBarStartPosX[1], kMpBarUnderPosY, 0);

		//右下
		vertex[3].pos = VGet(kMpBarStartPosX[1] - mpBarLength, kMpBarUnderPosY, 0);


		//2個目の三角形の第2頂点は左下の頂点なのでコピー
		vertex[4].pos = vertex[2].pos;

		//2ポリゴン目の第3頂点は右上の頂点なのでコピー
		vertex[5].pos = vertex[1].pos;

		//Mpバーを表示
		DrawPolygon2D(vertex, 2, DX_NONE_GRAPH, true);

		DrawRotaGraph(kMpBarGraphPosX[1], kMpBarGraphPosY, 1.0, 0.0, barGraphHandle, true, true);
	}

}

void GameUi::DrawFade(int color, int alpha)
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, color, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameUi::DrawBashButton(std::string button)
{
	auto& graphManager = GraphManager::GetInstance();

	int buttonHandle = graphManager.GetHandle("Bash" + button);

	//ボタンを切り替える時間を図っておく
	m_bashButtonChangeTime++;

	int cutPos = kBashButtonGraphWidth;

	if (m_bashButtonChangeTime % static_cast<int>(kBashButtonChangeTime * 2) < kBashButtonChangeTime)
	{
		cutPos = 0;
	}

	//ボタンの後ろの背景を表示する
	DrawRotaGraph(kBashButtonPosX, kBashButtonPosY,1.0,0.0,graphManager.GetHandle("BashBack"),true);

	//ボタンを描画する
	DrawRectRotaGraph(kBashButtonPosX, kBashButtonPosY,cutPos,0,kBashButtonGraphWidth,kBashButtonGraphHeight,
	1.0,0.0,buttonHandle,true);
}
