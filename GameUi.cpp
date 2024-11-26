#include "GameUi.h"
#include "GraphManager.h"
#include "DxLib.h"
#include <cmath>
#include "Game.h"

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
