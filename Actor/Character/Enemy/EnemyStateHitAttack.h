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
	enum class HitKind
	{
		kLow,//弱
		kMiddle,//中
		kUpBurst,//上吹っ飛び
		kDownBurst,//下吹っ飛び
		kFarBurst,//奥吹っ飛び
		kBottomStan,//下段スタン
		kMiddleStan,//中段スタン
		kGuard,//ガード状態
		kGuardBreak,//ガードブレイク状態
		kKindNum
	};

public:

	EnemyStateHitAttack(std::shared_ptr<Enemy> enemy);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	/// <summary>
	/// 攻撃を受けた時に呼ぶ関数
	/// </summary>
	/// <param name="reaction">攻撃を受けた時の反応</param>
	void HitAttack(HitKind reaction);
	
	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

private:

	//動けない時間
	int m_downTime;

	//動かされる時間
	int m_moveTime;

	//移動ベクトル
	MyEngine::Vector3 m_moveVec;

	//今まで受けた攻撃
	std::list<HitKind> m_hitReactions;

	//現在のやられ状態
	HitKind m_hitReaction;
};

