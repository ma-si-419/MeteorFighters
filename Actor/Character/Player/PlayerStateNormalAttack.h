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

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	int GetAnimKind(std::string animName);
private:

	std::string m_nowAttackName;

	std::string m_nextAttackName;

	bool m_isNextAttack;

	MyEngine::Vector3 m_moveDir;
};

