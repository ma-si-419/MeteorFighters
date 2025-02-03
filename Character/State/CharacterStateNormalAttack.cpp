#include "CharacterStateNormalAttack.h"
#include "CharacterStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "Character.h"
#include <cmath>
#include "Effect.h"
#include "GameSceneConstant.h"
#include "TutorialManager.h"
#include "SoundManager.h"

namespace
{
	//次の攻撃の入力判定をはじめるフレーム
	constexpr float kNextAttackInputTime = 5.0f;
	//格闘攻撃の攻撃持続フレーム
	constexpr int kPhysicalAttackLifeTime = 2;
	//格闘攻撃の判定の大きさ
	constexpr float kPhysicalAttackRadius = 5.0f;
	//格闘攻撃で最低限離れる距離
	constexpr float kPhysicalAttackNearLange = 8.0f;
	//瞬間移動攻撃でずらす大きさ
	constexpr float kTeleportationShiftLange = 10.0f;
	//気弾攻撃の攻撃判定が残る時間
	constexpr int kEnergyAttackLifeTime = 240;
	//気弾攻撃の判定の大きさ
	constexpr float kEnergyAttackRadius = 3.0f;
	//派生攻撃のスティックの傾き
	constexpr int kPhysicalAttackStickPower = 500;
	//ため攻撃を行っているときの経過時間の経過速度
	constexpr float kOnChargeStateTimeCountSpeed = 0.1f;
	//ため攻撃を行っているときのアニメ再生速度
	constexpr float kOnChargeAnimPlaySpeed = 0.1f;
	//ため攻撃の貯められるマックスのフレーム数
	constexpr float kChargeMaxTime = kOnChargeStateTimeCountSpeed * 40;
	//瞬間移動のエフェクトを表示する時間
	constexpr int kTeleportationEffectTime = 20;
	//瞬間移動時にカメラを止める時間
	constexpr int kTeleportationCameraStopTime = 18;
	//上入力の強攻撃
	const std::string kUpperAttackName = "Upper";
	//中入力の強攻撃
	const std::string kStanAttackName = "Stan";
	//下入力の強攻撃
	const std::string kLegSweepAttackName = "LegSweep";
	//上入力のため攻撃
	const std::string kUpChargeAttack = "UpCharge";
	//中入力のため攻撃
	const std::string kMiddleChargeAttack = "MiddleCharge";
	//下入力のため攻撃
	const std::string kDownChargeAttack = "DownCharge";
	//気弾のため攻撃
	const std::string kEnergyChargeAttack = "EnergyCharge";
	//瞬間移動する攻撃
	const std::string kTeleportationAttack = "Teleportation";
	//アニメーションのブレンドスピード
	constexpr float kAnimBlendSpeed = 0.15f;

	//格闘攻撃の際に再生する音声
	const std::map<Character::AttackHitKind, Character::VoiceKind> kPhysicalAttackSound =
	{
		{Character::AttackHitKind::kLow,Character::VoiceKind::kLowAttack},
		{Character::AttackHitKind::kMiddle,Character::VoiceKind::kMiddleAttack},
		{Character::AttackHitKind::kWeakUpBurst,Character::VoiceKind::kHighAttack},
		{Character::AttackHitKind::kUpBurst,Character::VoiceKind::kMiddleAttack},
		{Character::AttackHitKind::kDownBurst,Character::VoiceKind::kHighAttack},
		{Character::AttackHitKind::kFarBurst,Character::VoiceKind::kHighAttack},
		{Character::AttackHitKind::kBottomStan,Character::VoiceKind::kMiddleAttack},
		{Character::AttackHitKind::kMiddleStan,Character::VoiceKind::kMiddleAttack}
	};

}

