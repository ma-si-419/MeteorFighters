#pragma once
#include "PlayerStateBase.h"
class PlayerStateMove : public PlayerStateBase, public std::enable_shared_from_this<PlayerStateMove>
{
public:
	PlayerStateMove(std::shared_ptr<Player> player);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
private:

	std::string m_attackKey;

	//空中に浮くかどうか
	bool m_isFloat;

	//今自分にかかっている重力
	float m_gravityPower;

	//前のフレームに地上にいたかどうか
	bool m_isLastGround;

	int m_attackButtonPushTime;
};

