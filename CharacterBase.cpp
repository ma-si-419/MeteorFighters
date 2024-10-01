#include "CharacterBase.h"

CharacterBase::CharacterBase(ObjectTag tag):
	Actor(tag,ColliderData::Kind::kCapsule),
	m_modelHandle(-1)
{

}

CharacterBase::~CharacterBase()
{
}

void CharacterBase::SetGameManager(std::shared_ptr<GameManager> manager)
{
	m_pGameManager = manager;
}

MyEngine::Vector3 CharacterBase::GetPos()
{
	return m_rigidbody.GetPos();
}
