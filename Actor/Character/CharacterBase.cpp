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

		//一旦情報を保存する変数
		std::string temp;

		//受けた時のやられ状態
		temp = item[static_cast<int>(NormalAttackDataSort::kHitReaction)];

		HitReaction hitReaction;
		if (temp == "弱")
		{
			hitReaction = HitReaction::kRow;
		}
		else if (temp == "中")
		{
			hitReaction = HitReaction::kMid;
		}
		else if (temp == "強")
		{
			hitReaction = HitReaction::kHigh;
		}
		else if (temp == "吹っ飛び")
		{
			hitReaction = HitReaction::kBurst;
		}

		pushData.hitReaction = hitReaction;

		//攻撃を受けた時の吹き飛ぶ方向
		temp = item[static_cast<int>(NormalAttackDataSort::kHitDirection)];

		HitDirection hitDirection;

		if (temp == "奥")
		{
			hitDirection = HitDirection::kFar;
		}
		else if (temp == "上")
		{
			hitDirection = HitDirection::kUp;
		}
		else if (temp == "下")
		{
			hitDirection = HitDirection::kDown;
		}

		pushData.hitDirection = hitDirection;

		//吹き飛ぶ強さ
		temp = item[static_cast<int>(NormalAttackDataSort::kBurstPower)];

		BurstPower burstPower;

		if (temp == "無")
		{
			burstPower = BurstPower::kNone;
		}
		else if (temp == "弱")
		{
			burstPower = BurstPower::kRow;
		}
		else if (temp == "中")
		{
			burstPower = BurstPower::kMid;
		}
		else if (temp == "強")
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
	//アニメーションを再生していたらけしておく
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
	if (kind == "ビーム")
	{
		return SpecialAttackKind::kBeam;
	}
	else if (kind == "格闘")
	{
		return SpecialAttackKind::kRush;
	}
	else if (kind == "気弾")
	{
		return SpecialAttackKind::kEnergy;
	}
	else if (kind == "投げ")
	{
		return SpecialAttackKind::kThrow;
	}
	else if (kind == "体当たり")
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


		//一旦情報を保存する変数
		std::string temp;

		//受けた時のやられ状態
		temp = item[static_cast<int>(NormalAttackDataSort::kHitReaction)];

		HitReaction hitReaction;
		if (temp == "弱")
		{
			hitReaction = HitReaction::kLow;
		}
		else if (temp == "中")
		{
			hitReaction = HitReaction::kMid;
		}
		else if (temp == "強")
		{
			hitReaction = HitReaction::kHigh;
		}
		else if (temp == "吹っ飛び")
		{
			hitReaction = HitReaction::kBurst;
		}

		pushData.hitReaction = hitReaction;

		//攻撃を受けた時の吹き飛ぶ方向
		temp = item[static_cast<int>(NormalAttackDataSort::kHitDirection)];

		HitDirection hitDirection;

		if (temp == "奥")
		{
			hitDirection = HitDirection::kFar;
		}
		else if (temp == "上")
		{
			hitDirection = HitDirection::kUp;
		}
		else if (temp == "下")
		{
			hitDirection = HitDirection::kDown;
		}

		pushData.hitDirection = hitDirection;

		//吹き飛ぶ強さ
		temp = item[static_cast<int>(NormalAttackDataSort::kBurstPower)];

		BurstPower burstPower;

		if (temp == "無")
		{
			burstPower = BurstPower::kNone;
		}
		else if (temp == "弱")
		{
			burstPower = BurstPower::kLow;
		}
		else if (temp == "中")
		{
			burstPower = BurstPower::kMid;
		}
		else if (temp == "強")
		{
			burstPower = BurstPower::kHigh;
		}

		pushData.burstPower = burstPower;


		m_normalAttackData[item[static_cast<int>(NormalAttackDataSort::kAttackName)]] = pushData;
	}
}

std::shared_ptr<Attack> CharacterBase::CreateAttack(AttackData attackData)
{
	//返す値
	std::shared_ptr<Attack> ans;

	//攻撃のタグ
	ObjectTag tag;

	//プレイヤーとエネミーどちらの攻撃か判断
	if (attackData.isPlayer)
	{
		tag = ObjectTag::kPlayerAttack;
	}
	else
	{
		tag = ObjectTag::kEnemyAttack;
	}

	//攻撃のポインタ作成
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
