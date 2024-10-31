#include "Attack.h"
#include "Physics.h"
#include "CapsuleColliderData.h"

namespace
{
	constexpr float kEnergyAttackTrackRange = 2.5f;

	constexpr float kAttackLange = 0.5f;
}

Attack::Attack(ObjectTag tag, MyEngine::Vector3 pos) :
	Collidable(tag, ColliderData::Kind::kCapsule),
	m_lifeTime(0),
	m_isExist(true),
	m_isTrack(true)
{
	m_rigidbody.SetPos(pos);
	m_pColData->SetIsTrigger(true);
}

Attack::~Attack()
{
}

void Attack::Init(AttackStatus status)
{
	m_status = status;
	Collidable::Init();
	auto col = std::dynamic_pointer_cast<CapsuleColliderData>(m_pColData);
	col->m_radius = status.radius;
	col->m_lange = (m_status.targetPos - m_rigidbody.GetPos()).Normalize() * kAttackLange;

	m_dir = (m_status.targetPos - m_rigidbody.GetPos()).Normalize();
}

void Attack::Update()
{
	//�����ł̍폜
	if (m_lifeTime > m_status.lifeTime)
	{
		m_isExist = false;
	}

	//�^�[�Q�b�g�̋߂��܂ŗ�����ǔ�����߂�
	if ((m_status.targetPos - m_rigidbody.GetPos()).Length() < kEnergyAttackTrackRange)
	{
		m_isTrack = false;
	}

	//��{�I�ɒǔ�����
	if (m_isTrack)
	{
		m_dir = (m_status.targetPos - m_rigidbody.GetPos()).Normalize();
	}

	//�U���ɑ��x������ΓG�Ɍ������Ĕ��ł���	
	m_rigidbody.SetVelo(m_dir * m_status.speed);

	//�V�[���ɏo�Ă���̃t���[�����𐔂���
	m_lifeTime++;
}

void Attack::Draw()
{
	if (m_isTrack)
	{
		DrawSphere3D(m_rigidbody.GetPos().CastVECTOR(), 2.0f, 4, GetColor(255, 0, 0), GetColor(255, 0, 0), true);
	}
}

void Attack::Final()
{
	Collidable::Final();
}

void Attack::OnCollide(std::shared_ptr<Collidable> collider)
{
	//���g���v���C���[�̍U����
	if (GetTag() == ObjectTag::kPlayerAttack)
	{
		//�G�l�~�[�ɂԂ�������
		if (collider->GetTag() == ObjectTag::kEnemy)
		{
			m_isExist = false;
			//printfDx("�Ԃ�����");
		}
	}
	//���g���G�l�~�[�̍U����
	else if (GetTag() == ObjectTag::kEnemyAttack)
	{
		//�v���C���[�ɂԂ�������
		if (collider->GetTag() == ObjectTag::kPlayer)
		{
			m_isExist = false;
		}
	}

}
