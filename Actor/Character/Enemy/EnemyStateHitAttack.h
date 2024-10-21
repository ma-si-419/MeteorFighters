#pragma once
#include "EnemyStateBase.h"
#include "Vector3.h"
#include <list>
class EnemyStateHitAttack : public EnemyStateBase, public std::enable_shared_from_this<EnemyStateHitAttack>
{
public:

	/// <summary>
	/// 攻撃を受けた時の反応
	/// </summary>
	enum class HitReaction
	{
		kLow,//弱
		kMid,//中
		kHigh,//強
		kBurst,//吹っ飛び
		kBottomStan,//下段スタン
		kMiddleStan,//中段スタン
		kTopStan,//上段スタン
		kReactionNum
	};
	/// <summary>
	/// 吹き飛ばす力
	/// </summary>
	enum class BurstPower
	{
		kNone,
		kLow,
		kMid,
		kHigh
	};

	/// <summary>
	/// 攻撃を受けた際にどちらの方向に動かすのか
	/// </summary>
	enum class HitDirection
	{
		kUp,
		kDown,
		kFar
	};

	/// <summary>
	/// 攻撃を受けた時に呼ぶ関数
	/// </summary>
	/// <param name="reaction">攻撃を受けた時の反応</param>
	/// <param name="power">吹き飛ぶ強さ</param>
	/// <param name="direction">吹き飛ぶ方向</param>
	void HitAttack(HitReaction reaction,BurstPower power,HitDirection direction);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

private:

	/// <summary>
	/// 動けない時間を取得する
	/// </summary>
	/// <param name="hitReaction">受けた攻撃</param>
	/// <returns>動けない時間</returns>
	int GetDownTime(HitReaction hitReaction);

	//動けない時間
	int m_downTime;

	//移動ベクトル
	MyEngine::Vector3 m_moveVec;

	//今まで受けた攻撃
	std::list<HitReaction> m_hitReactions;

	//現在のやられ状態
	HitReaction m_hitReaction;
};

