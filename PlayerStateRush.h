#pragma once
#include "PlayerStateBase.h"
class PlayerStateRush : public PlayerStateBase, public std::enable_shared_from_this<PlayerStateRush>
{
public:

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;
};

