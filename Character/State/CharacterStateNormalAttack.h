#pragma once
#include "CharacterStateBase.h"
class CharacterStateNormalAttack : public CharacterStateBase,public std::enable_shared_from_this<CharacterStateNormalAttack>
{
public:

	CharacterStateNormalAttack(std::shared_ptr<Character> character);

	/// <summary>
	/// このStateに入るときに最初に呼ぶ関数
	/// </summary>
	/// <param name="key">どのキーが押されたのか</param>
	/// <param name="attackName">何の攻撃を行うのか</param>
	void SetAttack(std::string key, std::string attackName);

	/// <summary>
	/// ダッシュから攻撃など移動が変化する攻撃の際に呼ぶ関数
	/// </summary>
	/// <param name="velo">攻撃の移動ベクトル</param>
	void SetAttackVelo(MyEngine::Vector3 velo);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	/// <summary>
	/// 攻撃までの時間を取得する
	/// </summary>
	/// <returns>攻撃までの時間(攻撃をすでに出していたら-1を返す)</returns>
	int GetTimeToAttack();

	std::string GetNowAttackName() { return m_nowAttackName; }

	bool GetEndAttack() { return m_isEndAttack; };
private:

	//現在行っている攻撃の名前
	std::string m_nowAttackName;

	//次に行う攻撃の名前
	std::string m_nextAttackName;

	//次の攻撃にチャージができるかどうか
	bool m_isNextCharge;

	//チャージ攻撃を行っているかどうか
	bool m_isCharge;

	//何フレームチャージを行ったか
	float m_chargeTime;

	//攻撃を行ったかどうか
	bool m_isAttacked;

	//何キーを押して攻撃を行っているか
	std::string m_attackKey;

	//次に攻撃を行うかどうか
	bool m_isNextAttack;

	//瞬間移動攻撃の追撃を行ったかどうか
	int m_chaseAttackNum;

	//最初の攻撃だけ移動が変わる可能性があるため
	MyEngine::Vector3 m_firstAttackMoveVec;

	//格闘攻撃で移動する座標
	MyEngine::Vector3 m_moveTargetPos;

	//攻撃が終了したかどうか
	bool m_isEndAttack;
};