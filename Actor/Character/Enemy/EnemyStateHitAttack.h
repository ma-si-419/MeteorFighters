#pragma once
#include "EnemyStateBase.h"
class EnemyStateHitAttack : public EnemyStateBase, public std::enable_shared_from_this<EnemyStateHitAttack>
{
public:
	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;
};

