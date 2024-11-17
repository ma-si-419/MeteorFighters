#include "LocalPos.h"
#include <cmath>

using namespace MyEngine;

LocalPos::LocalPos()
{
}

MyEngine::Vector3 LocalPos::GetWorldPos()
{
	MyEngine::Vector3 ans;

	MyEngine::Vector3 move = m_localPos;

	MATRIX mat = m_centerRotation.GetRotationMat();

	move = move.MatTransform(mat);

	ans = move + m_centerPos;

	return ans;
}

void LocalPos::SetCenterPos(MyEngine::Vector3 pos)
{
	m_centerPos = pos;
}

void LocalPos::SetLocalPos(MyEngine::Vector3 pos)
{
	m_localPos = pos;
}

void LocalPos::SetFrontPos(MyEngine::Vector3 frontPos)
{
	//Šp“x‚ÌŒvŽZ
	float vX = m_centerPos.x - frontPos.x;
	float vZ = m_centerPos.z - frontPos.z;

	float yAngle = std::atan2f(vX, vZ);

	MyEngine::Vector3 rotation(0.0f, yAngle + DX_PI_F, 0.0f);

	m_centerRotation = rotation;
}

MyEngine::Vector3 LocalPos::ChangeWorldToLocal(MyEngine::Vector3 worldPos)
{
	MyEngine::Vector3 ans;

	MyEngine::Vector3 toWorldPos = worldPos - m_centerPos;

	MATRIX mat = (-m_centerRotation).GetRotationMat();

	ans = toWorldPos.MatTransform(mat);

	return ans;
}
