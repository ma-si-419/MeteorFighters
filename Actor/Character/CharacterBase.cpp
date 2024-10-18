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
	//アニメーションを再生していたらけしておく
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

	//ブレンド率の調整
	m_animBlendRate += kAnimBlendSpeed;

	if (m_animBlendRate > 1.0f)
	{
		m_animBlendRate = 1.0f;
	}

	//アニメーションのブレンド
	MV1SetAttachAnimBlendRate(m_modelHandle, m_lastAnim, 1.0f - m_animBlendRate);
	MV1SetAttachAnimBlendRate(m_modelHandle, m_attachAnim, m_animBlendRate);
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


		//一旦情報を保存する変数
		std::string temp;

		//受けた時のやられ状態
		temp = item[static_cast<int>(NormalAttackDataSort::kHitReaction)];

		HitReaction hitReaction = HitReaction::kLow;
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

		HitDirection hitDirection = HitDirection::kFar;

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

		BurstPower burstPower = BurstPower::kNone;

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

void CharacterBase::CreateAttack(AttackData attackData)
{
	//攻撃クラス
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
	//攻撃座標の設定
	MyEngine::Vector3 attackPos;

	//キャラクターの正面方向に攻撃を出す
	LocalPos localPos;

	//自身を中心とするローカル座標を作成
	localPos.SetCenterPos(m_rigidbody.GetPos());

	//正面方向を設定
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

	//どのくらいずらすかを設定
	localPos.SetLocalPos(kAttackPos);

	//ローカル座標からワールド座標に変換
	attackPos = localPos.GetWorldPos();

	//攻撃のポインタ作成
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
