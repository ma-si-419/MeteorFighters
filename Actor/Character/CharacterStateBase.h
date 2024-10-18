#pragma once
#include "StateBase.h"
class Collidable;
class CharacterStateBase : public StateBase
{
public:
	enum class CharacterStateKind
	{
		kIdle,//‰½‚à‚µ‚Ä‚¢‚È‚¢ó‘Ô
		kMove,//ˆÚ“®ó‘Ô
		kGuard,//ƒK[ƒhó‘Ô
		kAttack,//UŒ‚ó‘Ô
		kRush,//“ËŒ‚ó‘Ô
		kHitAttack,//UŒ‚‚ğó‚¯‚½
		kButtonBashing//“G‚Æ‚Ì‰Ÿ‚µ‡‚¢
	};

	virtual void OnCollide(std::shared_ptr<Collidable> collider) abstract;

	CharacterStateKind GetKind() { return m_kind; }

	//Ÿ‚ÌState
	std::shared_ptr<CharacterStateBase> m_pNextState;

protected:
	
	virtual void ChangeState(std::shared_ptr<CharacterStateBase> nextState) abstract;

	//Œ»İ‚Ìó‘Ô
	CharacterStateKind m_kind;

	//State‚É“ü‚Á‚Ä‰½ƒtƒŒ[ƒ€—§‚Á‚½‚©
	int m_time = 0;

};

