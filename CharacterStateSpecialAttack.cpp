#include "CharacterStateSpecialAttack.h"
#include "CharacterStateIdle.h"
#include "CharacterBase.h"
#include "Effect.h"
#include "Attack.h"
#include "GameSceneConstant.h"
#include <cassert>
#include <map>
#include <cmath>

namespace
{

	const std::map<CharacterBase::AttackKind, int> kAttackTimeMap =
	{
		{CharacterBase::AttackKind::kEnergy,90},
		{CharacterBase::AttackKind::kPhysical,90},
		{CharacterBase::AttackKind::kBeam,90},
		{CharacterBase::AttackKind::kRush,90},
		{CharacterBase::AttackKind::kThrow,30},
		{CharacterBase::AttackKind::kAssault,90}
	};

	const std::map<CharacterBase::AttackKind, CharacterBase::AttackHitKind> kHitKindMap =
	{
		{CharacterBase::AttackKind::kEnergy,CharacterBase::AttackHitKind::kWeakUpBurst},
		{CharacterBase::AttackKind::kPhysical,CharacterBase::AttackHitKind::kFarBurst},
		{CharacterBase::AttackKind::kBeam,CharacterBase::AttackHitKind::kWeakUpBurst},
		{CharacterBase::AttackKind::kRush,CharacterBase::AttackHitKind::kFarBurst},
		{CharacterBase::AttackKind::kThrow,CharacterBase::AttackHitKind::kFarBurst},
		{CharacterBase::AttackKind::kAssault,CharacterBase::AttackHitKind::kWeakUpBurst}
	};

	const std::map<CharacterBase::AttackKind, float> kAttackSpeedMap =
	{
		{CharacterBase::AttackKind::kEnergy,6.0f},
		{CharacterBase::AttackKind::kPhysical,0.0f},
		{CharacterBase::AttackKind::kBeam,6.0f},
		{CharacterBase::AttackKind::kRush,0.0f},
		{CharacterBase::AttackKind::kThrow,0.0f},
		{CharacterBase::AttackKind::kAssault,0.0f},
	};
}

CharacterStateSpecialAttack::CharacterStateSpecialAttack(std::shared_ptr<CharacterBase> character) :
	CharacterStateBase(character),
	m_specialAttackNumber(-1),
	m_attackTime(0),
	m_endHitReaction(-1),
	m_isAttacked(false)
{

}

void CharacterStateSpecialAttack::Enter()
{
	//必殺技が設定されていなければ止める
	if (m_specialAttackNumber == -1) assert(false);

	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kSpecialAttack;

	//必殺技の情報取得
	auto attackData = m_pCharacter->GetSpecialAttackData(m_specialAttackNumber);

	//アニメーションの種類設定
	CharacterBase::AnimKind anim = CharacterBase::AnimKind::kIdle;

	//一つ目の必殺技を使用する場合
	if (m_specialAttackNumber == 1)
	{
		anim = CharacterBase::AnimKind::kStartFirstSpecialAttack;
	}
	//二つ目の必殺技を使用する場合
	else if (m_specialAttackNumber == 2)
	{
		anim = CharacterBase::AnimKind::kStartSecondSpecialAttack;
	}

	//アニメーション変更
	m_pCharacter->ChangeAnim(anim, false);

	//技の種類を設定する
	auto kind = m_pCharacter->GetSpecialAttackData(m_specialAttackNumber).kind;

	//技を出している時間を設定する
	m_attackTime = kAttackTimeMap.at(kind);

	//敵の方向を向く
	m_pCharacter->LookTarget();

}

