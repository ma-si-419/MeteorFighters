#include "GameCamera.h"
#include <cmath>

namespace
{
	const MyEngine::Vector3 kPlayerToCameraInitVec(17, 15, -30);

	constexpr float kCameraNear = 10.0f;
	constexpr float kCameraFar = 10000.0f;

	constexpr float kCameraToPlayerLange = 50.0f;

	constexpr float kOnMoveMaxLocalPosX = 42.0f;//����(�傫���قǍ��ɍL����)
	constexpr float kOnMoveMaxLocalPosY = 18.0f;//����(�傫���قǉ��ɍL����)
	constexpr float kOnMoveMaxLocalPosZ = -30.0f;//��O��(�傫���قǃv���C���[����O�ɗ���)

	constexpr float kOnMoveMinLocalPosX = -42.0f;//�E��(�������قǉE�ɍL����)
	constexpr float kOnMoveMinLocalPosY = -12.0f;//�㕝(�������قǏ�ɍL����)
	constexpr float kOnMoveMinLocalPosZ = -35.0f;//����(�������قǃv���C���[�����ɍs��)


	constexpr float kLocalInitPosX = 27.0f;
	constexpr float kLocalInitTopPosY = -2.0f;
	constexpr float kLocalInitUnderPosY = 8.0f;
	constexpr float kLocalInitPosZ = -30.0f;

	//�v���C���[�ƃ^�[�Q�b�g�̊Ԃłǂ̓�����ɃJ�����̏œ_�����킹�邩
	constexpr float kCameraTargetPosRate = 0.4f;

	//���[�J�����W�̕␳�ɂ����鎞��
	constexpr int kLocalPosMoveTime = 12;

	//���[�J�����W�̕␳���n�߂鎞��
	constexpr int kLocalPosMoveStartTime = 5;

	//�����ɂ���ĉ��ړ��̂����т����炷����
	constexpr float kLangeSubRate = 0.1f;

	//�O�̃t���[���̃J�������W�Ǝ��̃t���[���̃J�������W�̋����������Ɣ��肷�鋗��
	constexpr float kCameraFarLange = 100.0f;

	//�J�����̎��̍��W�������Ƃ��̈ړ����x�̍ő�
	constexpr float kFarCameraMaxMoveSpeed = 50.0f;
	//�J�����̎��̍��W�������Ƃ��̈ړ����x�̍ŏ�
	constexpr float kFarCameraMinMoveSpeed = 5.0f;

	//�J�����������ړ�����Ƃ��ɖڕW���W�܂ōs���̂ɂ����鎞��
	constexpr int kFastMoveTime = 5;

	//�ŏ��ړ��x�N�g��
	constexpr float kSmallestMoveVecLength = 2.0f;

	//�J������h�炷�傫��
	constexpr int kShakePower = 10;

	//���₩�ɃJ������h�炷���̑傫��
	constexpr float kSwayPower = 0.6f;

	//�ɂ₩�ɃJ������h�炷���̗h��̑���
	constexpr float kSwaySpeed = 0.5f;

	//�㉺�̍����ł����ۂ�Y���W���ǂꂾ�����炷��
	constexpr float kCameraPosMaxYGap = 10.0f;

	//�㉺�̍��̍ő�Ɣ��f����Y���W�̍�
	constexpr float kMaxYGap = 50.0f;

	//�㉺�̍�������Ƃ��̃v���C���[�ɋ߂Â��J�����̍ŋߋ���
	constexpr float kNearestLength = 10.0f;

	//�J�����ɍł��߂Â�����Y���W�̍�
	constexpr float kNearestCameraPosYGap = 100.0f;

}

GameCamera::GameCamera() :
	m_updateFunc(nullptr),
	m_lightHandle(-1),
	m_stopCorrectTime(0),
	m_isFastMove(false),
	m_isStop(false),
	m_shakeTime(0),
	m_isSway(false),
	m_swayTime(0.0f),
	m_stopTime(0)
{
	//�����C�g
	m_lightHandle = CreateDirLightHandle(VGet(0, 0, 0));
	SetCameraNearFar(kCameraNear, kCameraFar);
}

GameCamera::~GameCamera()
{
}

void GameCamera::SetPoseCamera()
{
	m_updateFunc = &GameCamera::PoseUpdate;
}

void GameCamera::Update()
{
	(this->*m_updateFunc)();

	//���C�g�̍��W�������ƍX�V��������
	SetLightPositionHandle(m_lightHandle, (m_localPos.GetWorldPos()).CastVECTOR());

}

void GameCamera::Final()
{
	DeleteLightHandle(m_lightHandle);
}

