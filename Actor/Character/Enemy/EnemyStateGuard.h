#pragma once
#include "EnemyStateBase.h"
class EnemyStateGuard : public EnemyStateBase, public std::enable_shared_from_this<EnemyStateGuard>
{
public:
	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;
};

