#include "GameCamera.h"
#include <cmath>

namespace
{
	const MyEngine::Vector3 kPlayerToCameraInitVec(-17, 15, -30);

	constexpr float kCameraNear = 1.0f;
	constexpr float kCameraFar = 500.0f;

	constexpr float kCameraToPlayerLange = 50.0f;

	constexpr float kMaxLocalPosX = 30;
	constexpr float kMaxLocalPosY = 10;
	constexpr float kMaxLocalPosZ = -35;

	constexpr float kMinLocalPosX = -30;
	constexpr float kMinLocalPosY = -15;
	constexpr float kMinLocalPosZ = -45;
}

GameCamera::GameCamera() :
	m_lightHandle(-1)
{
	m_lightHandle = CreateDirLightHandle(VGet(0, 0, 0));
}

GameCamera::~GameCamera()
{
	DeleteLightHandle(m_lightHandle);
}

void GameCamera::Init(MyEngine::Vector3 centerPos)
{
	m_localPos.SetCenterPos(centerPos);
	m_localPos.SetLocalPos(kPlayerToCameraInitVec);
	SetCameraNearFar(kCameraNear, kCameraFar);
	m_lastCameraPos = m_localPos.GetWorldPos();
}

void GameCamera::Update()
{
	//今のカメラのワールド座標
	MyEngine::Vector3 cameraWorldPos = m_localPos.GetWorldPos();
	//プレイヤーの次の座標
	MyEngine::Vector3 playerPos = m_localPos.GetCenterPos() + m_playerVelo;
	//中心座標の更新
	m_localPos.SetCenterPos(playerPos);
	//プレイヤーからカメラへの次の座標に対するベクトル
	MyEngine::Vector3 playerToCamera = cameraWorldPos - playerPos;
	//次のカメラのローカル座標
	MyEngine::Vector3 nextCameraLocalPos = m_localPos.ChangeWorldToLocal(playerPos + playerToCamera);
	
	//X,Y,Zそれぞれを一定の値の中にとどめるようにする
	nextCameraLocalPos.x = std::fmin(nextCameraLocalPos.x, kMaxLocalPosX);
	nextCameraLocalPos.y = std::fmin(nextCameraLocalPos.y, kMaxLocalPosY);
	nextCameraLocalPos.z = std::fmin(nextCameraLocalPos.z, kMaxLocalPosZ);

	nextCameraLocalPos.x = std::fmax(nextCameraLocalPos.x, kMinLocalPosX);
	nextCameraLocalPos.y = std::fmax(nextCameraLocalPos.y, kMinLocalPosY);
	nextCameraLocalPos.z = std::fmax(nextCameraLocalPos.z, kMinLocalPosZ);

	DrawFormatString(0,112,GetColor(255,255,255),"ローカル座標:%.1f,%.1f,%.1f",nextCameraLocalPos.x, nextCameraLocalPos.y, nextCameraLocalPos.z);

	//ローカル座標の設定
	m_localPos.SetLocalPos(nextCameraLocalPos);

	SetCameraPositionAndTarget_UpVecY(cameraWorldPos.CastVECTOR(), m_targetPos.CastVECTOR());

	SetLightDirectionHandle(m_lightHandle, (cameraWorldPos - m_targetPos).Normalize().CastVECTOR());

	m_lastCameraPos = cameraWorldPos;
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

void GameCamera::SetPlayerVelo(MyEngine::Vector3 velo)
{
	m_playerVelo = velo;
}

void GameCamera::SetCamera()
{
	SetCameraNearFar(kCameraNear, kCameraFar);

	MyEngine::Vector3 cameraPos = m_localPos.GetWorldPos();
	SetCameraPositionAndTarget_UpVecY(cameraPos.CastVECTOR(), m_targetPos.CastVECTOR());

	SetLightDirectionHandle(m_lightHandle, (cameraPos - m_targetPos).Normalize().CastVECTOR());
}
