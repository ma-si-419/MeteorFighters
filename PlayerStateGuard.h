#pragma once
#include "PlayerStateBase.h"
class PlayerStateGuard : public PlayerStateBase, public std::enable_shared_from_this<PlayerStateGuard>
{
public:
	PlayerStateGuard(std::shared_ptr<Player> player);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;
};

