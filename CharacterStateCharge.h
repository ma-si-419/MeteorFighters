#pragma once
#include "CharacterStateBase.h"
class CharacterStateCharge : public CharacterStateBase,public std::enable_shared_from_this<CharacterStateCharge>
{
public:
	CharacterStateCharge(std::shared_ptr<CharacterBase> character);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;
};

