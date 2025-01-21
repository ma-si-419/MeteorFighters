#include "Attack.h"
#include "Physics.h"
#include "CapsuleColliderData.h"
#include "Effect.h"
#include "EffectManager.h"
#include <cmath>

namespace
{
	constexpr float kEnergyAttackTrackRange = 20.0f;

	constexpr float kAttackLange = 0.5f;

	constexpr int kEnergyHitEffectTime = 30;
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

void Attack::Init(AttackStatus status, std::shared_ptr<EffectManager> manager)
{
	m_status = status;
	Collidable::Init();
	auto col = std::dynamic_pointer_cast<CapsuleColliderData>(m_pColData);
	col->m_radius = status.radius;
	col->m_lange = (m_status.targetPos - m_rigidbody.GetPos()).Normalize() * kAttackLange;
	col->m_endPos = m_rigidbody.GetPos() + col->m_lange;

	m_pEffectManager = manager;
	//�G�t�F�N�g���ݒ肳��Ă�����
	if (status.effectName != "None")
	{
		m_pEffect = std::make_shared<Effect>(status.effectName);

		manager->Entry(m_pEffect, m_rigidbody.GetPos());
	}

	m_dir = (m_status.targetPos - m_rigidbody.GetPos()).Normalize();
}

void Attack::Update()
{
	m_lastPos = m_rigidbody.GetPos();

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

	//�G�t�F�N�g�̍��W���X�V
	if (m_pEffect)
	{
		m_pEffect->SetPos(m_rigidbody.GetPos());
	}

	//�V�[���ɏo�Ă���̃t���[�����𐔂���
	m_lifeTime++;
}

void Attack::Draw()
{
}

void Attack::Final()
{
	Collidable::Final();
	if (m_pEffect)
	{
		//���g�̍U���G�t�F�N�g�̍Đ�����߂�
		m_pEffectManager->Exit(m_pEffect);
	}
}

void Attack::OnCollide(std::shared_ptr<Collidable> collider)
{
	//���g���v���C���[�̍U����
	if (GetTag() == ObjectTag::kOnePlayerAttack)
	{
		//�G�l�~�[�ɂԂ�������
		if (collider->GetTag() == ObjectTag::kTwoPlayer)
		{

		}
	}
	//���g���G�l�~�[�̍U����
	else if (GetTag() == ObjectTag::kTwoPlayerAttack)
	{
		//�v���C���[�ɂԂ�������
		if (collider->GetTag() == ObjectTag::kOnePlayer)
		{
		}
	}

}
