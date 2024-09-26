#include "CapsuleColliderData.h"

CapsuleColliderData::CapsuleColliderData() :
	ColliderData(ColliderData::Kind::kCapsule),
	m_isMoveStartPos(true)
{
}
