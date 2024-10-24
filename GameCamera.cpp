#include "GameCamera.h"

namespace
{
	const MyEngine::Vector3 kPlayerToCameraInitVec(-17,15,-15);

	constexpr float kCameraNear = 1.0f;
	constexpr float kCameraFar = 500.0f;
}

GameCamera::GameCamera():
	m_cameraHandle(-1)
{
}

GameCamera::~GameCamera()
{

}

void GameCamera::Init(MyEngine::Vector3 centerPos)
{
	m_localPos.SetCenterPos(centerPos);
	m_localPos.SetLocalPos(kPlayerToCameraInitVec);
	SetCameraNearFar(kCameraNear, kCameraFar);

	m_cameraHandle = CreateDirLightHandle(VGet(0,0,0));

}

void GameCamera::Update()
{
	MyEngine::Vector3 cameraPos = m_localPos.GetWorldPos();
	SetCameraPositionAndTarget_UpVecY(cameraPos.CastVECTOR(),m_targetPos.CastVECTOR());

	SetLightDirectionHandle(m_cameraHandle,(cameraPos - m_targetPos).Normalize().CastVECTOR());
}

void GameCamera::SetPlayerPosAndTarget(MyEngine::Vector3 player, MyEngine::Vector3 target)
{
	m_localPos.SetCenterPos(player);
	m_targetPos = target;
}

void GameCamera::SetPlayerFrontPos(MyEngine::Vector3 pos)
{
	m_localPos.SetFrontPos(pos);
}
