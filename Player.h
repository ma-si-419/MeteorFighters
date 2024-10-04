#pragma once
#include "CharacterBase.h"
class PlayerStateBase;
class GameCamera;
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

	std::shared_ptr<GameCamera> m_camera;

	friend PlayerStateBase;
};

