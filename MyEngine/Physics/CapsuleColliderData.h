#pragma once
#include "ColliderData.h"
#include "Vector3.h"
class CapsuleColliderData : public ColliderData
{
public:
	CapsuleColliderData();
	MyEngine::Vector3 m_startPos;
	MyEngine::Vector3 m_nextStartPos;
	float m_radius;
	MyEngine::Vector3 m_lange;
	bool m_isMoveStartPos;
};

