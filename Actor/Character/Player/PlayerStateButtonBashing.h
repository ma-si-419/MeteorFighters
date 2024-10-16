#pragma once
#include "PlayerStateBase.h"
class PlayerStateButtonBashing : public PlayerStateBase, public std::enable_shared_from_this<PlayerStateButtonBashing>
{
public:
	PlayerStateButtonBashing(std::shared_ptr<Player> player);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;
};

