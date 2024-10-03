#pragma once
#include "PlayerStateBase.h"
class PlayerStateMove : public PlayerStateBase, public std::enable_shared_from_this<PlayerStateMove>
{
public:

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;
};

