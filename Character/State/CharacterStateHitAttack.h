#pragma once
#include "CharacterStateBase.h"
#include <list>
class CharacterStateHitAttack : public CharacterStateBase,public std::enable_shared_from_this<CharacterStateHitAttack>
{
public:

	CharacterStateHitAttack(std::shared_ptr<Character> character);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	/// <summary>
	/// 攻撃を受けた時に呼ぶ関数
	/// </summary>
	/// <param name="reaction">攻撃を受けた時の反応(Character::HitReactionKindをintにキャストする)</param>
	void HitAttack(int reaction);

	/// <summary>
	/// 今再生しているアニメーションと受けた攻撃から次に再生するアニメーションを考えて返す
	/// </summary>
	/// <param name="kind">受けた攻撃の種類</param>
	/// <returns>次に再生するアニメーション</returns>
	int GetNextAnimKind(int kind);

	/// <summary>
	/// 受け身できる状況かどうかを返す
	/// </summary>
	/// <returns>受け身できるのならtrueを返す</returns>
	bool IsFalls() { return m_isFalls; }

private:

	//動かされる時間
	int m_moveTime;

	//移動ベクトル
	MyEngine::Vector3 m_moveVec;

	//移動距離
	float m_moveLength;

	//減速ベクトル
	MyEngine::Vector3 m_decelerationVec;

	//今まで受けた攻撃
	std::list<int> m_hitReactions;

	//前方から殴られたかどうか
	bool m_isFrontHit;

	//ステージとぶつかるかどうか
	bool m_isStageBump;

	//受け身できるかどうか
	bool m_isFalls;
};