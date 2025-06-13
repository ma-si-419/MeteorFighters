#pragma once
#include "CharacterStateBase.h"
class CharacterStateAttack : public CharacterStateBase
{
public:
	CharacterStateAttack();
	~CharacterStateAttack();

	virtual void Enter(Character* character) override;

	virtual void Update() override;

	virtual void Exit() override;
};

