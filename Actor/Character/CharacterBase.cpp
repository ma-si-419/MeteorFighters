#include "CharacterBase.h"
#include "CapsuleColliderData.h"
#include "LoadCsv.h"
#include "Attack.h"
#include <cassert>
#include "GameManager.h"
#include "LocalPos.h"

namespace
{
	constexpr float kCharacterHeight = 4.5f;
	constexpr float kCharacterRadius = 3.0f;

	constexpr float kAttackPopPos = kCharacterRadius * 3.0f;

	const MyEngine::Vector3 kAttackPos(0.0f, kCharacterHeight, kAttackPopPos);

	constexpr float kAnimBlendSpeed = 0.08f;
}

CharacterBase::CharacterBase(ObjectTag tag, CharacterKind kind) :
	Actor(tag, ColliderData::Kind::kCapsule),
	m_modelHandle(-1),
	m_attachAnim(-1),
	m_totalAnimTime(-1),
	m_playAnimTime(-1),
	m_animPlaySpeed(0),
	m_isLoop(false),
	m_characterKind(kind),
	m_lastAnim(-1),
	m_animBlendRate(1.0f)
{
	auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(m_pColData);

	capsuleData->m_radius = kCharacterRadius;

	capsuleData->m_length = kCharacterHeight;

	capsuleData->m_isMoveStartPos = true;

	LoadCsv load;

	std::vector<std::vector<std::string>> data = load.LoadFile("data/csv/normalAttackData.csv");

	SetNormalAttackData(data);

}

CharacterBase::~CharacterBase()
{
}

void CharacterBase::SetGameManager(std::shared_ptr<GameManager> manager)
{
	m_pGameManager = manager;
}

MyEngine::Vector3 CharacterBase::GetPos()
{
	return m_rigidbody.GetPos();
}


void CharacterBase::ChangeAnim(AnimKind animKind, bool loop)
{
	//�A�j���[�V�������Đ����Ă����炯���Ă���
	if (m_attachAnim != -1)
	{
		MV1DetachAnim(m_modelHandle, m_attachAnim);
	}

	int animNumber = static_cast<int>(animKind);

	printfDx("%d\n", animNumber);
	m_attachAnim = MV1AttachAnim(m_modelHandle, animNumber);
	m_totalAnimTime = MV1GetAnimTotalTime(m_modelHandle, animNumber);
	m_playAnimTime = 0;
	m_animPlaySpeed = 1.0f;
	m_isLoop = loop;
	m_animBlendRate = 0.0f;
}

void CharacterBase::PlayAnim()
{
	MV1SetAttachAnimTime(m_modelHandle, m_lastAnim, m_playAnimTime);
	MV1SetAttachAnimTime(m_modelHandle, m_attachAnim, m_playAnimTime);
	m_playAnimTime += m_animPlaySpeed;
	if (m_playAnimTime > m_totalAnimTime)
	{
		if (m_isLoop)
		{
			m_playAnimTime = 0;
		}
	}

	//�u�����h���̒���
	m_animBlendRate += kAnimBlendSpeed;

	if (m_animBlendRate > 1.0f)
	{
		m_animBlendRate = 1.0f;
	}

	//�A�j���[�V�����̃u�����h
	MV1SetAttachAnimBlendRate(m_modelHandle, m_lastAnim, 1.0f - m_animBlendRate);
	MV1SetAttachAnimBlendRate(m_modelHandle, m_attachAnim, m_animBlendRate);
}

bool CharacterBase::IsGetAnimEnd()
{
	return false;
}

CharacterBase::SpecialAttackKind CharacterBase::GetSpecialAttackKind(std::string kind)
{
	if (kind == "�r�[��")
	{
		return SpecialAttackKind::kBeam;
	}
	else if (kind == "�i��")
	{
		return SpecialAttackKind::kRush;
	}
	else if (kind == "�C�e")
	{
		return SpecialAttackKind::kEnergy;
	}
	else if (kind == "����")
	{
		return SpecialAttackKind::kThrow;
	}
	else if (kind == "�̓�����")
	{
		return SpecialAttackKind::kAssault;
	}
	else
	{
		assert(false);

		return SpecialAttackKind::kBeam;
	}
}

