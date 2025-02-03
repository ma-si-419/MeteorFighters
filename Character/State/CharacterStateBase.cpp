#include "CharacterStateBase.h"
#include "CharacterStateHitAttack.h"
#include "CharacterStateGuard.h"
#include "CharacterStateButtonBashing.h"
#include "CharacterStateTeleportation.h"
#include "Character.h"
#include "GameManagerBase.h"
#include "TutorialManager.h"
#include "Attack.h"
#include "Input.h"
#include "Effect.h"
#include "EffectManager.h"
#include <cmath>
#include "GameSceneConstant.h"
#include "LocalPos.h"
#include "SoundManager.h"

namespace
{
	//エネルギー攻撃をガードした時のダメージカット率
	constexpr float kDamageCutRate = 0.2f;

	//攻撃を受けた時に遷移する状態
	const std::map<Character::AttackHitKind, Character::HitReactionKind> kHitKindMap =
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

	//ヒットエフェクトを残す時間
	constexpr int kHitEffectLifeTime = 180;

	//ガードできる可能性がある状態
	const std::vector<Character::HitReactionKind> kCanGuardHitReactionKind =
	{
		Character::HitReactionKind::kGuard,
		Character::HitReactionKind::kLow,
		Character::HitReactionKind::kMiddle,
		Character::HitReactionKind::kNone
	};

	//瞬間移動にかける時間
	constexpr int kTeleportationTime = 12;

	//敵の背後に瞬間移動するときの敵との距離
	constexpr float kTeleportationDistance = GameSceneConstant::kCharacterRadius + 5.0f;

	//攻撃を受けた時の硬直時間
	const std::map<Character::HitReactionKind, int> kDownTimeMap =
	{
		{Character::HitReactionKind::kGuard,10},
		{Character::HitReactionKind::kLow,40},
		{Character::HitReactionKind::kMiddle,40},
		{Character::HitReactionKind::kWeakUpBurst,30},
		{Character::HitReactionKind::kUpBurst,110},
		{Character::HitReactionKind::kFarBurst,110},
		{Character::HitReactionKind::kDownBurst,110},
		{Character::HitReactionKind::kWeakBurst,30},
		{Character::HitReactionKind::kMiddleStan,60},
		{Character::HitReactionKind::kBottomStan,60}
	};

	//吹き飛ばす攻撃を当てた時のカメラを揺らす時間
	constexpr int kHighShakeCameraTime = 10;

	//吹き飛ばす攻撃を当てた時のカメラを揺らす大きさ
	constexpr int kHighShakeCameraPower = 10;

	//中くらいの攻撃を当てた時のカメラを揺らす時間
	constexpr int kMiddleShakeCameraTime = 5;

	//中くらいの攻撃を当てた時のカメラを揺らす大きさ
	constexpr int kMiddleShakeCameraPower = 5;
}

CharacterStateBase::CharacterStateBase(std::shared_ptr<Character> character) :
	m_pCharacter(character),
	m_pManager(m_pCharacter->m_pBattleManager),
	m_pNextState(nullptr),
	m_kind(CharacterStateKind::kIdle),
	m_time(0.0f),
	m_guardKind(CharacterGuardKind::kNone),
	m_stopTime(0)
{
}

void CharacterStateBase::OnCollide(std::shared_ptr<Collidable> collider)
{
	//1P側の処理
	if (m_pCharacter->GetTag() == ObjectTag::kOnePlayer)
	{
		//2Pの攻撃に当たったら
		if (collider->GetTag() == ObjectTag::kTwoPlayerAttack)
		{
			auto attack = std::static_pointer_cast<Attack>(collider);

			auto status = attack->GetStatus();

			CharacterStateBase::HitAttack(attack);
		}
	}
	//2P側の処理
	else if (m_pCharacter->GetTag() == ObjectTag::kTwoPlayer)
	{
		//2Pの攻撃に当たったら
		if (collider->GetTag() == ObjectTag::kOnePlayerAttack)
		{
			auto attack = std::static_pointer_cast<Attack>(collider);

			auto status = attack->GetStatus();

			CharacterStateBase::HitAttack(attack);
		}
	}
}

void CharacterStateBase::UpdateCommon()
{
	if (m_kind != CharacterStateKind::kButtonBashing)
	{
		//相手のStateがボタン連打だったらButtonBashingStateに変更する
		if (static_cast<CharacterStateKind>(m_pManager->GetTargetState(m_pCharacter)) == CharacterStateKind::kButtonBashing)
		{
			auto next = std::make_shared<CharacterStateButtonBashing>(m_pCharacter);

			ChangeState(next);
		}
	}
}