CharacterStateNormalAttack::CharacterStateNormalAttack(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
	m_nowAttackName("empty"),
	m_nextAttackName("empty"),
	m_isNextAttack(false),
	m_isCharge(false),
	m_attackKey("empty"),
	m_chargeTime(0.0f),
	m_isAttacked(false),
	m_isNextCharge(false),
	m_chaseAttackNum(0),
	m_isEndAttack(false)
{
}
void CharacterStateNormalAttack::SetAttack(std::string key, std::string attackName)
{
	//押されたキーを保存しておく
	m_attackKey = key;

	m_nowAttackName = attackName;
}
void CharacterStateNormalAttack::SetAttackVelo(MyEngine::Vector3 velo)
{
	m_firstAttackMoveVec = velo;
}
void CharacterStateNormalAttack::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kNormalAttack;

	//攻撃の情報
	auto status = m_pCharacter->GetNormalAttackData(m_nowAttackName);

	//設定された攻撃のアニメーション取得
	std::string animName = status.animationName;

	Character::AnimKind anim = static_cast<Character::AnimKind>(m_pCharacter->GetAttackAnimKind(animName));


	//アニメーションの変更
	m_pCharacter->ChangeAnim(anim, false, kAnimBlendSpeed);
	m_pCharacter->SetAnimPlaySpeed(status.animationSpeed);

	//向かう方向の設定
	MyEngine::Vector3 shiftVec = (m_pManager->GetTargetPos(m_pCharacter) - m_pCharacter->GetPos()).Normalize();

	shiftVec *= kPhysicalAttackNearLange;

	shiftVec.y = 0;

	m_moveTargetPos = m_pManager->GetTargetPos(m_pCharacter) + shiftVec;

	m_pCharacter->LookTarget();

	//最初の攻撃がチャージできるかどうかを設定する
	if (m_nowAttackName == kUpChargeAttack ||
		m_nowAttackName == kMiddleChargeAttack ||
		m_nowAttackName == kDownChargeAttack)
	{
		m_isCharge = true;
	}

	//格闘攻撃なら
	if (status.attackKind == Character::AttackKind::kPhysical)
	{
		//最初の音声を再生する
		m_pCharacter->PlayVoice(kPhysicalAttackSound.at(status.attackHitKind));
	}
	//気弾攻撃なら
	else if (status.attackKind == Character::AttackKind::kEnergy)
	{
		//気弾攻撃のサウンドを再生する
		SoundManager::GetInstance().PlayOnceSound("StartEnergy");
	}

}

