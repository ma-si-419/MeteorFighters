#include "CharacterStateSpecialAttack.h"
#include "CharacterStateIdle.h"
#include "Character.h"
#include "Effect.h"
#include "Attack.h"
#include "GameSceneConstant.h"
#include "TutorialManager.h"
#include <cassert>
#include <map>
#include <cmath>
#include "SoundManager.h"

namespace
{
	//最低何フレーム攻撃を行うか
	constexpr int kMinAttackTime = 60;

	//開始時のエフェクトの再生時間
	constexpr int kStartEffectLifeTime = 60;

	//攻撃のエフェクトを残す時間
	constexpr int kAttackEffectLifeTime = 40;

	const std::map<Character::AttackKind, int> kAttackTimeMap =
	{
		{Character::AttackKind::kEnergy,90},
		{Character::AttackKind::kPhysical,90},
		{Character::AttackKind::kLaser,90},
		{Character::AttackKind::kRush,90},
		{Character::AttackKind::kThrow,30},
		{Character::AttackKind::kAssault,90}
	};

	const std::map<Character::AttackKind, Character::AttackHitKind> kHitKindMap =
	{
		{Character::AttackKind::kEnergy,Character::AttackHitKind::kWeakUpBurst},
		{Character::AttackKind::kPhysical,Character::AttackHitKind::kFarBurst},
		{Character::AttackKind::kLaser,Character::AttackHitKind::kFarBurst},
		{Character::AttackKind::kRush,Character::AttackHitKind::kFarBurst},
		{Character::AttackKind::kThrow,Character::AttackHitKind::kFarBurst},
		{Character::AttackKind::kAssault,Character::AttackHitKind::kWeakUpBurst}
	};

	const std::map<Character::AttackHitKind, Character::HitReactionKind> kHitReactionMap =
	{
		{Character::AttackHitKind::kLow,Character::HitReactionKind::kLow},
		{Character::AttackHitKind::kMiddle,Character::HitReactionKind::kMiddle},
		{Character::AttackHitKind::kWeakUpBurst,Character::HitReactionKind::kWeakUpBurst},
		{Character::AttackHitKind::kUpBurst,Character::HitReactionKind::kUpBurst},
		{Character::AttackHitKind::kDownBurst,Character::HitReactionKind::kDownBurst},
		{Character::AttackHitKind::kFarBurst,Character::HitReactionKind::kFarBurst},
		{Character::AttackHitKind::kBottomStan,Character::HitReactionKind::kBottomStan},
		{Character::AttackHitKind::kMiddleStan,Character::HitReactionKind::kMiddleStan}
	};

	const std::map<Character::AttackKind, float> kAttackSpeedMap =
	{
		{Character::AttackKind::kEnergy,6.0f},
		{Character::AttackKind::kPhysical,0.0f},
		{Character::AttackKind::kLaser,20.0f},
		{Character::AttackKind::kRush,0.0f},
		{Character::AttackKind::kThrow,0.0f},
		{Character::AttackKind::kAssault,0.0f}
	};
}

