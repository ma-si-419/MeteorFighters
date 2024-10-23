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
	float m_length;
	bool m_isMoveStartPos;
};

