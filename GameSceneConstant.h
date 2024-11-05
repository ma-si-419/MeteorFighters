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

}