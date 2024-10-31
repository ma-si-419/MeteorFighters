#pragma once
#include "PlayerStateBase.h"
class CapsuleColliderData;
class PlayerStateJump : public PlayerStateBase
{
public:

	PlayerStateJump(std::shared_ptr<Player> player);

	/// <summary>
	/// ジャンプを行うときに呼ぶ
	/// </summary>
	/// <param name="moveVec">今の移動ベクトル</param>
	void StartJump(MyEngine::Vector3 moveVec);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

private:

	MyEngine::Vector3 m_moveVec;

	std::shared_ptr<CapsuleColliderData> m_pFallCollider;

};

