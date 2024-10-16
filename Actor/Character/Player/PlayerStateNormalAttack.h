#pragma once
#include "PlayerStateBase.h"
#include <string>
class PlayerStateNormalAttack : public PlayerStateBase, public std::enable_shared_from_this<PlayerStateNormalAttack>
{
public:

	PlayerStateNormalAttack(std::shared_ptr<Player> player);

	void SetAttack(std::string attackName);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

private:

	std::string m_nowAttackName;

};

