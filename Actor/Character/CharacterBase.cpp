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
	constexpr float kCharacterRadius = 5.0f;

	constexpr float kAttackPopPos = kCharacterRadius * 3.0f;

	constexpr float kAttackMaxShiftY = kCharacterHeight * 0.5f;

	const MyEngine::Vector3 kAttackPos(0.0f, 0.0f, kAttackPopPos);

	constexpr float kAnimBlendSpeed = 0.08f;

	constexpr float kAnimPlaySpeed = 1.0f;

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

	const std::map<std::string, CharacterBase::AnimKind> kAttackAnimKindMap =
	{
		{"LowAttack1",CharacterBase::AnimKind::kLowAttack1},
		{"LowAttack2",CharacterBase::AnimKind::kLowAttack2},
		{"LowAttack3",CharacterBase::AnimKind::kLowAttack3},
		{"LowAttack4",CharacterBase::AnimKind::kLowAttack4},
		{"LowAttack5",CharacterBase::AnimKind::kLowAttack5},
		{"LowAttack6",CharacterBase::AnimKind::kLowAttack6},
		{"LowAttack7",CharacterBase::AnimKind::kLowAttack7},
		{"LowAttack8",CharacterBase::AnimKind::kLowAttack8},
		{"EnergyAttackRight",CharacterBase::AnimKind::kEnergyAttackRight},
		{"EnergyAttackLeft",CharacterBase::AnimKind::kEnergyAttackLeft},
		{"UpperAttack",CharacterBase::AnimKind::kUpperAttack},
		{"StanAttack",CharacterBase::AnimKind::kStanAttack},
		{"LegSweepAttack",CharacterBase::AnimKind::kLegSweepAttack},
		{"UpChargeAttack",CharacterBase::AnimKind::kUpChargeAttack},
		{"MiddleChargeAttack",CharacterBase::AnimKind::kMiddleChargeAttack},
		{"DownChargeAttack",CharacterBase::AnimKind::kDownChargeAttack},
		{"EnergyChargeAttack",CharacterBase::AnimKind::kEnergyChargeAttack}
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
	m_isEndAnimationBlend(true),
	m_isEndAnim(false)
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

	//ひとつ前のアニメーションをけしておく
	if (m_lastAnim != -1)
	{
		MV1DetachAnim(m_modelHandle, m_lastAnim);
	}

	//アニメーション番号を設定
	int animNumber = static_cast<int>(animKind);
	//今再生していたアニメーションを保存する
	m_lastAnim = m_attachAnim;
	//今から再生するアニメーションを設定する
	m_attachAnim = MV1AttachAnim(m_modelHandle, animNumber);
	//今から再生するアニメーションが何かを保存する
	m_playAnimKind = animKind;
	//アニメーションの合計再生時間を設定する
	m_totalAnimTime = MV1GetAnimTotalTime(m_modelHandle, animNumber);
	//さっきまで再生していたアニメーションの再生時間を保存する(ブレンドの際におかしくならないように)
	m_lastPlayAnimTime = m_nowPlayAnimTime;
	//再生時間をリセット
	m_nowPlayAnimTime = 0;
	//アニメーションの再生時間を設定
	m_animPlaySpeed = 1.0f;
	//アニメーションがループするかどうかを設定
	m_isLoop = loop;
	//アニメーションブレンド率のリセット
	m_animBlendRate = 0.0f;
	//アニメーションをブレンドする速さを設定
	m_animBlendSpeed = blendSpeed;
	//ブレンドが終わったかどうかをfalseにする
	m_isEndAnimationBlend = false;
	//アニメーションが終わったフラグをfalseにする
	m_isEndAnim = false;
}

void CharacterBase::PlayAnim()
{
	//アニメーションの再生フレームを設定
	MV1SetAttachAnimTime(m_modelHandle, m_lastAnim, m_lastPlayAnimTime);
	MV1SetAttachAnimTime(m_modelHandle, m_attachAnim, m_nowPlayAnimTime);
	//再生中のアニメーションを進める
	m_nowPlayAnimTime += m_animPlaySpeed;
	//アニメーションが最後まで言った場合
	if (m_nowPlayAnimTime > m_totalAnimTime)
	{
		//ループするなら
		if (m_isLoop)
		{
			//再生時間をリセットする
			m_nowPlayAnimTime = 0;
		}
		//ループしないなら
		else
		{
			//アニメーションが終わったフラグをtrueにする
			m_isEndAnim = true;
		}
	}

	//ブレンド率の調整
	m_animBlendRate += m_animBlendSpeed;

	//アニメーションのブレンドが終わったら
	if (m_animBlendRate > 1.0f)
	{
		//変な値にならないようにする
		m_animBlendRate = 1.0f;
		//アニメーションブレンドが終わったフラグをtrueにする
		m_isEndAnimationBlend = true;
		//ひとつ前のアニメーションを消す
		m_lastPlayAnimTime = 0;
		MV1DetachAnim(m_modelHandle,m_lastAnim);
	}

	//アニメーションのブレンド
	MV1SetAttachAnimBlendRate(m_modelHandle, m_lastAnim, 1.0f - m_animBlendRate);
	MV1SetAttachAnimBlendRate(m_modelHandle, m_attachAnim, m_animBlendRate);
}

void CharacterBase::SetAnimPlaySpeed(float speed)
{
	m_animPlaySpeed = speed;
}

void CharacterBase::SetAnimPlaySpeed()
{
	m_animPlaySpeed = kAnimPlaySpeed;
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
		toTarget = targetPos - m_pGameManager->GetPlayerPos();
	}
	else
	{
		targetPos = m_pGameManager->GetPlayerPos();
		toTarget = targetPos - m_pGameManager->GetEnemyPos();
	}

	localPos.SetFrontPos(m_rigidbody.GetPos() + toTarget.Normalize());

	//どのくらいずらすかを設定
	localPos.SetLocalPos(kAttackPos);

	//ローカル座標からワールド座標に変換
	attackPos = localPos.GetWorldPos();

	//y座標のずれを一定値までは補正
	float shiftY = toTarget.y;

	if (shiftY > kAttackMaxShiftY)
	{
		shiftY = kAttackMaxShiftY;
	}
	else if (shiftY < -kAttackMaxShiftY)
	{
		shiftY = -kAttackMaxShiftY;
	}

	attackPos.y += shiftY;

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
	return kAttackAnimKindMap.at(animName);
}

float CharacterBase::GetRadius()
{
	return kCharacterRadius;
}

void CharacterBase::LookTarget(bool isPlayer)
{
	if (isPlayer)
	{
		MV1SetRotationZYAxis(m_modelHandle, (m_rigidbody.GetPos() - m_pGameManager->GetEnemyPos()).CastVECTOR(), VGet(0.0f, 1.0f, 0.0f), 0.0f);
	}
	else
	{
		MV1SetRotationZYAxis(m_modelHandle, (m_rigidbody.GetPos() - m_pGameManager->GetPlayerPos()).CastVECTOR(), VGet(0.0f, 1.0f, 0.0f), 0.0f);
	}
}
