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

	constexpr int kHitEffectTime = 50;

	//�i���U�����󂯂����ɏo���G�t�F�N�g
	const std::map<Character::AttackHitKind, Effect::EffectKind> kPhysicalAttackHitEffectMap =
	{
		{Character::AttackHitKind::kLow,Effect::EffectKind::kLowHit},
		{Character::AttackHitKind::kMiddle,Effect::EffectKind::kMiddleHit},
		{Character::AttackHitKind::kWeakUpBurst,Effect::EffectKind::kMiddleHit},
		{Character::AttackHitKind::kUpBurst,Effect::EffectKind::kHighHit},
		{Character::AttackHitKind::kDownBurst,Effect::EffectKind::kHighHit},
		{Character::AttackHitKind::kFarBurst,Effect::EffectKind::kHighHit},
		{Character::AttackHitKind::kBottomStan,Effect::EffectKind::kMiddleHit},
		{Character::AttackHitKind::kMiddleStan,Effect::EffectKind::kMiddleHit}
	};

	//�U���̎�ނɂ���ďo���G�t�F�N�g�̎��
	const std::map<Character::AttackKind, Effect::EffectKind> kAttackEffectMap =
	{
		{Character::AttackKind::kPhysical,Effect::EffectKind::kLowHit},//�i���U���̏ꍇ�͏�̃}�b�v���g��
		{Character::AttackKind::kEnergy,Effect::EffectKind::kEnergyHit},
		{Character::AttackKind::kLaser,Effect::EffectKind::kLaserHit}
	};

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

void Attack::StopEffect()
{
	m_pEffectManager->Exit(m_pEffect);
}

void Attack::SetEffectLifeTime(int time)
{
	m_pEffect->SetLifeTime(time);
}

int Attack::GetHitEffeckKind()
{
	//�i���U���̏ꍇ
	if (m_status.attackKind == Character::AttackKind::kPhysical)
	{
		return static_cast<int>(kPhysicalAttackHitEffectMap.at(m_status.attackHitKind));
	}
	//����ȊO�̍U���̏ꍇ
	else
	{
		return static_cast<int>(kAttackEffectMap.at(m_status.attackKind));
	}

	return 0;
}

void Attack::OnCollide(std::shared_ptr<Collidable> collider)
{
	//�C�e�n�̍U����������
	if (m_status.attackKind == Character::AttackKind::kLaser ||
		m_status.attackKind == Character::AttackKind::kEnergy)
	{
		//���g���v���C���[�̍U����
		if (GetTag() == ObjectTag::kOnePlayerAttack)
		{
			//�G�l�~�[�̍U���ɂԂ�������
			if (collider->GetTag() == ObjectTag::kTwoPlayerAttack)
			{
				//����̍U��
				auto attack = std::static_pointer_cast<Attack>(collider);

				//���g�̍U�����������ǂ����`�F�b�N
				if (IsDelete(attack))
				{
					//���̍U��������
					m_isExist = false;

					//�q�b�g�G�t�F�N�g���Đ�����
					auto effect = std::make_shared<Effect>(static_cast<Effect::EffectKind>(GetHitEffeckKind()));

					//���W�ݒ�
					effect->SetPos(m_rigidbody.GetPos());

					//�G�t�F�N�g�̍Đ����Ԃ�ݒ�
					effect->SetLifeTime(kHitEffectTime);

					//�G�t�F�N�g��o�^
					m_pEffectManager->Entry(effect, m_rigidbody.GetPos());
				}
			}
		}
		//���g���G�l�~�[�̍U����
		else if (GetTag() == ObjectTag::kTwoPlayerAttack)
		{
			//�v���C���[�̍U���ɂԂ�������
			if (collider->GetTag() == ObjectTag::kOnePlayerAttack)
			{
				//����̍U��
				auto attack = std::static_pointer_cast<Attack>(collider);

				//���g�̍U�����������ǂ����`�F�b�N
				if (IsDelete(attack))
				{
					//���̍U��������
					m_isExist = false;

					//�q�b�g�G�t�F�N�g���Đ�����
					auto effect = std::make_shared<Effect>(static_cast<Effect::EffectKind>(GetHitEffeckKind()));

					//���W�ݒ�
					effect->SetPos(m_rigidbody.GetPos());

					//�G�t�F�N�g�̍Đ����Ԃ�ݒ�
					effect->SetLifeTime(kHitEffectTime);

					//�G�t�F�N�g��o�^
					m_pEffectManager->Entry(effect, m_rigidbody.GetPos());
				}
			}
		}
	}

}

bool Attack::IsDelete(std::shared_ptr<Attack> attack)
{
	//���g�̍U���̃X�e�[�^�X
	auto attackKind = m_status.attackKind;

	//����̍U���̃X�e�[�^�X
	auto enemyAttackKind = attack->GetStatus().attackKind;

	//����̍U�����G�l���M�[�Ŏ��g�̍U�����r�[����������
	if (attackKind == Character::AttackKind::kLaser &&
		enemyAttackKind == Character::AttackKind::kEnergy)
	{
		//�����Ȃ�
		return false;
	}
	//����ȊO�̏ꍇ�͂��ׂď���
	else
	{
		//����
		return true;
	}
}