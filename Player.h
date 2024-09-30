#pragma once
#include "CharacterBase.h"
class Player : public CharacterBase
{
public:

	Player();
	virtual ~Player();

	void Init() override;
	void Update() override;
	void Draw() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

private:
#ifdef _DEBUG

	bool isGround;

#endif // _DEBUG

};

