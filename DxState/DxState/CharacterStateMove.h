#pragma once
#include "CharacterStateBase.h"

class CharacterStateMove : public CharacterStateBase
{
public:
	CharacterStateMove() {};
	~CharacterStateMove() {};

	virtual void Enter(Character* character) override;

	virtual void Update() override;

	virtual void Exit() override;
};

