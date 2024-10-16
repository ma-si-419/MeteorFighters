#pragma once
#include "CharacterStateBase.h"

class EnemyStateBase : public CharacterStateBase
{
protected:
	//State‚ÌØ‘Ö
	void ChangeState(std::shared_ptr<CharacterStateBase> nextState) override;
#ifdef _DEBUG

	//‚»‚ÌState‚É“ü‚Á‚Ä‰½ƒtƒŒ[ƒ€—§‚Á‚½‚©
	int m_time = 0;

#endif // _DEBUG

	
};

