#include "CharacterStateButtonBashing.h"
#include "CharacterStateIdle.h"
#include "Character.h"
#include "DxLib.h"
#include "GameSceneConstant.h"
#include "GameManagerBase.h"


namespace
{
	const MyEngine::Vector3 kPlayerInitPos(-3, 100, -3);
	const MyEngine::Vector3 kEnemyInitPos(3, 100, 3);

	//中心座標
	const MyEngine::Vector3 kTargetPos(0, 100, 0);

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
}

CharacterStateButtonBashing::CharacterStateButtonBashing(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
	m_moveSpeed(kBumpSpeed),
	m_isBump(false),
	m_bumpTime(0),
	m_stayTime(0),
	m_isStay(false)
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

	//アニメーション変更
	m_pCharacter->ChangeAnim(Character::AnimKind::kButtonBashingHitBack, false);

	//ボタン連打を始めるとマネージャーに伝える
	StartButtonBashing();
}

void CharacterStateButtonBashing::Update()
{
	//ぶつかってから何フレーム立ったかを保存する
	m_bumpTime++;

	//敵の方向を向く
	m_pCharacter->LookTarget();

	//中心までのベクトル
	MyEngine::Vector3 toTarget = kTargetPos - m_pCharacter->GetPos();

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
				SetBashingSituation(static_cast<int>(GameManagerBase::ButtonBashingSituation::kSecondHit));
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
	m_moveSpeed = fmin(m_moveSpeed, kMaxMoveSpeed);

	//敵との距離
	float toEnemyLength = (GetTargetPos() - m_pCharacter->GetPos()).Length();

	//移動ベクトル
	MyEngine::Vector3 moveVec;

	//移動ベクトルで移動するかどうか
	bool isMove = true;

	//敵とぶつかる距離になったら
	if ((GetTargetPos() - m_pCharacter->GetPos()).Length() < (GameSceneConstant::kCharacterRadius * 2.0f) + 3.0f)
	{
		//一度ぶつかっていたら
		if (m_isBump)
		{
			//移動しないようにする
			isMove = false;
			SetCharacterVelo(MyEngine::Vector3(0, 0, 0));

			//二つ目のSituationに行く
			SetBashingSituation(static_cast<int>(GameManagerBase::ButtonBashingSituation::kFighting));

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
			m_moveSpeed = kBumpSpeed;
			m_pCharacter->ChangeAnim(Character::AnimKind::kButtonBashingHitBack, false);
		}

		m_isBump = true;
		m_bumpTime = 0;
	}


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

	//ボタン連打をやめるタイミングになったら
	if (!IsButtonBashing())
	{
		auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		ChangeState(next);

		//勝っていた場合
		if (IsBashWin())
		{

		}
		else
		{

		}
	}
}
void CharacterStateButtonBashing::Exit()
{
}
