#pragma once
#include "PlayerStateBase.h"
class PlayerStateRush : public PlayerStateBase, public std::enable_shared_from_this<PlayerStateRush>
{
public:
	PlayerStateRush(std::shared_ptr<Player> player);

	virtual void SetMoveDir(MyEngine::Vector3 dir);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

private:

	MyEngine::Vector3 m_moveDir;

	//“G‚ÌŒã‚ë‚ÉŒü‚©‚Á‚Ä“ËŒ‚‚·‚é‚©‚Ç‚¤‚©
	bool m_isRushEnemy;
};

