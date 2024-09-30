#include "GameCamera.h"

namespace
{
	const MyEngine::Vector3 kPlayerToCameraInitVec(30,50,-100);
}

GameCamera::GameCamera()
{
}

GameCamera::~GameCamera()
{
}

void GameCamera::Init(MyEngine::Vector3 centerPos)
{
	m_localPos.SetCenterPos(centerPos);
}

void GameCamera::Update()
{
}
