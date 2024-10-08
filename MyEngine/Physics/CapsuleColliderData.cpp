#include "CapsuleColliderData.h"

CapsuleColliderData::CapsuleColliderData() :
	ColliderData(ColliderData::Kind::kCapsule),
	m_isMoveStartPos(true),
	m_radius(0),
	m_length(0)
{
}
