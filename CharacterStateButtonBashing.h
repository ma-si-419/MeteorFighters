#pragma once
#include "CharacterStateBase.h"
class CharacterStateButtonBashing : public CharacterStateBase,std::enable_shared_from_this<CharacterStateButtonBashing>
{
public:
	CharacterStateButtonBashing(std::shared_ptr<CharacterBase> character);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
};

