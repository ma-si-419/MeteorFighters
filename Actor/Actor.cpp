#include "Actor.h"

Actor::Actor(ObjectTag tag, ColliderData::Kind colKind):
	Collidable(tag,colKind)
{
}


Actor::~Actor()
{
}