void CharacterStateBase::ChangeState(std::shared_ptr<CharacterStateBase> nextState)
{
	//終了処理
	Exit();

	//次のStateの設定
	m_pNextState = nextState;

	//次のStateの開始処理
	m_pNextState->Enter();
}

std::shared_ptr<MyEngine::InputData> CharacterStateBase::GetCharacterInput()
{
	return m_pCharacter->m_input;
}

void CharacterStateBase::SetCharacterVelo(MyEngine::Vector3 velo)
{
	m_pCharacter->m_rigidbody.SetVelo(velo);
}

void CharacterStateBase::SetCharacterPos(MyEngine::Vector3 pos)
{
	m_pCharacter->m_rigidbody.SetPos(pos);
}

void CharacterStateBase::HitAttack(std::shared_ptr<Attack> attack)
{
	//攻撃のステータス
	auto status = attack->GetStatus();

	int damage = status.damage;

	//次の状態を取得する
	auto hitReaction = static_cast<Character::HitReactionKind>(GetNextHitReactionKind(attack));

	//再生するエフェクト
	Effect::EffectKind effectKind = Effect::EffectKind::kNone;

	//もし次の状態がガードであれば
	if (hitReaction == Character::HitReactionKind::kGuard)
	{
		//気弾攻撃系をダメージカットする
		if (status.attackKind == Character::AttackKind::kLaser ||
			status.attackKind == Character::AttackKind::kEnergy)
		{
			damage = static_cast<int>(damage * kDamageCutRate);
		}
		//打撃系はすべて0ダメージにする(投げは未実装)
		else
		{
			damage = 0;
		}

		//基本的にガードエフェクト
		effectKind = Effect::EffectKind::kGuardHit;

		//ビーム攻撃の場合
		if (status.attackKind == Character::AttackKind::kLaser)
		{
			//ガードに関係なくエフェクトを再生する
			effectKind = Effect::EffectKind::kLaserHit;
		}
	}
	//ガードしていない場合
	else
	{
		//ジャストガードできていた場合
		if (m_guardKind == CharacterGuardKind::kJustGuard)
		{
			//瞬間移動のエフェクトを再生する
			effectKind = Effect::EffectKind::kTeleportaion;
		}
		//回避状態の場合
		else if (m_guardKind == CharacterGuardKind::kDodge)
		{
			//特に何もエフェクトを再生しない			
		}
		else
		{
			//攻撃に合わせたエフェクトを再生する
			effectKind = static_cast<Effect::EffectKind>(static_cast<Effect::EffectKind>(attack->GetHitEffeckKind()));
		}
	}

	//エフェクトが設定されていれば
	if (effectKind != Effect::EffectKind::kNone)
	{
		std::shared_ptr<Effect> hitEffect = std::make_shared<Effect>(effectKind);
		hitEffect->SetLifeTime(kHitEffectLifeTime);
		hitEffect->SetPos(m_pCharacter->GetPos());
		m_pCharacter->m_pBattleManager->GetEffectManagerPointer()->Entry(hitEffect, m_pCharacter->GetPos());

		MyEngine::Vector3 rotation;

		float vX = attack->GetPos().x - m_pCharacter->GetPos().x;
		float vZ = attack->GetPos().z - m_pCharacter->GetPos().z;

		rotation.y = std::atan2f(vX, vZ);

		hitEffect->SetRotation(rotation);
	}

	//回避状態であれば
	if (m_guardKind == CharacterGuardKind::kDodge)
	{
		//ダメージを受けない
		return;
	}

	//ジャストガード時であれば
	if (m_guardKind == CharacterGuardKind::kJustGuard)
	{
		MyEngine::Vector3 moveTargetPos;

		auto nextState = std::make_shared<CharacterStateTeleportation>(m_pCharacter);

		//気弾系の攻撃であれば
		if (attack->GetStatus().attackKind == Character::AttackKind::kEnergy ||
			attack->GetStatus().attackKind == Character::AttackKind::kLaser)
		{
			//移動目標を攻撃の横の座標にする
			MyEngine::LocalPos local;

			local.SetCenterPos(attack->GetPos());

			MyEngine::Vector3 frontPos = (attack->GetStatus().targetPos - attack->GetLastPos()).Normalize() + attack->GetPos();

			local.SetFrontPos(frontPos);

			//ランダムで左右どちらに移動するか決める
			if (GetRand(1))
			{
				local.SetLocalPos(MyEngine::Vector3(attack->GetStatus().radius + kTeleportationDistance, 0.0f, 0.0f));
			}
			else
			{
				local.SetLocalPos(MyEngine::Vector3(-attack->GetStatus().radius - kTeleportationDistance, 0.0f, 0.0f));
			}

			moveTargetPos = local.GetWorldPos();

			//移動先の座標を設定する
			nextState->Init(moveTargetPos, kTeleportationTime);
		}
		//打撃系の攻撃であれば
		else
		{
			//敵の後ろ側に移動する
			nextState->Init(m_pManager->GetTargetBackPos(kTeleportationDistance, m_pCharacter), kTeleportationTime);
		}

		//ジャストガードチュートリアルをクリアさせる
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kJustGuard));

		//瞬間移動状態に遷移する
		ChangeState(nextState);

		return;
	}

	//攻撃を削除する
	attack->DeleteAttack();

	//ダメージを受ける
	m_pCharacter->SubHp(damage);

	//ダメージを受ける際にヒットサウンドを鳴らす
	SoundManager::GetInstance().PlayOnceSound(status.hitSoundName);

	//敵を吹き飛ばす攻撃であれば
	if (hitReaction == Character::HitReactionKind::kUpBurst ||
		hitReaction == Character::HitReactionKind::kDownBurst ||
		hitReaction == Character::HitReactionKind::kFarBurst)
	{
		//カメラを揺らす
		m_pManager->ShakeCamera(kHighShakeCameraTime, kHighShakeCameraPower);
	}
	//中くらいの攻撃であれば
	else if (hitReaction == Character::HitReactionKind::kMiddle ||
		hitReaction == Character::HitReactionKind::kWeakBurst ||
		hitReaction == Character::HitReactionKind::kWeakUpBurst)
	{
		//カメラを揺らす
		m_pManager->ShakeCamera(kMiddleShakeCameraTime, kMiddleShakeCameraPower);
	}

	//スーパーアーマー状態ならば
	if (m_guardKind == CharacterGuardKind::kSuperArmor)
	{
		//やられが弱と中であれば
		if (hitReaction == Character::HitReactionKind::kLow ||
			hitReaction == Character::HitReactionKind::kMiddle)
		{
			//やられ状態にならない
			return;
		}
		//それ以外の場合は下を通る(普通にダメージ演出に移行する)
	}

	//ガード状態
	if (hitReaction == Character::HitReactionKind::kGuard)
	{
		//動けない時間を設定する
		SetStopTime(kDownTimeMap.at(hitReaction));

		//敵の方向を向く
		m_pCharacter->LookTarget();

		//ガードチュートリアルをクリアさせる
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kGuard));

		//現在の状態がガード状態であればガード状態を継続する
		if (m_kind == CharacterStateKind::kGuard) return;

		//現在の状態がガード状態でなければガード状態に遷移する
		auto nextState = std::make_shared<CharacterStateGuard>(m_pCharacter);

		//次のStateに動けない時間を設定する
		nextState->SetStopTime(kDownTimeMap.at(hitReaction));

		ChangeState(nextState);
		return;
	}

	//今の状態が攻撃を受けた状態でないならば
	if (m_kind != CharacterStateKind::kHitAttack)
	{
		//ここまで来たらヒットアタック状態に遷移する
		std::shared_ptr<CharacterStateHitAttack> nextState = std::make_shared<CharacterStateHitAttack>(m_pCharacter);

		//動けない時間を設定する
		nextState->SetStopTime(kDownTimeMap.at(hitReaction));

		//攻撃を受けた時の状態を設定する
		nextState->HitAttack(static_cast<int>(hitReaction));

		ChangeState(nextState);
	}
	else
	{
		auto nowState = std::dynamic_pointer_cast<CharacterStateHitAttack>(m_pCharacter->m_pState);

		//動けない時間を設定する
		nowState->SetStopTime(kDownTimeMap.at(hitReaction));

		nowState->HitAttack(static_cast<int>(hitReaction));
	}

	//ここまで来たら攻撃を受けているので

	//特定の攻撃を行っていたらチュートリアルをクリアさせる
	if (status.attackName == "Low1")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kPhysicalAttack1));
	}
	else if (status.attackName == "Low2")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kPhysicalAttack2));
	}
	else if (status.attackName == "Low3")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kPhysicalAttack3));
	}
	else if (status.attackName == "Low4")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kPhysicalAttack4));
	}
	else if (status.attackName == "Low5")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kPhysicalAttack5));
	}
	else if (status.attackName == "Low6")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kPhysicalAttack6));
	}
	else if (status.attackName == "Low7")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kPhysicalAttack7));
	}
	else if (status.attackName == "Low8")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kPhysicalAttack8));
	}
	//チャージ格闘攻撃をしていたらチャージ格闘攻撃チュートリアルをクリアさせる
	else if (status.attackName == "UpCharge" ||
		status.attackName == "MiddleCharge" ||
		status.attackName == "DownCharge")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kChargePhysicalAttack));
	}
	else if (status.attackName == "Upper")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kUpperAttack));
	}
	else if (status.attackName == "Stan")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kDekaKick));
	}
	else if (status.attackName == "LegSweep")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kCycloneKick));
	}
	else if (status.attackName == "Energy1")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kEnergyAttack));
	}
	else if (status.attackName == "EnergyCharge")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kChargeEnergyAttack));
	}
	else if (status.attackName == "Teleportation")
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kChaseAttack));
	}
	//必殺技を受けていたら必殺技チュートリアルをクリアさせる
	else if (status.attackKind == Character::AttackKind::kLaser)
	{
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kSpecialAttack));
	}
}

