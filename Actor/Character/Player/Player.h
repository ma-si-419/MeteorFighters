#pragma once
#include "CharacterBase.h"
class PlayerStateBase;
class Player : public CharacterBase
{
public:

	Player(CharacterKind kind);
	virtual ~Player();

	void Init() override;
	void Update() override;
	void Draw() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

private:

	friend PlayerStateBase;
};

