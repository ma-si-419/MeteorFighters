#pragma once
#include "CharacterStateBase.h"
class CharacterStateGuard : public CharacterStateBase,public std::enable_shared_from_this<CharacterStateGuard>
{
public:
	CharacterStateGuard(std::shared_ptr<CharacterBase> character);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

};

