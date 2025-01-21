#pragma once
#include "CharacterStateBase.h"
class CharacterStateGuard : public CharacterStateBase,public std::enable_shared_from_this<CharacterStateGuard>
{
public:
	CharacterStateGuard(std::shared_ptr<Character> character);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;
};