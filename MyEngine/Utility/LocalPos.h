#pragma once
#include "Vector3.h"
struct LocalPos
{

	LocalPos();

	MyEngine::Vector3 GetWorldPos();

	void SetCenterPos(MyEngine::Vector3 pos);

	void SetLocalPos(MyEngine::Vector3 pos);

	void SetFrontPos(MyEngine::Vector3 frontPos);


private:

	MyEngine::Vector3 m_centerPos;
	MyEngine::Vector3 m_centerRotation;

	MyEngine::Vector3 m_localPos;

};

