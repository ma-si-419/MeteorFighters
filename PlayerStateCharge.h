#pragma once
#include "PlayerStateBase.h"
class PlayerStateCharge : public PlayerStateBase, public std::enable_shared_from_this<PlayerStateCharge>
{
public:
	PlayerStateCharge(std::shared_ptr<Player> player);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
};

