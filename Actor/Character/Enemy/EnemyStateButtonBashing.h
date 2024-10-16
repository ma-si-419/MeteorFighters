#pragma once
#include "EnemyStateBase.h"
class EnemyStateButtonBashing : public EnemyStateBase, public std::enable_shared_from_this<EnemyStateButtonBashing>
{
public:
	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;
};

