#pragma once
#include "EnemyStateBase.h"
class EnemyStateMove : public EnemyStateBase, public std::enable_shared_from_this<EnemyStateMove>
{
public:
	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;
};

