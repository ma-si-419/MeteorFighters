#pragma once
#include "CharacterStateBase.h"
class CharacterStateButtonBashing : public CharacterStateBase,public std::enable_shared_from_this<CharacterStateButtonBashing>
{
public:
	CharacterStateButtonBashing(std::shared_ptr<Character> character);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;
};
