#pragma once
#include "PlayerStateBase.h"
class PlayerStateIdle : public PlayerStateBase , public std::enable_shared_from_this<PlayerStateIdle>
{
public:
	PlayerStateIdle(std::shared_ptr<Player> player);

	virtual void Enter() override;

	virtual void Update() override;
	
	virtual void Exit() override;

};
