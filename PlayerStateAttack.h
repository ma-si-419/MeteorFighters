#pragma once
#include "PlayerStateBase.h"
class PlayerStateAttack : public PlayerStateBase, public std::enable_shared_from_this<PlayerStateAttack>
{
public:

	PlayerStateAttack(std::shared_ptr<Player> player);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;
};

