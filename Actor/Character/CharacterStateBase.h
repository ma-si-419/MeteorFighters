#pragma once
#include "StateBase.h"
#include <string>
class Collidable;
class CharacterStateBase : public StateBase
{
public:
	enum class CharacterStateKind
	{
		kIdle,//‰½‚à‚µ‚Ä‚¢‚È‚¢ó‘Ô
		kMove,//ˆÚ“®ó‘Ô
		kJump,//ƒWƒƒƒ“ƒvó‘Ô
		kGuard,//ƒK[ƒhó‘Ô
		kNormalAttack,//’ÊíUŒ‚ó‘Ô
		kSpecialAttack,//•KE‹Zó‘Ô
		kRush,//“ËŒ‚ó‘Ô
		kHitAttack,//UŒ‚‚ğó‚¯‚½
		kButtonBashing,//“G‚Æ‚Ì‰Ÿ‚µ‡‚¢
		kEnergyAttack,//‹C’eUŒ‚
		kCharge,//ƒ`ƒƒ[ƒWó‘Ô
		kStateKindNum//State‚Ì”
	};

public:

	virtual void OnCollide(std::shared_ptr<Collidable> collider) abstract;

	CharacterStateKind GetKind() { return m_kind; }

	//Ÿ‚ÌState
	std::shared_ptr<CharacterStateBase> m_pNextState;

protected:

	virtual void ChangeState(std::shared_ptr<CharacterStateBase> nextState);

	//Œ»İ‚Ìó‘Ô
	CharacterStateKind m_kind = CharacterStateKind::kIdle;

	//State‚É“ü‚Á‚Ä‰½ƒtƒŒ[ƒ€—§‚Á‚½‚©
	float m_time = 0.0f;
};

