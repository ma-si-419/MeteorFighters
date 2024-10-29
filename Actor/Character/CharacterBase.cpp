#include "CharacterBase.h"
#include "SphereColliderData.h"
#include "LoadCsv.h"
#include "Attack.h"
#include <cassert>
#include "GameManager.h"
#include "LocalPos.h"

namespace
{
	constexpr float kCharacterHeight = 4.5f;
	constexpr float kCharacterRadius = 3.0f;

	constexpr float kAttackPopPos = kCharacterRadius * 2.0f;

	const MyEngine::Vector3 kAttackPos(0.0f, 0.0f, kAttackPopPos);

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
	Actor(tag, ColliderData::Kind::kSphere),
	m_modelHandle(-1),
	m_attachAnim(-1),
	m_totalAnimTime(-1),
	m_nowPlayAnimTime(-1),
	m_animPlaySpeed(0),
	m_isLoop(false),
	m_characterKind(kind),
	m_lastAnim(-1),
	m_animBlendRate(1.0f),
	m_animBlendSpeed(kAnimBlendSpeed),
	m_isEndAnimationBlend(true)
{
	auto sphereData = std::dynamic_pointer_cast<SphereColliderData>(m_pColData);

	sphereData->m_radius = kCharacterRadius;

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

	//アニメーションをけしておく
	if (m_lastAnim != -1)
	{
		MV1DetachAnim(m_modelHandle, m_lastAnim);
	}

	int animNumber = static_cast<int>(animKind);
	m_lastAnim = m_attachAnim;
	m_attachAnim = MV1AttachAnim(m_modelHandle, animNumber);
	m_playAnimKind = animKind;
	m_totalAnimTime = MV1GetAnimTotalTime(m_modelHandle, animNumber);
	m_lastPlayAnimTime = m_nowPlayAnimTime;
	m_nowPlayAnimTime = 0;
	m_animPlaySpeed = 1.0f;
	m_isLoop = loop;
	m_animBlendRate = 0.0f;
	m_animBlendSpeed = blendSpeed;
	m_isEndAnimationBlend = false;
}

void CharacterBase::PlayAnim()
{
	MV1SetAttachAnimTime(m_modelHandle, m_lastAnim, m_lastPlayAnimTime);
	MV1SetAttachAnimTime(m_modelHandle, m_attachAnim, m_nowPlayAnimTime);
	m_nowPlayAnimTime += m_animPlaySpeed;
	if (m_nowPlayAnimTime > m_totalAnimTime)
	{
		if (m_isLoop)
		{
			m_nowPlayAnimTime = 0;
		}
	}

	//ブレンド率の調整
	m_animBlendRate += m_animBlendSpeed;

	if (m_animBlendRate > 1.0f)
	{
		m_animBlendRate = 1.0f;
		m_isEndAnimationBlend = true;
		m_lastPlayAnimTime = 0;
		MV1DetachAnim(m_modelHandle,m_lastAnim);
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
		pushData.attackMoveSpeed = stof(item[static_cast<int>(NormalAttackDataSort::kAttackMoveSpeed)]);
		pushData.isTeleportation = static_cast<bool>(stoi(item[static_cast<int>(NormalAttackDataSort::kIsTeleportation)]));
		pushData.animationName = item[static_cast<int>(NormalAttackDataSort::kAnimationName)];
		pushData.attackKind = static_cast<AttackKind>(stoi(item[static_cast<int>(NormalAttackDataSort::kAttackKind)]));
		pushData.nextComboName = item[static_cast<int>(NormalAttackDataSort::kNextComboName)];
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
	MyEngine::Vector3 targetPos;
	if (attackData.isPlayer)
	{
		targetPos = m_pGameManager->GetEnemyPos();
		toTarget = (targetPos - m_pGameManager->GetPlayerPos()).Normalize();
	}
	else
	{
		targetPos = m_pGameManager->GetPlayerPos();
		toTarget = (targetPos - m_pGameManager->GetEnemyPos()).Normalize();
	}

	localPos.SetFrontPos(m_rigidbody.GetPos() + toTarget);

	//どのくらいずらすかを設定
	localPos.SetLocalPos(kAttackPos);

	//ローカル座標からワールド座標に変換
	attackPos = localPos.GetWorldPos();

	//攻撃のポインタ作成
	ans = std::make_shared<Attack>(tag, attackPos);

	Attack::AttackStatus status;

	status.targetPos = targetPos;
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

void CharacterBase::SetDrawPos(MyEngine::Vector3 pos)
{
	pos.y -= kCharacterHeight;

	MV1SetPosition(m_modelHandle,pos.CastVECTOR());
}

CharacterBase::AnimKind CharacterBase::GetAttackAnimKind(std::string animName)
{
	CharacterBase::AnimKind ans = AnimKind::kLowAttack1;

	if (animName == "LowAttack1")
	{
		ans = CharacterBase::AnimKind::kLowAttack1;
	}
	else if (animName == "LowAttack2")
	{
		ans = CharacterBase::AnimKind::kLowAttack2;
	}
	else if (animName == "LowAttack3")
	{
		ans = CharacterBase::AnimKind::kLowAttack3;
	}
	else if (animName == "LowAttack4")
	{
		ans = CharacterBase::AnimKind::kLowAttack4;
	}
	else if (animName == "LowAttack5")
	{
		ans = CharacterBase::AnimKind::kLowAttack5;
	}
	else if (animName == "LowAttack6")
	{
		ans = CharacterBase::AnimKind::kLowAttack6;
	}
	else if (animName == "LowAttack7")
	{
		ans = CharacterBase::AnimKind::kLowAttack7;
	}
	else if (animName == "LowAttack8")
	{
		ans = CharacterBase::AnimKind::kLowAttack8;
	}
	else if (animName == "EnergyAttackRight")
	{
		ans = CharacterBase::AnimKind::kEnergyAttackRight;
	}
	else if (animName == "EnergyAttackLeft")
	{
		ans = CharacterBase::AnimKind::kEnergyAttackLeft;
	}
	else if (animName == "UpperAttack")
	{
		ans = CharacterBase::AnimKind::kUpperAttack;
	}
	else if (animName == "StanAttack")
	{
		ans = CharacterBase::AnimKind::kStanAttack;
	}
	else if (animName == "LegSweepAttack")
	{
		ans = CharacterBase::AnimKind::kLegSweepAttack;
	}
	return ans;
}
