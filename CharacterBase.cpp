#include "CharacterBase.h"

CharacterBase::CharacterBase(ObjectTag tag):
	Actor(tag,ColliderData::Kind::kCapsule),
	m_modelHandle(-1)
{

}

CharacterBase::~CharacterBase()
{
}
