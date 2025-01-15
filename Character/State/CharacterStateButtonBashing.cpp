#include "CharacterStateButtonBashing.h"
#include "CharacterStateIdle.h"
#include "Character.h"
#include "DxLib.h"
#include "GameSceneConstant.h"


namespace
{
	const MyEngine::Vector3 kPlayerInitPos(-30, 100, -30);
	const MyEngine::Vector3 kEnemyInitPos(30, 100, 30);

	//中心座標
	const MyEngine::Vector3 kTargetPos(0,100,0);

	//最初の移動速度
	constexpr float kInitMoveSpeed = -3.5f;

	//最大速度
	constexpr float kMaxMoveSpeed = 3.0f;

	//加速度
	constexpr float kAcceleration = 0.25f;
}

CharacterStateButtonBashing::CharacterStateButtonBashing(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
	m_moveSpeed(kInitMoveSpeed)
{
}

void CharacterStateButtonBashing::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kButtonBashing;

	//1Pか2Pかで最初の座標を変更する
	if (m_pCharacter->GetCharacterNumber() == Character::PlayerNumber::kOnePlayer)
	{
		SetCharacterPos(kPlayerInitPos);
	}
	else
	{
		SetCharacterPos(kEnemyInitPos);
	}

	//アニメーション変更
	m_pCharacter->ChangeAnim(Character::AnimKind::kButtonBashingHitBack,false);

	//ボタン連打を始めるとマネージャーに伝える
	StartButtonBashing();
}

void CharacterStateButtonBashing::Update()
{
	//敵の方向を向く
	m_pCharacter->LookTarget();

	//中心までのベクトル
	MyEngine::Vector3 toTarget = kTargetPos - m_pCharacter->GetPos();

	//中心に向かって移動する
	MyEngine::Vector3 moveDir = (toTarget).Normalize();

	//移動速度を足していく
	m_moveSpeed += kAcceleration;

	//移動速度をクランプ
	m_moveSpeed = fmin(m_moveSpeed,kMaxMoveSpeed);

	//敵との距離
	float toEnemyLength = (GetTargetPos() - m_pCharacter->GetPos()).Length();

	//敵とぶつかる距離になったら
	if (toEnemyLength < GameSceneConstant::kCharacterRadius)
	{
		if ()
		{
			//////////////////////////////////////
		}
	}

	//移動ベクトル
	MyEngine::Vector3 moveVec = moveDir * m_moveSpeed;

	//移動ベクトルを設定
	SetCharacterVelo(moveVec);

	//アニメーションの変更

	//移動速度が前方向になっていれば
	if (m_moveSpeed > 0.0f)
	{
		//アニメーションが攻撃中になっていなければ
		if (m_pCharacter->GetPlayAnimKind() != Character::AnimKind::kOnButtonBashing)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kOnButtonBashing,true);
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
