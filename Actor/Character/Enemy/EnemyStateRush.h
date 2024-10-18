#pragma once
#include "EnemyStateBase.h"
class EnemyStateRush : public EnemyStateBase, public std::enable_shared_from_this<EnemyStateRush>
{
public:
	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
};

