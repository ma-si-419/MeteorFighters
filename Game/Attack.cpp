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

	//格闘攻撃を受けた時に出すエフェクト
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

	//攻撃の種類によって出すエフェクトの種類
	const std::map<Character::AttackKind, Effect::EffectKind> kAttackEffectMap =
	{
		{Character::AttackKind::kPhysical,Effect::EffectKind::kLowHit},//格闘攻撃の場合は上のマップを使う
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
	//エフェクトが設定されていたら
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

	//寿命での削除
	if (m_lifeTime > m_status.lifeTime)
	{
		m_isExist = false;
	}

	//ターゲットの近くまで来たら追尾をやめる
	if ((m_status.targetPos - m_rigidbody.GetPos()).Length() < kEnergyAttackTrackRange)
	{
		m_isTrack = false;
	}

	//基本的に追尾する
	if (m_isTrack)
	{
		m_dir = (m_status.targetPos - m_rigidbody.GetPos()).Normalize();
	}

	//攻撃に速度があれば敵に向かって飛んでいく	
	m_rigidbody.SetVelo(m_dir * m_status.speed);

	//エフェクトの座標を更新
	if (m_pEffect)
	{
		m_pEffect->SetPos(m_rigidbody.GetPos());
	}

	//シーンに出てからのフレーム数を数える
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
		//自身の攻撃エフェクトの再生をやめる
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
	//格闘攻撃の場合
	if (m_status.attackKind == Character::AttackKind::kPhysical)
	{
		return static_cast<int>(kPhysicalAttackHitEffectMap.at(m_status.attackHitKind));
	}
	//それ以外の攻撃の場合
	else
	{
		return static_cast<int>(kAttackEffectMap.at(m_status.attackKind));
	}

	return 0;
}

void Attack::OnCollide(std::shared_ptr<Collidable> collider)
{
	//気弾系の攻撃だったら
	if (m_status.attackKind == Character::AttackKind::kLaser ||
		m_status.attackKind == Character::AttackKind::kEnergy)
	{
		//自身がプレイヤーの攻撃で
		if (GetTag() == ObjectTag::kOnePlayerAttack)
		{
			//エネミーの攻撃にぶつかったら
			if (collider->GetTag() == ObjectTag::kTwoPlayerAttack)
			{
				//相手の攻撃
				auto attack = std::static_pointer_cast<Attack>(collider);

				//自身の攻撃を消すかどうかチェック
				if (IsDelete(attack))
				{
					//この攻撃を消す
					m_isExist = false;

					//ヒットエフェクトを再生する
					auto effect = std::make_shared<Effect>(static_cast<Effect::EffectKind>(GetHitEffeckKind()));

					//座標設定
					effect->SetPos(m_rigidbody.GetPos());

					//エフェクトの再生時間を設定
					effect->SetLifeTime(kHitEffectTime);

					//エフェクトを登録
					m_pEffectManager->Entry(effect, m_rigidbody.GetPos());
				}
			}
		}
		//自身がエネミーの攻撃で
		else if (GetTag() == ObjectTag::kTwoPlayerAttack)
		{
			//プレイヤーの攻撃にぶつかったら
			if (collider->GetTag() == ObjectTag::kOnePlayerAttack)
			{
				//相手の攻撃
				auto attack = std::static_pointer_cast<Attack>(collider);

				//自身の攻撃を消すかどうかチェック
				if (IsDelete(attack))
				{
					//この攻撃を消す
					m_isExist = false;

					//ヒットエフェクトを再生する
					auto effect = std::make_shared<Effect>(static_cast<Effect::EffectKind>(GetHitEffeckKind()));

					//座標設定
					effect->SetPos(m_rigidbody.GetPos());

					//エフェクトの再生時間を設定
					effect->SetLifeTime(kHitEffectTime);

					//エフェクトを登録
					m_pEffectManager->Entry(effect, m_rigidbody.GetPos());
				}
			}
		}
	}

}

bool Attack::IsDelete(std::shared_ptr<Attack> attack)
{
	//自身の攻撃のステータス
	auto attackKind = m_status.attackKind;

	//相手の攻撃のステータス
	auto enemyAttackKind = attack->GetStatus().attackKind;

	//相手の攻撃がエネルギーで自身の攻撃がビームだったら
	if (attackKind == Character::AttackKind::kLaser &&
		enemyAttackKind == Character::AttackKind::kEnergy)
	{
		//消さない
		return false;
	}
	//それ以外の場合はすべて消す
	else
	{
		//消す
		return true;
	}
}