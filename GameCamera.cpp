#include "GameCamera.h"
#include <cmath>

namespace
{
	const MyEngine::Vector3 kPlayerToCameraInitVec(-17, 15, -30);

	constexpr float kCameraNear = 1.0f;
	constexpr float kCameraFar = 500.0f;

	constexpr float kCameraToPlayerLange = 50.0f;

	constexpr float kOnMoveMaxLocalPosX = 42.0f;//����(�傫���قǍ��ɍL����)
	constexpr float kOnMoveMaxLocalPosY = 18.0f;//����(�傫���قǉ��ɍL����)
	constexpr float kOnMoveMaxLocalPosZ = -30.0f;//��O��(�傫���قǃv���C���[����O�ɗ���)

	constexpr float kOnMoveMinLocalPosX = -42.0f;//�E��(�������قǉE�ɍL����)
	constexpr float kOnMoveMinLocalPosY = -12.0f;//�㕝(�������قǏ�ɍL����)
	constexpr float kOnMoveMinLocalPosZ = -40.0f;//����(�������قǃv���C���[�����ɍs��)


	constexpr float kLocalInitPosX = 27.0f;
	constexpr float kLocalInitTopPosY = -2.0f;
	constexpr float kLocalInitUnderPosY = 8.0f;
	constexpr float kLocalInitPosZ = -30.0f;

	//�v���C���[�ƃ^�[�Q�b�g�̊Ԃłǂ̓�����ɃJ�����̏œ_�����킹�邩
	constexpr float kCameraTargetPosRate = 0.4;

	//���[�J�����W�̕␳�ɂ����鎞��
	constexpr int kLocalPosMoveTime = 12;

	//���[�J�����W�̕␳���n�߂鎞��
	constexpr int kLocalPosMoveStartTime = 5;

	//�����ɂ���ĉ��ړ��̂����т����炷����
	constexpr float kLangeSubRate = 0.1f;

}

GameCamera::GameCamera() :
	m_lightHandle(-1),
	m_stopTime(0)
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
	m_nextCameraPos = m_localPos.GetWorldPos();
}

void GameCamera::Update()
{
	//���[�J�����W�̐ݒ�
	m_localPos.SetLocalPos(m_nextCameraPos);

	//���̃J�����̃��[���h���W
	MyEngine::Vector3 cameraWorldPos = m_localPos.GetWorldPos();
	//�v���C���[�̎��̍��W
	MyEngine::Vector3 playerPos = m_localPos.GetCenterPos() + m_playerVelo;
	//���S���W�̍X�V
	m_localPos.SetCenterPos(playerPos);
	//�v���C���[����J�����ւ̎��̍��W�ɑ΂���x�N�g��
	MyEngine::Vector3 playerToCamera = cameraWorldPos - playerPos;
	//���̃J�����̃��[�J�����W
	m_nextCameraPos = m_localPos.ChangeWorldToLocal(playerPos + playerToCamera);

	//X,Y,Z���ꂼ������̒l�̒��ɂƂǂ߂�悤�ɂ���

	//�ő�l�̒���
	
	//X���W�͗����قǂ����т����������Ă���
	float maxX = kOnMoveMaxLocalPosX - ((m_nextCameraPos - m_targetPos).Length()) * kLangeSubRate;
	m_nextCameraPos.x = std::fmin(m_nextCameraPos.x, maxX);
	
	
	m_nextCameraPos.y = std::fmin(m_nextCameraPos.y, kOnMoveMaxLocalPosY);
	m_nextCameraPos.z = std::fmin(m_nextCameraPos.z, kOnMoveMaxLocalPosZ);

	//�ŏ��l�̒���
	
	//X���W�͗����قǂ����т����������Ă���
	float minX = kOnMoveMinLocalPosX + ((m_nextCameraPos - m_targetPos).Length()) * kLangeSubRate;
	m_nextCameraPos.x = std::fmax(m_nextCameraPos.x, minX);
	
	
	m_nextCameraPos.y = std::fmax(m_nextCameraPos.y, kOnMoveMinLocalPosY);
	m_nextCameraPos.z = std::fmax(m_nextCameraPos.z, kOnMoveMinLocalPosZ);

	//�v���C���[���ړ����Ă��Ȃ��Ƃ�
	if (m_playerVelo.SqLength() < 0.001f)
	{
		//�ړ����Ă��Ȃ����Ԃ��v��
		m_stopTime++;
		
		//�ړ����Ă��Ȃ����Ԃ���莞�Ԃ�������
		if (m_stopTime > kLocalPosMoveStartTime)
		{
			//��莞�Ԃ����ă��[�J�����W������̍��W�ɋ߂Â��Ă���
			MyEngine::Vector3 initPos(m_nextCameraPos.x, m_nextCameraPos.y, kLocalInitPosZ);

			//X���W�͍��E�𒴂��Ă���������W�܂Ŗ߂�
			if (m_nextCameraPos.x > kLocalInitPosX)
			{
				initPos.x = kLocalInitPosX - ((m_nextCameraPos - m_targetPos).Length()) * kLangeSubRate;
			}
			else if (m_nextCameraPos.x < -kLocalInitPosX)
			{
				initPos.x = -kLocalInitPosX + ((m_nextCameraPos - m_targetPos).Length()) * kLangeSubRate;
			}
			//Y���W�͏㉺�Ŗ߂����W��ς���
			if (m_nextCameraPos.y > kLocalInitUnderPosY)
			{
				initPos.y = kLocalInitUnderPosY;
			}
			else if (m_nextCameraPos.y < kLocalInitTopPosY)
			{
				initPos.y = kLocalInitTopPosY;
			}

			//�ړ��x�N�g�����w�肳��Ă��Ȃ����
			if (m_moveVec.SqLength() < 0.001f)
			{
				MyEngine::Vector3 moveDir = (initPos - m_nextCameraPos).Normalize();
				float moveSpeed = (initPos - m_nextCameraPos).Length() / kLocalPosMoveTime;

				m_moveVec = moveDir * moveSpeed;
			}

			if ((initPos - m_nextCameraPos).Length() < m_moveVec.Length())
			{
				m_moveVec = m_moveVec.Normalize();

				m_moveVec *= (initPos - m_nextCameraPos).Length();
			}

			m_nextCameraPos += m_moveVec;
		}
	}
	else
	{
		//�������тɃ��Z�b�g
		m_moveVec = MyEngine::Vector3(0.0f, 0.0f, 0.0f);
		m_stopTime = 0;
	}

	//�^�[�Q�b�g���W�̐ݒ�
	MyEngine::Vector3 targetPos = (m_targetPos - m_localPos.GetCenterPos()) * kCameraTargetPosRate + m_localPos.GetCenterPos();

	SetCameraPositionAndTarget_UpVecY(cameraWorldPos.CastVECTOR(), targetPos.CastVECTOR());

	SetLightDirectionHandle(m_lightHandle, (cameraWorldPos - targetPos).Normalize().CastVECTOR());
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
