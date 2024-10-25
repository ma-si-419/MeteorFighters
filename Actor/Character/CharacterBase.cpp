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

	const std::map<std::string, CharacterBase::AttackHitKind> kAttackHitKindMap =
	{
		{"弱",CharacterBase::AttackHitKind::kLow},
		{"中",CharacterBase::AttackHitKind::kMiddle},
		{"奥吹っ飛び",CharacterBase::AttackHitKind::kFarBurst},
		{"上吹っ飛び",CharacterBase::AttackHitKind::kUpBurst},
		{"下吹っ飛び",CharacterBase::AttackHitKind::kDownBurst},
		{"中スタン",CharacterBase::AttackHitKind::kMiddleStan},
		{"下スタン",CharacterBase::AttackHitKind::kBottomStan}
	};

	const std::map<std::string, CharacterBase::AttackKind> kSpecialAttackKindMap =
	{
		{"ビーム",CharacterBase::AttackKind::kBeam},
		{"格闘",CharacterBase::AttackKind::kRush},
		{"気弾",CharacterBase::AttackKind::kEnergy},
		{"投げ",CharacterBase::AttackKind::kThrow},
		{"体当たり",CharacterBase::AttackKind::kAssault}
	};

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
	m_animBlendRate(1.0f),
	m_animBlendSpeed(kAnimBlendSpeed),
	m_isEndAnimationBlend(true)
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

MyEngine::Vector3 CharacterBase::GetVelo()
{
	return m_rigidbody.GetVelo();
}

void CharacterBase::ChangeAnim(AnimKind animKind, bool loop)
{
	ChangeAnim(animKind, loop, kAnimBlendSpeed);
}

void CharacterBase::ChangeAnim(AnimKind animKind, bool loop, float blendSpeed)
{
	//アニメーションを再生していたらけしておく
	if (m_attachAnim != -1)
	{
		MV1DetachAnim(m_modelHandle, m_attachAnim);
	}

	int animNumber = static_cast<int>(animKind);

	m_attachAnim = MV1AttachAnim(m_modelHandle, animNumber);
	m_playAnimKind = animKind;
	m_totalAnimTime = MV1GetAnimTotalTime(m_modelHandle, animNumber);
	m_playAnimTime = 0;
	m_animPlaySpeed = 1.0f;
	m_isLoop = loop;
	m_animBlendRate = 0.0f;
	m_animBlendSpeed = blendSpeed;
	m_isEndAnimationBlend = false;
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
	m_animBlendRate += m_animBlendSpeed;

	if (m_animBlendRate > 1.0f)
	{
		m_animBlendRate = 1.0f;
		m_isEndAnimationBlend = true;
	}

	//アニメーションのブレンド
	MV1SetAttachAnimBlendRate(m_modelHandle, m_lastAnim, 1.0f - m_animBlendRate);
	MV1SetAttachAnimBlendRate(m_modelHandle, m_attachAnim, m_animBlendRate);
}

bool CharacterBase::IsAnimEnd()
{
	return false;
}

CharacterBase::AttackKind CharacterBase::GetSpecialAttackKind(std::string kind)
{
	return kSpecialAttackKindMap.at(kind);
}

void CharacterBase::SetNormalAttackData(std::vector<std::vector<std::string>> normalAttackData)
{
	for (auto& item : normalAttackData)
	{
		NormalAttackData pushData;

		pushData.damageRate = stof(item[static_cast<int>(NormalAttackDataSort::kDamageRate)]);
		pushData.totalFrame = stoi(item[static_cast<int>(NormalAttackDataSort::kTotalFrame)]);
		pushData.attackFrame = stoi(item[static_cast<int>(NormalAttackDataSort::kAttackFrame)]);
		pushData.cancelFrame = stoi(item[static_cast<int>(NormalAttackDataSort::kCancelFrame)]);
		pushData.moveSpeed = stof(item[static_cast<int>(NormalAttackDataSort::kMoveSpeed)]);
		pushData.isTeleportation = static_cast<bool>(stoi(item[static_cast<int>(NormalAttackDataSort::kIsTeleportation)]));
		pushData.animationName = item[static_cast<int>(NormalAttackDataSort::kAnimationName)];
		pushData.attackKind = static_cast<AttackKind>(stoi(item[static_cast<int>(NormalAttackDataSort::kAttackKind)]));
		pushData.nextLowComboName = item[static_cast<int>(NormalAttackDataSort::kLowComboName)];
		pushData.nextHighComboName = item[static_cast<int>(NormalAttackDataSort::kHighComboName)];
		pushData.attackHitKind = kAttackHitKindMap.at(item[static_cast<int>(NormalAttackDataSort::kAttackHitKind)]);

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
	status.attackHitKind = attackData.attackHitKind;
	status.attackKind = attackData.attackKind;

	ans->Init(status);

	m_pGameManager->AddAttack(ans);
}

CharacterBase::NormalAttackData CharacterBase::GetNormalAttackData(std::string attackName)
{
	return m_normalAttackData[attackName];
}

void CharacterBase::SetFrontPos(MyEngine::Vector3 frontPos)
{
	//ローカル座標の前方向を修正
	m_targetLocalPos.SetFrontPos(frontPos);

	//モデルの前方向を修正する
	MV1SetRotationZYAxis(m_modelHandle, (m_rigidbody.GetPos() - frontPos).CastVECTOR(), VGet(0.0f, 1.0f, 0.0f), 0.0f);
}

bool CharacterBase::IsFrontTarget(bool isPlayer)
{

	MyEngine::Vector3 toTargetDir;

	//プレイヤーなら
	if (isPlayer)
	{
		toTargetDir = m_targetLocalPos.ChangeWorldToLocal(m_pGameManager->GetEnemyPos());
	}
	//2P側なら(基本CPU)
	else
	{
		toTargetDir = m_targetLocalPos.ChangeWorldToLocal(m_pGameManager->GetPlayerPos());
	}

	toTargetDir = toTargetDir.Normalize();

	if (toTargetDir.z > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
	
	//警告けしのため
	return true;

}
