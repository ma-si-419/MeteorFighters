#pragma once
#include "PlayerStateBase.h"
#include <string>
class PlayerStateNormalAttack : public PlayerStateBase, public std::enable_shared_from_this<PlayerStateNormalAttack>
{
public:

	PlayerStateNormalAttack(std::shared_ptr<Player> player);

	/// <summary>
	/// このStateに入るときに最初に呼ぶ関数
	/// </summary>
	/// <param name="key">何のボタンを押したのか</param>
	/// <param name="isCharge">チャージしたのか</param>
	void SetAttack(std::string key,bool isCharge);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
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

	//格闘攻撃で移動する座標
	MyEngine::Vector3 m_moveTargetPos;
};

