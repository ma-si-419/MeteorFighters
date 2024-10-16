#include "CharacterBase.h"
#include "CapsuleColliderData.h"
#include "LoadCsv.h"
#include "Attack.h"

namespace
{
	constexpr float kCharacterHeight = 4.5f;
	constexpr float kCharacterRadius = 3.0f;
}

CharacterBase::CharacterBase(ObjectTag tag,CharacterKind kind) :
	Actor(tag, ColliderData::Kind::kCapsule),
	m_modelHandle(-1),
	m_attachAnim(-1),
	m_totalAnimTime(-1),
	m_playAnimTime(-1),
	m_animPlaySpeed(0),
	m_isLoop(false),
	m_characterKind(kind)
{
	auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(m_pColData);

	capsuleData->m_radius = kCharacterRadius;

	capsuleData->m_length = kCharacterHeight;

	capsuleData->m_isMoveStartPos = true;

	LoadCsv load;

	std::vector<std::vector<std::string>> data = load.LoadFile("data/csv/normalAttackData.csv");

	for (auto item : data)
	{
		NormalAttackData pushData;

		pushData.damageRate = stof(item[static_cast<int>(NormalAttackDataSort::kDamageRate)]);
		pushData.totalFrame = stoi(item[static_cast<int>(NormalAttackDataSort::kTotalFrame)]);
		pushData.attackFrame = stoi(item[static_cast<int>(NormalAttackDataSort::kAttackFrame)]);
		pushData.animationName = item[static_cast<int>(NormalAttackDataSort::kAttackFrame)];

		//��U����ۑ�����ϐ�
		std::string temp;

		//�󂯂����̂�����
		temp = item[static_cast<int>(NormalAttackDataSort::kHitReaction)];

		HitReaction hitReaction;
		if (temp == "��")
		{
			hitReaction = HitReaction::kRow;
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

		HitDirection hitDirection;

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

		BurstPower burstPower;

		if (temp == "��")
		{
			burstPower = BurstPower::kNone;
		}
		else if (temp == "��")
		{
			burstPower = BurstPower::kRow;
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


		m_normalAttackData.push_back(pushData);

	}

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


void CharacterBase::ChangeAnim(int animNumber, bool loop)
{
	//�A�j���[�V�������Đ����Ă����炯���Ă���
	if (m_attachAnim != -1)
	{
		MV1DetachAnim(m_modelHandle, m_attachAnim);
	}

	m_attachAnim = MV1AttachAnim(m_modelHandle, animNumber);
	m_totalAnimTime = MV1GetAnimTotalTime(m_modelHandle, animNumber);
	m_playAnimTime = 0;
	m_animPlaySpeed = 1.0f;
	m_isLoop = loop;
}

void CharacterBase::PlayAnim()
{
	MV1SetAttachAnimTime(m_modelHandle, m_attachAnim, m_playAnimTime);
	m_playAnimTime += m_animPlaySpeed;
	if (m_playAnimTime > m_totalAnimTime)
	{
		if (m_isLoop)
		{
			m_playAnimTime = 0;
		}
		else
		{

		}
	}
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
}

void CharacterBase::SetNormalAttackData(std::vector<std::vector<std::string>> normalAttackData)
{
	for (auto item : normalAttackData)
	{
		NormalAttackData pushData;

		pushData.damageRate = stof(item[static_cast<int>(NormalAttackDataSort::kDamageRate)]);
		pushData.totalFrame = stoi(item[static_cast<int>(NormalAttackDataSort::kTotalFrame)]);
		pushData.attackFrame = stoi(item[static_cast<int>(NormalAttackDataSort::kAttackFrame)]);
		pushData.animationName = item[static_cast<int>(NormalAttackDataSort::kAttackFrame)];
		pushData.nextLowComboName = item[static_cast<int>(NormalAttackDataSort::kLowComboName)];
		pushData.nextHighComboName = item[static_cast<int>(NormalAttackDataSort::kHighComboName)];


		//��U����ۑ�����ϐ�
		std::string temp;

		//�󂯂����̂�����
		temp = item[static_cast<int>(NormalAttackDataSort::kHitReaction)];

		HitReaction hitReaction;
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

		HitDirection hitDirection;

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

		BurstPower burstPower;

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

std::shared_ptr<Attack> CharacterBase::CreateAttack(AttackData attackData)
{
	//�Ԃ��l
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

	//�U���̃|�C���^�쐬
	ans = std::make_shared<Attack>(tag,attackData.pos);

	Attack::AttackStatus status;

	status.damage = attackData.damage;
	status.speed = attackData.speed;
	status.lifeTime = attackData.lifeTime;
	status.hitDirection = attackData.hitDirection;
	status.hitReaction = attackData.hitReaction;
	status.burstPower = attackData.burstPower;

	ans->Init(status);


	return ans;
}

CharacterBase::NormalAttackData CharacterBase::GetNormalAttackData(std::string attackName)
{
	return m_normalAttackData[attackName];
}