void CharacterBase::SetNormalAttackData(std::vector<std::vector<std::string>> normalAttackData)
{
	for (auto item : normalAttackData)
	{
		NormalAttackData pushData;

		pushData.damageRate = stof(item[static_cast<int>(NormalAttackDataSort::kDamageRate)]);
		pushData.totalFrame = stoi(item[static_cast<int>(NormalAttackDataSort::kTotalFrame)]);
		pushData.attackFrame = stoi(item[static_cast<int>(NormalAttackDataSort::kAttackFrame)]);
		pushData.cancelFrame = stoi(item[static_cast<int>(NormalAttackDataSort::kCancelFrame)]);
		pushData.animationName = item[static_cast<int>(NormalAttackDataSort::kAnimationName)];
		pushData.nextLowComboName = item[static_cast<int>(NormalAttackDataSort::kLowComboName)];
		pushData.nextHighComboName = item[static_cast<int>(NormalAttackDataSort::kHighComboName)];


		//��U����ۑ�����ϐ�
		std::string temp;

		//�󂯂����̂�����
		temp = item[static_cast<int>(NormalAttackDataSort::kHitReaction)];

		HitReaction hitReaction = HitReaction::kLow;
		if (temp == "��")
		{
			hitReaction = HitReaction::kLow;
		}
		else if (temp == "��")
		{
			hitReaction = HitReaction::kMid;
		}
		else if (temp == "��")
		{
			hitReaction = HitReaction::kHigh;
		}
		else if (temp == "�������")
		{
			hitReaction = HitReaction::kBurst;
		}

		pushData.hitReaction = hitReaction;

		//�U�����󂯂����̐�����ԕ���
		temp = item[static_cast<int>(NormalAttackDataSort::kHitDirection)];

		HitDirection hitDirection = HitDirection::kFar;

		if (temp == "��")
		{
			hitDirection = HitDirection::kFar;
		}
		else if (temp == "��")
		{
			hitDirection = HitDirection::kUp;
		}
		else if (temp == "��")
		{
			hitDirection = HitDirection::kDown;
		}

		pushData.hitDirection = hitDirection;

		//������ԋ���
		temp = item[static_cast<int>(NormalAttackDataSort::kBurstPower)];

		BurstPower burstPower = BurstPower::kNone;

		if (temp == "��")
		{
			burstPower = BurstPower::kNone;
		}
		else if (temp == "��")
		{
			burstPower = BurstPower::kLow;
		}
		else if (temp == "��")
		{
			burstPower = BurstPower::kMid;
		}
		else if (temp == "��")
		{
			burstPower = BurstPower::kHigh;
		}

		pushData.burstPower = burstPower;


		m_normalAttackData[item[static_cast<int>(NormalAttackDataSort::kAttackName)]] = pushData;
	}
}

void CharacterBase::CreateAttack(AttackData attackData)
{
	//�U���N���X
	std::shared_ptr<Attack> ans;

	//�U���̃^�O
	ObjectTag tag;

	//�v���C���[�ƃG�l�~�[�ǂ���̍U�������f
	if (attackData.isPlayer)
	{
		tag = ObjectTag::kPlayerAttack;
	}
	else
	{
		tag = ObjectTag::kEnemyAttack;
	}
	//�U�����W�̐ݒ�
	MyEngine::Vector3 attackPos;

	//�L�����N�^�[�̐��ʕ����ɍU�����o��
	LocalPos localPos;

	//���g�𒆐S�Ƃ��郍�[�J�����W���쐬
	localPos.SetCenterPos(m_rigidbody.GetPos());

	//���ʕ�����ݒ�
	MyEngine::Vector3 toTarget;
	if (attackData.isPlayer)
	{
		toTarget = (m_pGameManager->GetEnemyPos() - m_pGameManager->GetPlayerPos()).Normalize();
	}
	else
	{
		toTarget = (m_pGameManager->GetPlayerPos() - m_pGameManager->GetEnemyPos()).Normalize();
	}

	localPos.SetFrontPos(m_rigidbody.GetPos() + toTarget);

	//�ǂ̂��炢���炷����ݒ�
	localPos.SetLocalPos(kAttackPos);

	//���[�J�����W���烏�[���h���W�ɕϊ�
	attackPos = localPos.GetWorldPos();

	//�U���̃|�C���^�쐬
	ans = std::make_shared<Attack>(tag, attackPos);

	Attack::AttackStatus status;

	status.damage = attackData.damage;
	status.speed = attackData.speed;
	status.lifeTime = attackData.lifeTime;
	status.radius = attackData.radius;
	status.hitDirection = attackData.hitDirection;
	status.hitReaction = attackData.hitReaction;
	status.burstPower = attackData.burstPower;

	ans->Init(status);

	m_pGameManager->AddAttack(ans);
}

CharacterBase::NormalAttackData CharacterBase::GetNormalAttackData(std::string attackName)
{
	return m_normalAttackData[attackName];
}
