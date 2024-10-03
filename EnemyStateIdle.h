#pragma once
#include "EnemyStateBase.h"
class EnemyStateIdle : public EnemyStateBase, public std::enable_shared_from_this<EnemyStateIdle>
{
public:
	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;
};

