#pragma once
#include "Collidable.h"
class Actor : public Collidable
{
public:
	Actor(ObjectTag tag, ColliderData::Kind colKind);
	virtual ~Actor();

	virtual void Init() abstract;
	virtual void Update() abstract;
	virtual void Draw() abstract;

};

