#pragma once
#include "CharacterStateBase.h"

class CharacterStateIdle : public CharacterStateBase
{
public:
	CharacterStateIdle() {};
	~CharacterStateIdle() {};

	virtual void Enter(Character* character) override;

	virtual void Update() override;

	virtual void Exit() override;
};

