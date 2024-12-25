#pragma once
#include "CharacterStateBase.h"
class Effect;
class Attack;
class CharacterStateSpecialAttack : public CharacterStateBase,public std::enable_shared_from_this<CharacterStateSpecialAttack>
{
public:
	CharacterStateSpecialAttack(std::shared_ptr<Character> character);

	/// <summary>
	/// 必殺技の番号を設定する
	/// </summary>
	/// <param name="number">必殺技の番号</param>
	void SetSpecialAttackNumber(int number) { m_specialAttackNumber = number; }

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

private:

	//キャラクターに設定されている必殺技の何番目を使用するか
	int m_specialAttackNumber;

	//技を出している時間(後隙の時間は技ごとに設定)
	int m_attackTime;

	//敵の状態が何になったら技を出すのをやめるか
	int m_endHitReaction;

	//攻撃を出したかどうか
	bool m_isAttacked;

	//攻撃のエフェクト
	std::shared_ptr<Effect> m_pEffect;

	//出した攻撃のポインタ
	std::shared_ptr<Attack> m_pAttack;
};