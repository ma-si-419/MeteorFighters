#pragma once
#include "StateBase.h"
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

	CharacterStateKind GetKind() { return m_kind; }

	//Ÿ‚ÌState
	std::shared_ptr<CharacterStateBase> m_pNextState;

protected:
	
	virtual void ChangeState(CharacterStateKind nextState) abstract;

	//Œ»İ‚Ìó‘Ô
	CharacterStateKind m_kind;

};