void GameCamera::SetBattleCamera()
{
	m_updateFunc = &GameCamera::BattleUpdate;
	m_localPos.SetLocalPos(kPlayerToCameraInitVec);
	m_nextCameraPos = m_localPos.GetWorldPos();
}

void GameCamera::SetCenterAndTarget(MyEngine::Vector3 player, MyEngine::Vector3 target)
{
	m_localPos.SetCenterPos(player);
	m_targetPos = target;
}

void GameCamera::SetFrontPos(MyEngine::Vector3 pos)
{
	m_localPos.SetFrontPos(pos);
}

void GameCamera::SetPlayerVelo(MyEngine::Vector3 velo)
{
	m_playerVelo = velo;
}

void GameCamera::SetLocalPos()
{
	m_localPos.SetLocalPos(kPlayerToCameraInitVec);
}

void GameCamera::ShakeCamera(int time)
{
	ShakeCamera(time,kShakePower);
}

void GameCamera::ShakeCamera(int time, int power)
{
	m_shakeTime = time;
	m_shakePower = power;
}

void GameCamera::PoseUpdate()
{
	//�h�炷�傫��(��{�̓[��)
	MyEngine::Vector3 shiftVec;

	//�J������h�炷�ݒ肪����Ă�����
	if (m_shakeTime > 0)
	{
		m_shakeTime--;

		//�}�C�i�X�ɂ��Ȃ�悤�Ƀ����_�����烉���_���̑傫���̔������炷
		int shakePowerHalf = static_cast<int>(m_shakePower * 0.5f);

		//�J�����̃^�[�Q�b�g���W��h�炷
		shiftVec.x = static_cast<float>(GetRand(m_shakePower) - shakePowerHalf);
		shiftVec.y = static_cast<float>(GetRand(m_shakePower) - shakePowerHalf);
		shiftVec.z = static_cast<float>(GetRand(m_shakePower) - shakePowerHalf);
	}

	//�J�������ɂ₩�ɗh�炷�ݒ肪����Ă�����
	if (m_isSway)
	{
		m_swayTime += kSwaySpeed;

		//�^�[�Q�b�g���W��h�炷
		shiftVec.y += sinf(m_swayTime) * kSwayPower;

		//�ɂ₩�ɗh�炷�̂���߂�ݒ�𖈃t���[������
		m_isSway = false;
	}

	SetCameraPositionAndTarget_UpVecY(m_localPos.GetWorldPos().CastVECTOR(), (m_targetPos + shiftVec).CastVECTOR());

	SetLightDirectionHandle(m_lightHandle, (m_localPos.GetWorldPos() - m_targetPos).Normalize().CastVECTOR());
}