void CharacterStateSpecialAttack::Update()
{
	//このStateにいる時間
	m_time++;

	//攻撃の情報
	auto attackData = m_pCharacter->GetSpecialAttackData(m_specialAttackNumber);

	//Stateを絶対に抜ける時間
	int totalFrame = attackData.startFrame + m_attackTime + attackData.endFrame;

	//攻撃をやめてから一定時間たっていたら
	if (m_time > totalFrame)
	{
		//アイドル状態に戻る
		auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		ChangeState(next);
	}

	//攻撃をやめるフレーム
	int attackEndFrame = attackData.startFrame + m_attackTime;

	//必殺技を受けた時の状態と敵の状態が一致したら
	if (m_endHitReaction == static_cast<int>(GetTargetHitReaction()) &&
		m_time < attackEndFrame)
	{
		//攻撃をやめるフレームまで進める
		m_time = attackEndFrame;
	}

	//攻撃をやめるフレームになったら
	if (m_time > attackEndFrame)
	{
		//アニメーションの変更を行う
		if (m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kOnFirstSpecialAttack)
		{
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kEndFirstSpecialAttack, false);
		}
		else if (m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kOnSecondSpecialAttack)
		{
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kEndSecondSpecialAttack, false);
		}
		//エフェクトを止める
		ExitEffect(m_pEffect);
	}
	//エフェクトが設定されていたら
	if (m_pEffect)
	{
		//座標を設定する
		m_pEffect->SetPos(m_pAttack->GetPos());
	}

	//攻撃を出す時間になったら
	if (m_time > attackData.startFrame)
	{
		//攻撃を出していたら通らないようにする
		if (m_isAttacked) return;

		//アニメーションの変更を行う
		if (m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kStartFirstSpecialAttack)
		{
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kOnFirstSpecialAttack, true, 0.15f);
			//変更を行ったら位置フレーム待つ
			return;
		}
		else if (m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kStartSecondSpecialAttack)
		{
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kOnSecondSpecialAttack, true, 0.15f);
			//変更を行ったら位置フレーム待つ
			return;
		}

		//攻撃の設定をする
		CharacterBase::AttackData attack;

		//ダメージの設定
		attack.damage = static_cast<int>(attackData.damageRate * m_pCharacter->GetPower());

		//受けた時のリアクションを設定
		attack.attackHitKind = kHitKindMap.at(attackData.kind);

		//1Pか2Pかの設定
		if (m_pCharacter->GetTag() == ObjectTag::kOnePlayer)
		{
			attack.isPlayer = true;
		}
		else if (m_pCharacter->GetTag() == ObjectTag::kTwoPlayer)
		{
			attack.isPlayer = false;
		}

		//必殺技がキャラクターとぶつかった時にダメージを受けるものであれば
		if (attackData.kind == CharacterBase::AttackKind::kAssault ||
			attackData.kind == CharacterBase::AttackKind::kPhysical ||
			attackData.kind == CharacterBase::AttackKind::kThrow ||
			attackData.kind == CharacterBase::AttackKind::kRush)
		{
			//必殺技の判定をキャラクターの当たり判定よりも少し大きい大きさにする
			attack.radius = GameSceneConstant::kCharacterRadius + 1.0f;
		}
		//気弾系であれば
		else
		{
			//設定された当たり判定の大きさを使用する
			attack.radius = attackData.radius;
		}

		//攻撃の種類を設定
		attack.attackKind = attackData.kind;

		//攻撃の速度を設定
		attack.speed = kAttackSpeedMap.at(attackData.kind);

		//攻撃の生存時間を設定
		attack.lifeTime = kAttackTimeMap.at(attackData.kind);

		//敵の状態が何になったら攻撃をやめるかを設定
		m_endHitReaction = static_cast<int>(kHitKindMap.at(attackData.kind));

		//攻撃を作成
		m_pAttack = m_pCharacter->CreateAttack(attack);

		//エフェクトを作成
		m_pEffect = std::make_shared<Effect>(attackData.path);

		m_pEffect->SetPos(m_pAttack->GetPos());

		//回転を設定
		float vX = m_pAttack->GetPos().x - m_pCharacter->GetPos().x;
		float vZ = m_pAttack->GetPos().z - m_pCharacter->GetPos().z;

		float rotY = std::atan2f(vX, vZ);

		m_pEffect->SetRotation(MyEngine::Vector3(0.0f, rotY, 0.0f));
		EntryEffect(m_pEffect);

		//攻撃を出したと設定
		m_isAttacked = true;
	}

}

void CharacterStateSpecialAttack::Exit()
{

}
