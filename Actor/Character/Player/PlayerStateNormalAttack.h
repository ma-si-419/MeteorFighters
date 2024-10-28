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
private:

	//���ݍs���Ă���U���̖��O
	std::string m_nowAttackName;

	//���ɍs���U���̖��O
	std::string m_nextAttackName;

	//���ɍU�����s�����ǂ���
	bool m_isNextAttack;

	//�i���U���ňړ�������W
	MyEngine::Vector3 m_moveTargetPos;
};

