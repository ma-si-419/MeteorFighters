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

	//Œ»İs‚Á‚Ä‚¢‚éUŒ‚‚Ì–¼‘O
	std::string m_nowAttackName;

	//Ÿ‚És‚¤UŒ‚‚Ì–¼‘O
	std::string m_nextAttackName;

	//Ÿ‚ÉUŒ‚‚ğs‚¤‚©‚Ç‚¤‚©
	bool m_isNextAttack;

	//Ši“¬UŒ‚‚ÅˆÚ“®‚·‚éÀ•W
	MyEngine::Vector3 m_moveTargetPos;
};

