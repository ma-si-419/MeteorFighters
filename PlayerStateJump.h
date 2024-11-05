#pragma once
#include "PlayerStateBase.h"
class CapsuleColliderData;
class PlayerStateJump : public PlayerStateBase, public std::enable_shared_from_this<PlayerStateJump>
{
public:

	PlayerStateJump(std::shared_ptr<Player> player);

	/// <summary>
	/// 左右前後移動ベクトルのあるジャンプを行うときに呼ぶ
	/// </summary>
	/// <param name="moveVec">今の移動ベクトル</param>
	void StartJump(MyEngine::Vector3 moveVec);

	/// <summary>
	/// 左右前後移動がないジャンプを行う時に呼ぶ
	/// </summary>
	void StartJump();

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

private:

	std::string m_attackKey;

	int m_attackButtonPushTime;

	MyEngine::Vector3 m_moveVec;

};

