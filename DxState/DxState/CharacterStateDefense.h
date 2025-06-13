#pragma once
#include "CharacterStateBase.h"
class CharacterStateDefense : public CharacterStateBase
{
public:
	CharacterStateDefense();
	~CharacterStateDefense();

	virtual void Enter(Character* character) override;

	virtual void Update() override;

	virtual void Exit() override;
};