CharacterStateSpecialAttack::CharacterStateSpecialAttack(std::shared_ptr<Character> character) :
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
	Character::AnimKind anim = Character::AnimKind::kIdle;

	//一つ目の必殺技を使用する場合
	if (m_specialAttackNumber == 1)
	{
		anim = Character::AnimKind::kStartFirstSpecialAttack;
	}
	//二つ目の必殺技を使用する場合
	else if (m_specialAttackNumber == 2)
	{
		anim = Character::AnimKind::kStartSecondSpecialAttack;
	}

	//アニメーション変更
	m_pCharacter->ChangeAnim(anim, false);

	//技の種類を設定する
	auto kind = m_pCharacter->GetSpecialAttackData(m_specialAttackNumber).kind;

	//技を出している時間を設定する
	m_attackTime = kAttackTimeMap.at(kind);

	//敵の方向を向く
	m_pCharacter->LookTarget();

	//ガードの状態をスーパーアーマーにする
	m_guardKind = CharacterGuardKind::kSuperArmor;

	//必殺技を発動するエフェクトを再生する
	auto effect = std::make_shared<Effect>(Effect::EffectKind::kStartLaser);

	//エフェクトの再生時間を設定する
	effect->SetLifeTime(kStartEffectLifeTime);

	//エフェクトの座標を設定する
	effect->SetPos(m_pCharacter->GetPos());

	//エフェクトを登録する
	m_pManager->EntryEffect(effect);

	//音声を再生する
	m_pCharacter->PlayVoice(Character::VoiceKind::kSpecialAttack);

	//必殺技発動のSeを再生する
	SoundManager::GetInstance().PlayOnceSound("SpecialStart");
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
		//多分とおらないと思うけど一応書いておく

		//アイドル状態に戻る
		auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		ChangeState(next);
	}

	//攻撃をやめるフレーム
	float attackEndFrame = static_cast<float>(attackData.startFrame + m_attackTime);

	//必殺技を受けた時の状態と敵の状態が一致したら
	if (m_endHitReaction == static_cast<int>(m_pManager->GetTargetHitReaction(m_pCharacter)) &&
		m_time < attackEndFrame && m_time > kMinAttackTime)
	{
		//攻撃をやめるフレームまで進める
		m_time = attackEndFrame;
	}

	//攻撃をやめるフレームになったら
	if (m_time > attackEndFrame)
	{
		//アニメーションの変更を行う
		if (m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kOnFirstSpecialAttack)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kEndFirstSpecialAttack, false);

			//アイドル状態に戻る
			auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

			//動けない時間を設定する
			next->SetEndAnim(static_cast<int>(Character::AnimKind::kEndFirstSpecialAttack), totalFrame - attackEndFrame);

			ChangeState(next);

		}
		else if (m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kOnSecondSpecialAttack)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kEndSecondSpecialAttack, false);

			//アイドル状態に戻る
			auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

			//動けない時間を設定する
			next->SetEndAnim(static_cast<int>(Character::AnimKind::kEndSecondSpecialAttack), totalFrame - attackEndFrame);

			ChangeState(next);
		}
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
		if (m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kStartFirstSpecialAttack)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kOnFirstSpecialAttack, true, 0.15f);
			//変更を行ったら1フレーム待つ
			return;
		}
		else if (m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kStartSecondSpecialAttack)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kOnSecondSpecialAttack, true, 0.15f);
			//変更を行ったら1フレーム待つ
			return;
		}

		//攻撃の設定をする
		Character::AttackData attack;

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
		if (attackData.kind == Character::AttackKind::kAssault ||
			attackData.kind == Character::AttackKind::kPhysical ||
			attackData.kind == Character::AttackKind::kThrow ||
			attackData.kind == Character::AttackKind::kRush)
		{
			//必殺技の判定をキャラクターの当たり判定よりも少し大きい大きさにする
			attack.radius = GameSceneConstant::kCharacterRadius + 1.0f;
		}
		//気弾系であれば
		else
		{
			//設定された当たり判定の大きさを使用する
			attack.radius = attackData.radius;

			//攻撃のサウンドを再生する
			SoundManager::GetInstance().PlayOnceSound("Laser");
		}

		//攻撃の種類を設定
		attack.attackKind = attackData.kind;

		//攻撃の速度を設定
		attack.speed = kAttackSpeedMap.at(attackData.kind);

		//攻撃の生存時間を設定
		attack.lifeTime = kAttackTimeMap.at(attackData.kind);

		//攻撃のヒットサウンドを設定
		attack.hitSoundName = "SpecialHit";

		//敵の状態が何になったら攻撃をやめるかを設定
		m_endHitReaction = static_cast<int>(kHitReactionMap.at(kHitKindMap.at(attackData.kind)));

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
		m_pManager->EntryEffect(m_pEffect);

		//攻撃を出したと設定
		m_isAttacked = true;
	}
	//攻撃を出す時間まで敵の方を向く
	else
	{
		m_pCharacter->LookTarget();
	}

}

void CharacterStateSpecialAttack::Exit()
{
	//エフェクトが設定されていたら
	if (m_pEffect)
	{
		//攻撃のエフェクトを少しだけ残す
		m_pEffect->SetLifeTime(kAttackEffectLifeTime);
	}
	//ガードの状態を通常に戻す
	m_guardKind = CharacterGuardKind::kNone;

	//必殺技発動のチュートリアルをクリアさせる
	SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kSpecialAttack));
}