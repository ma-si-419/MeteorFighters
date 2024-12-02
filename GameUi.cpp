#include "GameUi.h"
#include "GraphManager.h"
#include "DxLib.h"
#include <cmath>
#include "Game.h"
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

	//Mpのマックス量
	constexpr float kMpMax = 50000;

	//Mpバーの色
	const COLOR_U8 kMpBarColor = GetColorU8(255, 255, 0, 255);

	//Mpバーの画像を表示する座標
	constexpr int kMpBarGraphPosX[2] = { 350,Game::kWindowWidth - 350 };
	constexpr int kMpBarGraphPosY = 150;

	//Mpバーの開始座標
	constexpr int kMpBarStartPosX = 200;
	constexpr int kMpBarUpPosY = 200;
	constexpr int kMpBarUnderPosY = 300;

	//Mpバーの長さ
	constexpr int kMpBarMaxLength = 500;

	//Mpバーの上と下のX座標のずれ
	constexpr int kMpBarGapX = 50;


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

	//リザルトのロゴのフェードの速さ
	constexpr int kResultRogoFadeSpeed = 2;
}

GameUi::GameUi() :
	m_lastHp(),
	m_lastHpBarNum(),
	m_onHitDamageHp(),
	m_hitDamageTime(),
	m_selectItem(0),
	m_resultTime(0),
	m_resultLogoAlpha(255),
	m_resultLogoScale(kResultLogoDefaultScale),
	m_shakeTime(0)
{
}

void GameUi::DrawHpBar(float hp, bool isLeft)
{
	int barGraphHandle = GraphManager::GetInstance().GetHandle("HpBar");
	int barNumGraphHandle = GraphManager::GetInstance().GetHandle("HpBarNum");

	if (isLeft)
	{
		//この関数を初めて読んだタイミングならば
		if (m_lastHp[0] == -1)
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
		if (m_lastHp[1] == -1)
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
		//Mpバーの長さ
		float mpBarLength = kMpBarMaxLength * (mp / kMpMax);

		printfDx("%.5f\n",mp);

		VERTEX2D vertex[6];

		//左上のポリゴン
		vertex[0].pos = VGet(kMpBarStartPosX + kMpBarGapX, kMpBarUnderPosY, 0);
		vertex[0].dif = kMpBarColor;
		vertex[0].rhw = 1.0f;
		vertex[0].u = 0.0f;
		vertex[0].v = 0.0f;

		//右上
		vertex[1].pos = VGet(kMpBarStartPosX + kMpBarGapX + mpBarLength, kMpBarUpPosY, 0);
		vertex[1].dif = kMpBarColor;
		vertex[1].rhw = 1.0f;
		vertex[1].u = 0.0f;
		vertex[1].v = 0.0f;

		//左下
		vertex[2].pos = VGet(kMpBarStartPosX, kMpBarUnderPosY, 0);
		vertex[2].dif = kMpBarColor;
		vertex[2].rhw = 1.0f;
		vertex[2].u = 0.0f;
		vertex[2].v = 0.0f;


		//右下
		vertex[3].pos = VGet(kMpBarStartPosX + mpBarLength, kMpBarUnderPosY, 0);
		vertex[3].dif = kMpBarColor;
		vertex[3].rhw = 1.0f;
		vertex[3].u = 0.0f;
		vertex[3].v = 0.0f;

		//2個目の三角形の第2頂点は左下の頂点なのでコピー
		vertex[4] = vertex[2];

		//2ポリゴン目の第3頂点は右上の頂点なのでコピー
		vertex[5] = vertex[1];


		DrawPolygon2D(vertex,2,DX_NONE_GRAPH,true);

		DrawRotaGraph(kMpBarGraphPosX[0], kMpBarGraphPosY, 1.0, 0.0, barGraphHandle, false);
	}
	else
	{
		DrawRotaGraph(kMpBarGraphPosX[1], kMpBarGraphPosY, 1.0, 0.0, barGraphHandle, false, true);
	}

}

void GameUi::DrawFade(int color, int alpha)
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, color, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

int GameUi::UpdateResult()
{
	auto& input = MyEngine::Input::GetInstance();

	//Aボタンを押したときに選択されている項目を実行する
	if (input.IsTrigger("A"))
	{
		return m_selectItem;
	}
	//上下入力を受け取る
	if (input.IsTrigger("Down"))
	{
		m_selectItem++;

		m_selectItem = min(m_selectItem, static_cast<int>(SelectItem::kItemNum));
	}
	else if (input.IsTrigger("Up"))
	{
		m_selectItem--;

		m_selectItem = max(m_selectItem, 0);
	}

	return -1;

}

void GameUi::DrawResult(bool isWin)
{
	m_resultTime++;

	DrawString(50, 300, "もう一度たたかう", GetColor(255, 255, 255));
	DrawString(50, 350, "キャラクターセレクトに戻る", GetColor(255, 255, 255));
	DrawString(50, 400, "タイトルに戻る", GetColor(255, 255, 255));

	int posY = 0;

	if (m_selectItem == static_cast<int>(SelectItem::kRetry))
	{
		posY = 300;
	}
	else if (m_selectItem == static_cast<int>(SelectItem::kCharacterSelect))
	{
		posY = 350;
	}
	else if (m_selectItem == static_cast<int>(SelectItem::kTitle))
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
				logoPosX += GetRand(kResultLogoShakeScale) + (kResultLogoShakeScale * 0.5);
				logoPosY += GetRand(kResultLogoShakeScale) + (kResultLogoShakeScale * 0.5);
			}
			m_shakeTime++;
		}


		//アルファ値をあげていく
		m_resultLogoAlpha += kResultRogoFadeSpeed;

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_resultLogoAlpha);
		if (isWin)
		{
			DrawRotaGraph(logoPosX, logoPosY, m_resultLogoScale, 0.0, GraphManager::GetInstance().GetHandle("Winner"), true);
		}
		else
		{
			DrawRotaGraph(logoPosX, logoPosY, m_resultLogoScale, 0.0, GraphManager::GetInstance().GetHandle("Loser"), true);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

}