void GameCamera::BattleUpdate()
{
	//�����Ȃ����Ԃ��ݒ肳��Ă�����
	if (m_stopTime > 0)
	{
		m_stopTime--;
		if (m_stopTime <= 0)
		{
			m_isStop = false;
		}
	}


	//�����Ȃ��Ɛݒ肳��Ă����烊�^�[��
	if (m_isStop)return;

	//���l�ȉ��̈ړ��x�N�g����0�ɖ߂��Ă���
	if (m_moveVec.Length() < kSmallestMoveVecLength)
	{
		m_moveVec = MyEngine::Vector3(0.0f, 0.0f, 0.0f);
	}

	//���̃J�����̃��[���h���W
	//�v���C���[�̎��̍��W
	MyEngine::Vector3 playerPos = m_localPos.GetCenterPos() + m_playerVelo;
	//���S���W�̍X�V
	MyEngine::Vector3 cameraWorldPos = m_localPos.GetWorldPos();
	m_localPos.SetCenterPos(playerPos);
	//�v���C���[����J�����ւ̎��̍��W�ɑ΂���x�N�g��
	MyEngine::Vector3 playerToCamera = cameraWorldPos - playerPos;
	//���̃J�����̃��[�J�����W
	m_nextCameraPos = m_localPos.ChangeWorldToLocal(playerPos + playerToCamera);

	//X,Y,Z���ꂼ������̒l�̒��ɂƂǂ߂�悤�ɂ���

	//�ő�l�̒���
	m_nextCameraPos.x = std::fmin(m_nextCameraPos.x, kOnMoveMaxLocalPosX);
	m_nextCameraPos.y = std::fmin(m_nextCameraPos.y, kOnMoveMaxLocalPosY);
	m_nextCameraPos.z = std::fmin(m_nextCameraPos.z, kOnMoveMaxLocalPosZ);

	//�ŏ��l�̒���
	m_nextCameraPos.x = std::fmax(m_nextCameraPos.x, kOnMoveMinLocalPosX);
	m_nextCameraPos.y = std::fmax(m_nextCameraPos.y, kOnMoveMinLocalPosY);
	m_nextCameraPos.z = std::fmax(m_nextCameraPos.z, kOnMoveMinLocalPosZ);

	//�㉺�̍����傫����΃J�����̍��W��␳����
	MyEngine::Vector3 addVec;

	//�㉺�̍��ɍ��킹�ăJ�����̍��W���㉺�ɓ�����
	float yGap = m_targetPos.y - m_localPos.GetCenterPos().y;	

	float yShiftLength = yGap / kMaxYGap;

	addVec.y = -yShiftLength;
	
	m_nextCameraPos += addVec;

	//�v���C���[���ړ����Ă��Ȃ��Ƃ�
	if (m_playerVelo.SqLength() < 0.001f)
	{
		//�ړ����Ă��Ȃ����Ԃ��v��
		m_stopCorrectTime++;

		//�ړ����Ă��Ȃ����Ԃ���莞�Ԃ�������
		if (m_stopCorrectTime > kLocalPosMoveStartTime)
		{
			//��莞�Ԃ����ă��[�J�����W������̍��W�ɋ߂Â��Ă���
			MyEngine::Vector3 initPos(m_nextCameraPos.x, m_nextCameraPos.y, kLocalInitPosZ);

			//X���W�͍��E�𒴂��Ă���������W�܂Ŗ߂�
			if (m_nextCameraPos.x > kLocalInitPosX)
			{
				initPos.x = kLocalInitPosX;
			}
			else if (m_nextCameraPos.x < -kLocalInitPosX)
			{
				initPos.x = -kLocalInitPosX;
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
		m_stopCorrectTime = 0;
	}

	//�J�����������ړ�����Ɛݒ肳��Ă�����
	if (m_isFastMove)
	{
		//�������������[�J�����W��ݒ�
		m_localPos.SetLocalPos(m_nextCameraPos);
		//���̃��[�J�����W�Ɍ������x�N�g�����쐬
		MyEngine::Vector3 toNextPos = m_localPos.GetWorldPos() - m_lastCameraPos;
		//�ړ����x
		float speed = toNextPos.Length() / kFastMoveTime;

		//�ő呬�x�𒴂��Ȃ��悤�ɂ���
		speed = std::fmin(speed, kFarCameraMaxMoveSpeed);
		//�ŏ����x���ݒ肵�Ă���
		speed = std::fmax(speed, kFarCameraMinMoveSpeed);

		//�J�������u�Ԉړ��������ɑ����ړ�������
		m_moveVec = toNextPos.Normalize() * speed;

		if (m_moveVec.Length() > toNextPos.Length())
		{
			m_moveVec = toNextPos;
			m_isFastMove = false;
		}

		m_nextCameraPos = m_localPos.ChangeWorldToLocal(m_lastCameraPos + m_moveVec);
	}

	//�h�炷�傫��(��{�̓[��)
	MyEngine::Vector3 shakeVec;

	//�J������h�炷�ݒ肪����Ă�����
	if (m_shakeTime > 0)
	{
		m_shakeTime--;

		//�}�C�i�X�ɂ��Ȃ�悤�Ƀ����_�����烉���_���̑傫���̔������炷
		int shakePowerHalf = static_cast<int>(m_shakePower * 0.5f);

		//�J�����̃^�[�Q�b�g���W��h�炷
		shakeVec.x = static_cast<float>(GetRand(m_shakePower) - shakePowerHalf);
		shakeVec.y = static_cast<float>(GetRand(m_shakePower) - shakePowerHalf);
		shakeVec.z = static_cast<float>(GetRand(m_shakePower) - shakePowerHalf);
	}

	//�^�[�Q�b�g���W�̐ݒ�
	MyEngine::Vector3 targetPos = (m_targetPos - m_localPos.GetCenterPos()) * kCameraTargetPosRate + m_localPos.GetCenterPos();

	//�J�������ɂ₩�ɗh�炷�ݒ肪����Ă�����
	if (m_isSway)
	{
		m_swayTime += kSwaySpeed;

		//�^�[�Q�b�g���W��h�炷
		targetPos.y += sinf(m_swayTime) * kSwayPower;
	}

	//���[�J�����W�̐ݒ�
	m_localPos.SetLocalPos(m_nextCameraPos);
	//�ɂ₩�ɗh�炷�̂���߂�ݒ�𖈃t���[������
	m_isSway = false;

	SetCameraPositionAndTarget_UpVecY(m_localPos.GetWorldPos().CastVECTOR(), (targetPos + shakeVec).CastVECTOR());

	SetLightDirectionHandle(m_lightHandle, (m_localPos.GetWorldPos() - targetPos).Normalize().CastVECTOR());

	m_lastCameraPos = m_localPos.GetWorldPos();
}
