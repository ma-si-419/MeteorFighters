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

	//�󒆂ɕ������ǂ���
	bool m_isFloat;

	//�������ɂ������Ă���d��
	float m_gravityPower;

	//�O�̃t���[���ɒn��ɂ������ǂ���
	bool m_isLastGround;

	int m_attackButtonPushTime;
};