void CharacterStateBase::SuccessTutorial(int tutorialNumber)
{
	//ゲームモードがチュートリアルでなければ無視する
	if (m_pCharacter->m_pBattleManager->GetGameKind() != GameManagerBase::GameKind::kTutorial) return;

	auto manager = std::dynamic_pointer_cast<TutorialManager>(m_pCharacter->m_pBattleManager);

	auto clearKind = static_cast<TutorialManager::TutorialSuccessKind>(tutorialNumber);

	manager->SuccessTutorial(clearKind);
}

void CharacterStateBase::SetDrawFlag(bool flag)
{
	m_pCharacter->m_isDrawCharacter = flag;
}

int CharacterStateBase::GetNextHitReactionKind(std::shared_ptr<Attack> attack)
{
	auto status = attack->GetStatus();

	//そもそもガードできる状態にいるのかを調べる
	bool isGuard = false;

	//ガードできる状態かを調べる
	for (auto& hitReaction : kCanGuardHitReactionKind)
	{
		if (m_pCharacter->m_nowHitReaction == hitReaction)
		{
			isGuard = true;
		}
	}

	//敵が後ろ側にいるならば
	if (!m_pCharacter->IsFrontTarget())
	{
		//絶対にガードできない
		isGuard = false;
	}

	//ガードできる状態にいなければ
	if (!isGuard)
	{
		//ガード失敗
		return static_cast<int>(kHitKindMap.at(status.attackHitKind));
	}

	//打撃攻撃の場合
	if (status.attackKind == Character::AttackKind::kPhysical)
	{
		//上ガードをしている場合
		if (m_guardKind == CharacterGuardKind::kUpGuard)
		{
			//下に吹き飛ばす攻撃なら
			if (status.attackHitKind == Character::AttackHitKind::kDownBurst)
			{
				//上ガードのチュートリアルをクリアさせる
				SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kUpGuard));

				//ガード成功
				return static_cast<int>(Character::HitReactionKind::kGuard);
			}
			//弱攻撃か中攻撃なら
			else if (status.attackHitKind == Character::AttackHitKind::kLow ||
				status.attackHitKind == Character::AttackHitKind::kMiddle)
			{
				//攻撃を受けている状態ならば
				if (m_kind == CharacterStateKind::kHitAttack)
				{
					//ガード失敗
					return static_cast<int>(kHitKindMap.at(status.attackHitKind));
				}
				//攻撃を受けていない状態なら
				else
				{
					//基本的にガード成功させる
					return static_cast<int>(Character::HitReactionKind::kGuard);
				}
			}
			//ほかの攻撃は基本的にガード失敗
			else
			{
				return static_cast<int>(kHitKindMap.at(status.attackHitKind));
			}
		}
		//中段ガードをしている場合
		else if (m_guardKind == CharacterGuardKind::kMiddleGuard)
		{
			//奥に吹き飛ばす攻撃か中段スタンならば
			if (status.attackHitKind == Character::AttackHitKind::kFarBurst ||
				status.attackHitKind == Character::AttackHitKind::kMiddleStan)
			{
				//ガード成功
				return static_cast<int>(Character::HitReactionKind::kGuard);
			}
			//弱攻撃か中攻撃ならば
			else if (status.attackHitKind == Character::AttackHitKind::kLow ||
				status.attackHitKind == Character::AttackHitKind::kMiddle)
			{
				//攻撃を受けている状態ならば
				if (m_kind == CharacterStateKind::kHitAttack)
				{
					//ガード失敗
					return static_cast<int>(kHitKindMap.at(status.attackHitKind));
				}
				//攻撃を受けていない状態なら
				else
				{
					//基本的にガード成功させる
					return static_cast<int>(Character::HitReactionKind::kGuard);
				}
			}
			//ほかの攻撃はガード失敗
			else
			{
				return static_cast<int>(kHitKindMap.at(status.attackHitKind));
			}
		}
		//下段ガードをしている場合
		else if (m_guardKind == CharacterGuardKind::kDownGuard)
		{
			//上に吹き飛ばす攻撃か下段スタンならば
			if (status.attackHitKind == Character::AttackHitKind::kUpBurst ||
				status.attackHitKind == Character::AttackHitKind::kWeakUpBurst ||
				status.attackHitKind == Character::AttackHitKind::kBottomStan)
			{
				//下ガードのチュートリアルをクリアさせる
				SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kDownGuard));

				//ガード成功
				return static_cast<int>(Character::HitReactionKind::kGuard);
			}
			//弱攻撃か中攻撃ならば
			else if (status.attackHitKind == Character::AttackHitKind::kLow ||
				status.attackHitKind == Character::AttackHitKind::kMiddle)
			{
				//攻撃を受けている状態ならば
				if (m_kind == CharacterStateKind::kHitAttack)
				{
					//ガード失敗
					return static_cast<int>(kHitKindMap.at(status.attackHitKind));
				}
				//攻撃を受けていない状態なら
				else
				{
					//基本的にガード成功させる
					return static_cast<int>(Character::HitReactionKind::kGuard);
				}
			}
			//ほかの攻撃はガード失敗
			else
			{
				return static_cast<int>(kHitKindMap.at(status.attackHitKind));
			}
		}
		//ガードしていない場合
		else
		{
			return static_cast<int>(kHitKindMap.at(status.attackHitKind));
		}
	}
	//気弾系の攻撃の場合
	else if (status.attackKind == Character::AttackKind::kEnergy ||
		status.attackKind == Character::AttackKind::kLaser)
	{
		//攻撃を受けている状態ならば
		if (m_kind == CharacterStateKind::kHitAttack)
		{
			//ガード失敗
			return static_cast<int>(kHitKindMap.at(status.attackHitKind));
		}
		//攻撃を受けていない状態なら
		else
		{
			//ガードしていたら
			if (m_guardKind == CharacterGuardKind::kUpGuard ||
				m_guardKind == CharacterGuardKind::kMiddleGuard ||
				m_guardKind == CharacterGuardKind::kDownGuard)
			{

				//基本的にガード成功させる
				return static_cast<int>(Character::HitReactionKind::kGuard);
			}
			//ガードしていなかったら
			else
			{
				//ガード失敗
				return static_cast<int>(kHitKindMap.at(status.attackHitKind));
			}
		}
	}
	//ぶつかると演出に切り替わる攻撃や突撃攻撃の場合(未実装)
	else if (status.attackKind == Character::AttackKind::kRush ||
		status.attackKind == Character::AttackKind::kAssault)
	{
		//攻撃を受けている状態ならば
		if (m_kind == CharacterStateKind::kHitAttack)
		{
			//ガード失敗
			return static_cast<int>(kHitKindMap.at(status.attackHitKind));
		}
		//攻撃を受けていない状態なら
		else
		{
			//ガードしていたら
			if (m_guardKind == CharacterGuardKind::kUpGuard ||
				m_guardKind == CharacterGuardKind::kMiddleGuard ||
				m_guardKind == CharacterGuardKind::kDownGuard)
			{
				//ガードブレイク状態にする
				return static_cast<int>(Character::HitReactionKind::kGuardBreak);
			}
			//ガードしていない場合
			else
			{
				return static_cast<int>(kHitKindMap.at(status.attackHitKind));
			}
		}
	}
	//投げ攻撃(未実装)
	else if (status.attackKind == Character::AttackKind::kThrow)
	{
		//基本的にどの状態でも受ける
		return static_cast<int>(kHitKindMap.at(status.attackHitKind));
	}

	//たぶんここまで来ないはずだけど一応
	//警告けしのために返り値を設定しておく
	return static_cast<int>(kHitKindMap.at(status.attackHitKind));

}
