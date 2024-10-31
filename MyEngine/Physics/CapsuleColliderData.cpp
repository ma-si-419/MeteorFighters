#include "CapsuleColliderData.h"

CapsuleColliderData::CapsuleColliderData() :
	ColliderData(ColliderData::Kind::kCapsule),
	m_isMoveEndPos(true),
	m_radius(0)
{
}
