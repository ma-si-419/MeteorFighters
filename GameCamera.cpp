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
	//���̃J�����̃��[���h���W
	MyEngine::Vector3 cameraWorldPos = m_localPos.GetWorldPos();
	//�v���C���[�̎��̍��W
	MyEngine::Vector3 playerPos = m_localPos.GetCenterPos() + m_playerVelo;
	//���S���W�̍X�V
	m_localPos.SetCenterPos(playerPos);
	//�v���C���[����J�����ւ̎��̍��W�ɑ΂���x�N�g��
	MyEngine::Vector3 playerToCamera = cameraWorldPos - playerPos;
	//���̃J�����̃��[�J�����W
	MyEngine::Vector3 nextCameraLocalPos = m_localPos.ChangeWorldToLocal(playerPos + playerToCamera);
	
	//X,Y,Z���ꂼ������̒l�̒��ɂƂǂ߂�悤�ɂ���
	nextCameraLocalPos.x = std::fmin(nextCameraLocalPos.x, kMaxLocalPosX);
	nextCameraLocalPos.y = std::fmin(nextCameraLocalPos.y, kMaxLocalPosY);
	nextCameraLocalPos.z = std::fmin(nextCameraLocalPos.z, kMaxLocalPosZ);

	nextCameraLocalPos.x = std::fmax(nextCameraLocalPos.x, kMinLocalPosX);
	nextCameraLocalPos.y = std::fmax(nextCameraLocalPos.y, kMinLocalPosY);
	nextCameraLocalPos.z = std::fmax(nextCameraLocalPos.z, kMinLocalPosZ);

	DrawFormatString(0,112,GetColor(255,255,255),"���[�J�����W:%.1f,%.1f,%.1f",nextCameraLocalPos.x, nextCameraLocalPos.y, nextCameraLocalPos.z);

	//���[�J�����W�̐ݒ�
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
