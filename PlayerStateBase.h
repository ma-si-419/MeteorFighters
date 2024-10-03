#pragma once
#include "CharacterStateBase.h"
class PlayerStateBase : public CharacterStateBase
{
protected:
	void ChangeState(CharacterStateKind nextState) override;
};

