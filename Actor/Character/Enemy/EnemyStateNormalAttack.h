#pragma once
#include "EnemyStateBase.h"
class EnemyStateNormalAttack : public EnemyStateBase, public std::enable_shared_from_this<EnemyStateNormalAttack>
{
public:

	EnemyStateNormalAttack(std::shared_ptr<Enemy> enemy);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
};