void CharacterStateNormalAttack::Update()
{
	//入力情報
	auto input = GetCharacterInput();

	//チャージ中はフレームを数える速度を変える
	if (m_isCharge)
	{
		m_time += kOnChargeStateTimeCountSpeed;
		//どのくらいチャージしたかを数える
		m_chargeTime += kOnChargeStateTimeCountSpeed;
		//マックスまで貯めたらチャージをやめる
		if (m_time > kChargeMaxTime)
		{
			m_isCharge = false;
		}
		//アニメーションの再生速度も変える
		m_pCharacter->SetAnimPlaySpeed(kOnChargeAnimPlaySpeed);
	}
	//チャージしていない状態
	else
	{
		//Stateにいるフレーム数を数えておく
		m_time++;
		//アニメーションの再生速度を通常にする
		m_pCharacter->SetAnimPlaySpeed();
	}

	//ボタンを押している間はチャージを続ける
	if (m_isCharge)
	{
		//ボタンが離されたらチャージをやめる
		if (!input->IsPress(m_attackKey))
		{
			m_isCharge = false;
		}
	}

	auto attackData = m_pCharacter->GetNormalAttackData(m_nowAttackName);

	MyEngine::Vector3 velo;

	//攻撃の合計フレームを超えたら
	if (m_time >= attackData.totalFrame)
	{
		m_isEndAttack = true;

		//アイドル状態に戻る
		std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		next->SetLastAttackState();

		ChangeState(next);
		return;
	}

	//次の攻撃に移行できるフレームであれば
	if (m_time >= attackData.cancelFrame)
	{
		//次に攻撃を行うと決定していれば
		if (m_isNextAttack)
		{

			//次の攻撃がない攻撃だったら攻撃を行わない
			if (m_nextAttackName == "None") return;

			//次の攻撃の情報
			Character::NormalAttackData nextAttack = m_pCharacter->GetNormalAttackData(m_nextAttackName);

			//次の攻撃を出す条件に敵の状態があれば今の敵のやられ状態と照らし合わせる
			Character::HitReactionKind enemyHitReaction = static_cast<Character::HitReactionKind>(m_pManager->GetTargetHitReaction(m_pCharacter));

			//次の攻撃が気力攻撃であれば
			if (nextAttack.attackKind == Character::AttackKind::kEnergy)
			{
				//気力が足りなければ攻撃を行わない
				if (!m_pCharacter->SubMp(GameSceneConstant::kEnergyAttackCost)) return;
			}

			if (nextAttack.targetHitReaction == "中")
			{
				//中やられじゃなければ
				if (enemyHitReaction != Character::HitReactionKind::kMiddle)return;
			}
			else if (nextAttack.targetHitReaction == "吹っ飛び")
			{
				//吹っ飛び状態でなければじゃなければ
				bool flag = false;

				//下の中のどれかを通ればOK
				if (enemyHitReaction != Character::HitReactionKind::kUpBurst)
				{
					flag = true;
				}
				else if (enemyHitReaction != Character::HitReactionKind::kFarBurst)
				{
					flag = true;
				}
				else if (enemyHitReaction != Character::HitReactionKind::kDownBurst)
				{
					flag = true;
				}

				//1つも通っていなければ攻撃を出さない
				if (!flag) return;
			}

			//瞬間移動する攻撃であれば瞬間移動する
			if (nextAttack.isTeleportation)
			{
				//この攻撃がＹボタンによる追撃で
				if (m_attackKey == "Y")
				{
					//これ以上追撃できなければ
					if (m_chaseAttackNum >= m_pCharacter->GetChaseNum()) return;
					//追撃できるのなら追撃した回数を増やす
					m_chaseAttackNum++;
				}

				//移動前に瞬間移動のエフェクトを設定する
				std::shared_ptr<Effect> effect = std::make_shared<Effect>(Effect::EffectKind::kTeleportaion);

				effect->SetPos(m_pCharacter->GetPos());

				effect->SetLifeTime(kTeleportationEffectTime);

				m_pManager->EntryEffect(effect);

				//カメラを少しだけ止める
				m_pManager->StopCamera(kTeleportationCameraStopTime);

				//次の攻撃発生フレーム時に敵がいる場所を計算する
				MyEngine::Vector3 teleportationPos = m_pManager->GetTargetPos(m_pCharacter) + (m_pManager->GetTargetVelo(m_pCharacter) * (static_cast<float>(nextAttack.attackFrame)));
				//瞬間移動先に攻撃の攻撃範囲分だけずれを足す
				MyEngine::Vector3 attackShiftVec = m_pManager->GetTargetVelo(m_pCharacter);

				teleportationPos += attackShiftVec.Normalize() * (kTeleportationShiftLange);

				attackShiftVec.y = 0;

				teleportationPos += attackShiftVec.Normalize() * (kPhysicalAttackRadius);

				SetCharacterPos(teleportationPos);
			}

			//時間のリセット
			m_time = 0;
			//攻撃を行っていない状態に変化
			m_isAttacked = false;
			//次に行う攻撃の設定
			m_nowAttackName = m_nextAttackName;
			m_nextAttackName = "empty";
			m_firstAttackMoveVec = MyEngine::Vector3(0, 0, 0);


			//次の攻撃がチャージできるかどうか
			m_isCharge = m_isNextCharge;
			m_isNextCharge = false;

			//攻撃を行う方向を設定する
			MyEngine::LocalPos attackPos;

			//ローカル座標の中心を設定
			attackPos.SetCenterPos(m_pManager->GetTargetPos(m_pCharacter));
			//ローカル座標の正面座標を設定
			attackPos.SetFrontPos(m_pManager->GetTargetPos(m_pCharacter) + (m_pCharacter->GetPos() - m_pManager->GetTargetPos(m_pCharacter)).Normalize());
			//ローカル座標を設定
			attackPos.SetLocalPos(MyEngine::Vector3(0.0f, 0.0f, kPhysicalAttackRadius));

			MyEngine::Vector3 shiftVec = (attackPos.GetWorldPos() - m_pCharacter->GetPos()).Normalize();

			shiftVec *= kPhysicalAttackNearLange;

			shiftVec.y = 0;

			if (m_pCharacter->IsFrontTarget())
			{
				m_moveTargetPos = m_pManager->GetTargetPos(m_pCharacter) + shiftVec;
			}

			Character::AnimKind anim = static_cast<Character::AnimKind>(m_pCharacter->GetAttackAnimKind(nextAttack.animationName));

			m_pCharacter->ChangeAnim(anim, false, kAnimBlendSpeed);
			m_pCharacter->SetAnimPlaySpeed(nextAttack.animationSpeed);

			m_isNextAttack = false;

			if (m_pCharacter->IsFrontTarget())
			{
				m_pCharacter->LookTarget();
			}

			//攻撃情報の更新
			attackData = nextAttack;

			//格闘攻撃なら
			if (attackData.attackKind == Character::AttackKind::kPhysical)
			{
				//次の攻撃の音声を再生する
				m_pCharacter->PlayVoice(kPhysicalAttackSound.at(attackData.attackHitKind));
			}
			else if (attackData.attackKind == Character::AttackKind::kEnergy)
			{
				//気弾攻撃のサウンドを再生する
				SoundManager::GetInstance().PlayOnceSound("StartEnergy");
			}
		}
	}

	//格闘攻撃ならば
	if (attackData.attackKind == Character::AttackKind::kPhysical)
	{
		//攻撃を出すフレームまでは移動する
		if (m_time <= attackData.attackFrame)
		{

			MyEngine::Vector3 dir;

			dir = (m_moveTargetPos - m_pCharacter->GetPos()).Normalize();

			//プレイヤーの移動速度
			float speed = attackData.moveSpeed;

			//チャージ中であれば
			if (m_isCharge)
			{
				//移動速度をアニメーションの再生速度と同じ倍率にする
				speed *= kOnChargeAnimPlaySpeed;
			}

			//移動距離が行きたい座標までの距離よりも長ければ
			if ((m_moveTargetPos - m_pCharacter->GetPos()).Length() < speed * attackData.attackFrame)
			{
				//移動距離を補正する
				speed = (m_moveTargetPos - m_pCharacter->GetPos()).Length() / attackData.attackFrame;
			}

			velo = dir * speed;

			//もし設定された移動ベクトルがあれば
			if (m_firstAttackMoveVec.SqLength() > 0)
			{
				//その移動ベクトルにする
				velo = m_firstAttackMoveVec;
			}

		}
	}
	//気弾攻撃ならば
	else if (attackData.attackKind == Character::AttackKind::kEnergy)
	{

		//移動方向ベクトル
		MyEngine::Vector3 dir;

		MyEngine::Vector3 leftStickDir;

		//スティックの情報取得
		auto stick = input->GetStickInfo();

		//左スティックの傾き取得
		leftStickDir = MyEngine::Vector3(stick.leftStickX, 0, -stick.leftStickY);

		//移動ベクトルが0じゃなければ
		if (leftStickDir.SqLength() > 0.001)
		{
			//移動方向
			dir = leftStickDir.Normalize();

			//エネミーの方向に移動方向を回転させる
			float vX = m_pManager->GetTargetPos(m_pCharacter).x - m_pCharacter->GetPos().x;
			float vZ = m_pManager->GetTargetPos(m_pCharacter).z - m_pCharacter->GetPos().z;

			float angle = std::atan2f(vX, vZ);

			MyEngine::Vector3 rotation(0.0f, angle, 0.0f);

			MATRIX mat = rotation.GetRotationMat();

			dir = dir.MatTransform(mat);

			//移動方向にスピードをかける
			velo = dir * m_pCharacter->GetSpeed();
		}
		//ジャンプボタンが押されたら
		if (input->IsPress("RB"))
		{
			velo.y = m_pCharacter->GetSpeed();
		}
		//下降ボタンが押されたら
		else if (input->IsPushTrigger(true))
		{
			velo.y = -m_pCharacter->GetSpeed();
		}
	}

	//攻撃を出すフレームになったら
	if (m_time > attackData.attackFrame &&
		!m_isAttacked)
	{
		m_isAttacked = true;
		Character::AttackData attack;

		//瞬間移動する攻撃であれば
		if (attackData.isTeleportation)
		{
			//敵の方向を向く
			m_pCharacter->LookTarget();
			m_pCharacter->SetFrontPos(m_pManager->GetTargetPos(m_pCharacter));
		}

		//チャージした時のダメージ
		if (m_chargeTime > 0.0f)
		{
			//チャージした時間によってダメージを上昇させる
			attack.damage = static_cast<int>((attackData.damageRate + m_chargeTime) * m_pCharacter->GetPower());
		}
		//チャージされていないときのダメージ
		else
		{
			attack.damage = static_cast<int>(attackData.damageRate * m_pCharacter->GetPower());
		}

		attack.attackHitKind = attackData.attackHitKind;
		if (m_pCharacter->GetTag() == ObjectTag::kOnePlayer)
		{
			attack.isPlayer = true;
		}
		else if (m_pCharacter->GetTag() == ObjectTag::kTwoPlayer)
		{
			attack.isPlayer = false;
		}
		attack.speed = attackData.attackMoveSpeed;

		//格闘攻撃なら
		if (attackData.attackKind == Character::AttackKind::kPhysical)
		{
			attack.lifeTime = kPhysicalAttackLifeTime;
			attack.radius = kPhysicalAttackRadius;
		}
		//気弾攻撃なら
		else if (attackData.attackKind == Character::AttackKind::kEnergy)
		{
			attack.lifeTime = kEnergyAttackLifeTime;
			attack.radius = kEnergyAttackRadius;
		}
		attack.attackKind = attackData.attackKind;
		attack.effectName = attackData.effectName;
		attack.attackName = attackData.attackName;
		attack.hitSoundName = attackData.soundName;

		//攻撃を作成
		m_pCharacter->CreateAttack(attack);
	}

	//次の攻撃を行うか判定する
	if (m_time > kNextAttackInputTime)
	{
		//格闘攻撃なら
		if (attackData.attackKind == Character::AttackKind::kPhysical)
		{
			//Xボタンで次の通常攻撃に移行する
			if (input->IsTrigger("X"))
			{
				m_nextAttackName = attackData.nextComboName;
				m_isNextAttack = true;
				m_attackKey = "X";
			}
			//Yボタンで派生攻撃を出す
			else if (input->IsTrigger("Y"))
			{
				//前の攻撃が通常攻撃なら派生攻撃に移れる
				if (m_attackKey == "X")
				{
					//Yボタンの派生技は基本チャージができる
					m_isNextCharge = true;
					//上入力しながらの派生攻撃
					if (input->GetStickInfo().leftStickY < -kPhysicalAttackStickPower)
					{
						m_nextAttackName = kUpperAttackName;
					}
					else if (input->GetStickInfo().leftStickY > kPhysicalAttackStickPower)
					{
						m_nextAttackName = kLegSweepAttackName;
					}
					else
					{
						m_nextAttackName = kStanAttackName;
					}
					m_isNextAttack = true;
				}
				//敵が吹っ飛び状態の時に派生攻撃をしていたら
				Character::HitReactionKind kind = static_cast<Character::HitReactionKind>(m_pManager->GetTargetHitReaction(m_pCharacter));
				if (kind == Character::HitReactionKind::kUpBurst ||
					kind == Character::HitReactionKind::kFarBurst ||
					kind == Character::HitReactionKind::kDownBurst)
				{
					//前の攻撃が吹っ飛ばし攻撃であれば瞬間移動攻撃に移行する
					if (attackData.attackHitKind == Character::AttackHitKind::kUpBurst ||
						attackData.attackHitKind == Character::AttackHitKind::kFarBurst ||
						attackData.attackHitKind == Character::AttackHitKind::kDownBurst)
					{
						m_nextAttackName = kTeleportationAttack;
						//瞬間移動攻撃だけチャージができない
						m_isNextCharge = false;
						m_isNextAttack = true;
					}
				}
				m_attackKey = "Y";
			}
		}
		//気弾攻撃なら
		if (attackData.attackKind == Character::AttackKind::kEnergy)
		{
			//Yボタンで次の攻撃に移行する
			if (input->IsRelease("Y"))
			{
				m_nextAttackName = attackData.nextComboName;
				m_isNextAttack = true;
			}
		}
	}

	SetCharacterVelo(velo);

#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Attack", GetColor(255, 255, 255));

#endif // _DEBUG
}

void CharacterStateNormalAttack::Exit()
{
	//アニメーションの再生速度を戻しておく
	m_pCharacter->SetAnimPlaySpeed();
	//敵の方向を向く
	m_pCharacter->LookTarget();
	m_pCharacter->SetFrontPos(m_pManager->GetTargetPos(m_pCharacter));
}

int CharacterStateNormalAttack::GetTimeToAttack()
{
	//すでに攻撃を出しているなら-1を返す
	if (m_isAttacked) return -1;

	//攻撃の情報
	auto status = m_pCharacter->GetNormalAttackData(m_nowAttackName);

	//攻撃を出すフレームまであと何フレームかを返す
	return status.attackFrame - m_time;
}
