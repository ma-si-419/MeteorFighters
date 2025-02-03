#include "CharacterStateButtonBashing.h"
#include "CharacterStateIdle.h"
#include "Character.h"
#include "DxLib.h"
#include "GameSceneConstant.h"
#include "GameManagerBase.h"
#include "Input.h"
#include "Attack.h"
#include "Effect.h"
#include "TutorialManager.h"
#include "SoundManager.h"


namespace
{
	const MyEngine::Vector3 kPlayerInitPos(-3, 200, -3);
	const MyEngine::Vector3 kEnemyInitPos(3, 200, 3);

	//中心座標
	const MyEngine::Vector3 kTargetPos(0, 200, 0);

	//ぶつかった時の移動速度
	constexpr float kBumpSpeed = -10.0f;

	//ぶつかってから止まりはじめるまでの時間
	constexpr int kStopStartTime = 3;

	//止まる力
	constexpr float kStopPower = -(kBumpSpeed / 10.0f);

	//止まった時にどのくらいとどまるか
	constexpr int kHitBackStayTime = 15;

	//最大速度
	constexpr float kMaxMoveSpeed = 3.0f;

	//加速度
	constexpr float kAcceleration = 0.2f;

	//初速
	constexpr float kDashInitSpeed = 1.0f;

	//ブレンドスピード
	constexpr float kBlendSpeed = 0.03f;

	//終了時に発生する攻撃の名前
	const std::string kAttackName = "ButtonBashingAttack";

	//攻撃の大きさ
	constexpr float kAttackRadius = 8.0f;

	//攻撃の生存時間
	constexpr int kAttackLifeTime = 2;

	//エフェクトのループ開始時間
	constexpr float kEffectLoopStartTime = 9.0f;
	constexpr float kEffectLoopEndTime = 11.0f;

	//ぶつかった時のエフェクトの生存時間
	constexpr float kBumpEffectLifeTime = 5.0f;

	//ぶつかった時カメラを揺らす時間
	constexpr int kBumpCameraShakeTime = 2;

	//ぶつかった時カメラを揺らす大きさ
	constexpr int kBumpCameraShakePower = 3;

	//効果音を再生する間隔
	constexpr int kSoundPlayInterval = 7;
}

CharacterStateButtonBashing::CharacterStateButtonBashing(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
	m_moveSpeed(kBumpSpeed),
	m_isBump(false),
	m_bumpTime(0),
	m_stayTime(0),
	m_isStay(false),
	m_soundPlayTime(0)
{
}

void CharacterStateButtonBashing::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kButtonBashing;

	//1Pか2Pかで最初の座標を変更する
	if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
	{
		SetCharacterPos(kPlayerInitPos);
	}
	else
	{
		SetCharacterPos(kEnemyInitPos);
	}

	//移動速度を初期化
	SetCharacterVelo(MyEngine::Vector3(0, 0, 0));

	//アニメーション変更
	m_pCharacter->ChangeAnim(Character::AnimKind::kButtonBashingHitBack, false);

	//ボタン連打を始めるとマネージャーに伝える
	m_pManager->StartButtonBashing();

	//エフェクトを再生する
	auto effect = std::make_shared<Effect>(Effect::EffectKind::kHighHit);
	//座標設定
	effect->SetPos(kTargetPos);
	//エフェクトの再生時間を設定
	effect->SetLifeTime(kBumpEffectLifeTime);
	//エフェクトを登録
	m_pManager->EntryEffect(effect);
	//カメラを揺らす
	m_pManager->ShakeCamera(kBumpCameraShakeTime, kBumpCameraShakePower);
}

