#include "Rock.h"
#include "DxLib.h"
#include "SphereColliderData.h"

namespace
{
	//���ł����Ƃ��̏����̍ő呬�x
	constexpr float kMaxSpeed = 20.0f;
	//���ł����Ƃ��̏����̍ŏ����x
	constexpr float kMinSpeed = 10.0f;

	//���ł����Ƃ��̏����̍ő�p�x
	constexpr int kMaxAngle = 120;
	//���ł����Ƃ��̏����̍ŏ��p�x
	constexpr int kMinAngle = 60;
	//�x���@����ʓx�@�֕ϊ�����l
	constexpr float kToRadian = DX_PI_F / 180;

	//�����Ă����܂ł̎���
	constexpr int kAlphaSubStartTime = 180;

	//�����Ă�������
	constexpr int kAlphaSubSpeed = 10;

	//�����蔻��̃T�C�Y
	constexpr float kColliderSize = 1.0f;

	//�d�͂������n�߂鎞��
	constexpr int kGravityStartTime = 15;

	//�d�͂̑傫��
	constexpr float kGravity = 0.5f;

	//������
	constexpr float kDeceleration = 0.9f;

	//�΂̑傫��
	constexpr float kRockSize = 0.2f;

}

Rock::Rock() :
	ObjectBase(ColliderData::Kind::kSphere),
	m_moveVec(0.0f, 0.0f, 0.0f),
	m_time(0)
{
	m_alpha = 255;
	m_isExist = true;
}

Rock::~Rock()
{

}

void Rock::Init()
{
	//�����蔻��̃T�C�Y��������
	auto sphere = std::dynamic_pointer_cast<SphereColliderData>(m_pColData);
	sphere->m_radius = kColliderSize;

	//���f���̃T�C�Y��ύX
	MV1SetScale(m_handle, VGet(kRockSize, kRockSize, kRockSize));

	Collidable::Init();
}

void Rock::Update()
{
	//�����Ă�������
	m_time++;
	//��莞�Ԍo������
	if (m_time > kAlphaSubStartTime)
	{
		//�A���t�@�l�����炷
		m_alpha -= kAlphaSubSpeed;
		//�A���t�@�l��0�ȉ��ɂȂ�����
		if (m_alpha <= 0)
		{
			//����
			m_alpha = 0;
			m_isExist = false;
		}
	}

	//����
	m_moveVec *= kDeceleration;

	//�d�͂�������
	if (m_time > kGravityStartTime)
	{
		m_moveVec.y -= kGravity;
	}

	//�ړ�
	m_rigidbody.SetVelo(m_moveVec);
}

void Rock::SetMoveVec(MyEngine::Vector3 bumpPos, MyEngine::Vector3 characterPos)
{
	//�Ԃ������ꏊ����L�����N�^�[�̍��W�܂ł̃x�N�g�������߂�
	m_moveVec = characterPos - bumpPos;
	//���K��
	m_moveVec = m_moveVec.Normalize();
	//�����_���Ńx�N�g���̊p�x��ς���
	MATRIX randomX = MGetRotX(static_cast<float>((GetRand(kMaxAngle) - kMinAngle) * kToRadian));
	MATRIX randomY = MGetRotY(static_cast<float>((GetRand(kMaxAngle) - kMinAngle) * kToRadian));
	MATRIX mat = MMult(randomX, randomY);
	//�x�N�g����ϊ�
	m_moveVec = m_moveVec.MatTransform(mat);

	//���x�������_���ŕς���
	float speed = GetRand(kMaxSpeed - kMinSpeed) + kMinSpeed;
	m_moveVec *= speed;

	//�����ʒu���Z�b�g
	m_rigidbody.SetPos(bumpPos);
}
