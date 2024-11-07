#pragma once
/// <summary>
/// ゲームシーンで共通で扱う定数
/// </summary>
namespace GameSceneConstant
{
	//ジャンプ力
	constexpr float kJumpPower = 6.0f;
	//空中での重力の強さ
	constexpr float kSkyGravityPower = -0.2f;
	//地上での重力の強さ
	constexpr float kGroundGravityPower = -0.5f;
	//落下速度の限界
	constexpr float kMaxFallSpeed = -3.5f;
	//チャージ攻撃と判定する長押しの時間
	constexpr int kChargeAttackTime = 15;
	//ダッシュに使う気力
	constexpr int kDashCost = 20;
	//敵との距離が近いと判定する距離
	constexpr float kNearLange = 75.0f;
	//モデルの大きさ
	constexpr float kModelScale = 0.1f;
	//残像の初期透明度
	constexpr float kAfterImageInitOpacityRate = 3.0f;
	//残像の一番濃い透明度
	constexpr float kAfterImageMaxOpacityRate = 0.65f;
	//残像を作成する感覚
	constexpr int kAfterImageCreateTime = 6;
	//残像を消す速さ
	constexpr float kAfterImageDeleteSpeed = kAfterImageInitOpacityRate / 10;
}