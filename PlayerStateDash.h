#pragma once
#include "PlayerStateBase.h"
class PlayerStateDash : public PlayerStateBase, public std::enable_shared_from_this<PlayerStateDash>
{
public:
	PlayerStateDash(std::shared_ptr<Player> player);

	void SetMoveDir(MyEngine::Vector3 moveDir);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

private:
	MyEngine::Vector3 m_moveDir;
};