void CharacterStateButtonBashing::Update()
{

	//もし相手のStateがButtonBashing出なければ早期リターン
	if (static_cast<CharacterStateKind>(m_pManager->GetTargetState(m_pCharacter)) != CharacterStateKind::kButtonBashing) return;

	//ぶつかってから何フレーム立ったかを保存する
	m_bumpTime++;

	//敵の方向を向く
	m_pCharacter->LookTarget();

	//中心までのベクトル
	MyEngine::Vector3 toTarget;

	if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
	{
		toTarget = kTargetPos - kPlayerInitPos;
	}
	else
	{
		toTarget = kTargetPos - kEnemyInitPos;
	}

	//中心に向かって移動する
	MyEngine::Vector3 moveDir = (toTarget).Normalize();

	//止まり始めるタイミングになったら
	if (m_bumpTime > kStopStartTime)
	{
		//移動速度がマイナスであれば
		if (m_moveSpeed < 0.0f)
		{
			//動きを止めていく
			m_moveSpeed += kStopPower;

			//移動速度がプラスになれば
			if (m_moveSpeed > 0.0f)
			{
				m_isStay = true;
			}
		}
	}

	//一定時間とどまる
	if (m_isStay)
	{
		//止まっている時間を足していく
		m_moveSpeed = 0.0f;
		m_stayTime++;

		//一定時間とどまったら
		if (m_stayTime > kHitBackStayTime)
		{
			//とどまるのをやめる
			m_isStay = false;
			m_stayTime = 0;
			m_moveSpeed = kDashInitSpeed;
			if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
			{
				//二つ目のSituationに行く
				m_pManager->SetBashingSituation(GameManagerBase::ButtonBashingSituation::kSecondHit);
			}
		}
	}

	//とどまるのが終わったら
	if (m_bumpTime > kStopStartTime && !m_isStay)
	{
		//加速していく
		m_moveSpeed += kAcceleration;
	}

	//移動速度のクランプ
	m_moveSpeed = static_cast<float>(fmin(m_moveSpeed, kMaxMoveSpeed));

	//敵との距離
	float toEnemyLength = (m_pManager->GetTargetPos(m_pCharacter) - m_pCharacter->GetPos()).Length();

	//移動ベクトル
	MyEngine::Vector3 moveVec;

	//移動ベクトルで移動するかどうか
	bool isMove = true;

	//敵とぶつかる距離になったら
	if ((m_pManager->GetTargetPos(m_pCharacter) - m_pCharacter->GetPos()).Length() < (GameSceneConstant::kCharacterRadius * 2.0f) + 3.0f &&
		m_moveSpeed >= 0.0f)
	{
		//一度ぶつかっていたら
		if (m_isBump)
		{
			//移動しないようにする
			isMove = false;
			SetCharacterVelo(MyEngine::Vector3(0, 0, 0));

			//効果音再生時間をカウントする
			m_soundPlayTime++;

			//効果音を再生する
			if (m_soundPlayTime > kSoundPlayInterval)
			{
				SoundManager::GetInstance().PlayOnceSound("ButtonBashing");
				m_soundPlayTime = 0;
			}

			//二つ目のSituationに行く
			m_pManager->SetBashingSituation(GameManagerBase::ButtonBashingSituation::kFighting);

			//エフェクトを設定していなければ
			if (!m_pEffect)
			{
				//エフェクトを再生する
				m_pEffect = std::make_shared<Effect>(Effect::EffectKind::kLowHit);
				//ループ設定
				m_pEffect->SetLoop(kEffectLoopStartTime, kEffectLoopEndTime);
				//座標設定
				m_pEffect->SetPos(kTargetPos);
				//エフェクトを登録
				m_pManager->EntryEffect(m_pEffect);
			}

			//座標を補正する
			if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
			{
				SetCharacterPos(kPlayerInitPos);
			}
			else
			{
				SetCharacterPos(kEnemyInitPos);
			}
		}
		//初めてぶつかるタイミングであれば
		else
		{
			if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
			{
				moveDir = (kPlayerInitPos - kTargetPos).Normalize();
			}
			else
			{
				moveDir = (kEnemyInitPos - kTargetPos).Normalize();
			}
			m_moveSpeed = kBumpSpeed;
			m_pCharacter->ChangeAnim(Character::AnimKind::kButtonBashingHitBack, false);

			//エフェクトを再生する
			auto effect = std::make_shared<Effect>(Effect::EffectKind::kHighHit);
			//座標設定
			effect->SetPos(kTargetPos);
			//エフェクトの再生時間を設定
			effect->SetLifeTime(kBumpEffectLifeTime);
			//エフェクトを登録
			m_pManager->EntryEffect(effect);
			//カメラを揺らす
			m_pManager->ShakeCamera(kBumpCameraShakeTime, kBumpCameraShakePower);
			//ぶつかったときのサウンドを再生
			SoundManager::GetInstance().PlayOnceSound("HighHit");
		}
		m_bumpTime = 0;

		m_isBump = true;
	}

	//移動すると設定されていたら
	if (isMove)
	{
		moveVec = moveDir * m_moveSpeed;
	}

	//移動ベクトルを設定
	SetCharacterVelo(moveVec);

	//アニメーションの変更

	//移動速度が前方向になっていれば
	if (m_moveSpeed >= 0.0f && !m_isStay)
	{
		//一度ぶつかっていたら
		if (m_isBump)
		{
			if (m_pCharacter->GetPlayAnimKind() != Character::AnimKind::kOnButtonBashing)
			{
				m_pCharacter->ChangeAnim(Character::AnimKind::kOnButtonBashing, true);
			}
		}
		//一度も敵とぶつかっていなければ
		else
		{
			//アニメーションを変更する
			if (m_pCharacter->GetPlayAnimKind() != Character::AnimKind::kButtonBashingAttack)
			{
				m_pCharacter->ChangeAnim(Character::AnimKind::kButtonBashingAttack, false, kBlendSpeed);
			}
		}
	}

	//連打対決
	if (static_cast<GameManagerBase::ButtonBashingSituation>(m_pManager->GetBashingSituation()) == GameManagerBase::ButtonBashingSituation::kFighting)
	{
		auto input = MyEngine::Input::GetInstance().GetInputData(0);

		//指定されたボタンを押していたら
		if (input->IsTrigger(m_pManager->GetBashingButton()))
		{
			m_pManager->BashButton(m_pCharacter->GetPlayerNumber());
		}
	}

	//ボタン連打をやめるタイミングになったら
	if (!m_pManager->IsButtonBashing())
	{
		//勝っていた場合
		if (m_pManager->GetButtonBashWinner() == m_pCharacter->GetPlayerNumber())
		{
			//勝ったときの音声を再生する
			m_pCharacter->PlayVoice(Character::VoiceKind::kWinBashing);

			//アイドル状態に戻る
			auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

			next->SetEndAnim(static_cast<int>(Character::AnimKind::kOnButtonBashing), 30);

			ChangeState(next);
		}
		//負けていた場合
		else
		{
			//攻撃を受けるようにする

			//タグを相手の攻撃にする
			ObjectTag tag;

			if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
			{
				tag = ObjectTag::kTwoPlayerAttack;
			}
			else
			{
				tag = ObjectTag::kOnePlayerAttack;
			}

			//攻撃の情報を取得
			auto attackData = m_pCharacter->GetNormalAttackData(kAttackName);

			//攻撃に設定するステータス
			Attack::AttackStatus status;
			status.damage = static_cast<int>(attackData.damageRate * m_pCharacter->GetPower());
			status.speed = attackData.attackMoveSpeed;
			status.radius = kAttackRadius;
			status.lifeTime = kAttackLifeTime;
			status.attackKind = attackData.attackKind;
			status.attackHitKind = attackData.attackHitKind;
			status.targetPos = m_pManager->GetTargetPos(m_pCharacter);

			//受ける攻撃
			auto attack = std::make_shared<Attack>(tag, kTargetPos);

			attack->Init(status, m_pManager->GetEffectManagerPointer());

			m_pManager->AddAttack(attack);
		}
	}
}
void CharacterStateButtonBashing::Exit()
{
	m_pManager->ExitEffect(m_pEffect);

	//ボタン連打対決のチュートリアルをクリアする
	SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kButtonBashing));
}
