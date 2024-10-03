#pragma once
#include "CharacterBase.h"
class GameManager;
class Enemy : public CharacterBase
{
public:

	Enemy();
	virtual ~Enemy();

	void Init() override;
	void Update() override;
	void Draw() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
private:

};

