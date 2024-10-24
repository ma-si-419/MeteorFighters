#pragma once
#include "EnemyStateBase.h"
class EnemyStateAttack : public EnemyStateBase, public std::enable_shared_from_this<EnemyStateAttack>
{
public:

	EnemyStateAttack(std::shared_ptr<Enemy> enemy);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
};

