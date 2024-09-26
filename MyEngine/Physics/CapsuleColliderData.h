#pragma once
#include "ColliderData.h"
#include "Vector3.h"
class CapsuleColliderData : public ColliderData
{
public:
	CapsuleColliderData();
	MyEngine::Vector3 m_startPos;
	float m_radius;
	bool m_isMoveStartPos;
};

