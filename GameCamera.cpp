#include "GameCamera.h"

namespace
{
	const MyEngine::Vector3 kPlayerToCameraInitVec(-170,150,-150);
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
	m_localPos.SetLocalPos(kPlayerToCameraInitVec);
}

void GameCamera::Update()
{
	MyEngine::Vector3 cameraPos = m_localPos.GetWorldPos();
	SetCameraPositionAndTarget_UpVecY(cameraPos.CastVECTOR(),m_targetPos.CastVECTOR());
}

void GameCamera::SetPlayerPosAndTarget(MyEngine::Vector3 player, MyEngine::Vector3 target)
{
	m_localPos.SetCenterPos(player);
	m_targetPos = target;
}

void GameCamera::SetFrontPos(MyEngine::Vector3 pos)
{
	m_localPos.SetFrontPos(pos);
}
