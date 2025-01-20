#include "CharacterStateGuard.h"
#include "CharacterStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "Character.h"

namespace
{
	//攻撃を回避できる時間
	constexpr float kDodgeTime = 15;
	//アニメーションブレンドの速さ
	constexpr float kBlendSpeed = 1.0f / kDodgeTime;
	//スティックが傾いていると判断する傾きの大きさ
	constexpr int kStickTiltPower = 300;
	//上下を切り替えるときのブレンドの速さ
	constexpr float kGuardSwitchBlendSpeed = 0.16f;
}

CharacterStateGuard::CharacterStateGuard(std::shared_ptr<Character> character) :
	CharacterStateBase(character)
{
}

void CharacterStateGuard::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kGuard;
	m_pCharacter->ChangeAnim(Character::AnimKind::kGuardMiddle, true, kBlendSpeed);
	m_guardKind = CharacterGuardKind::kMiddleGuard;
}

void CharacterStateGuard::Update()
{
	//上下入力でガードの向きを変える
	int stickY = GetCharacterInput()->GetStickInfo().leftStickY;

	//上に傾いていたら
	if (stickY < -kStickTiltPower)
	{
		//上ガードに変更する
		m_guardKind = CharacterGuardKind::kUpGuard;

		//アニメーションを変更
		if (m_pCharacter->GetPlayAnimKind() != Character::AnimKind::kGuardHigh)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kGuardHigh, true, kGuardSwitchBlendSpeed);
		}

	}
	//下に傾いていたら
	else if (stickY > kStickTiltPower)
	{
		//下ガードに変更する
		m_guardKind = CharacterGuardKind::kDownGuard;

		//アニメーションを変更
		if (m_pCharacter->GetPlayAnimKind() != Character::AnimKind::kGuardLow)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kGuardLow, true, kGuardSwitchBlendSpeed);
		}

	}
	//どちらにも傾いていなければ
	else
	{
		//中段ガードに変更する
		m_guardKind = CharacterGuardKind::kMiddleGuard;

		//アニメーションを変更
		if (m_pCharacter->GetPlayAnimKind() != Character::AnimKind::kGuardMiddle)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kGuardMiddle, true, kGuardSwitchBlendSpeed);
		}
	}


	//ガードボタンが押されている間このStateにいる
	if (!GetCharacterInput()->IsPress("B"))
	{
		//ガードボタンが離されたら
		std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		ChangeState(next);
	}


#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Guard", GetColor(255, 255, 255));

#endif // _DEBUG

}
void CharacterStateGuard::Exit()
{
	m_guardKind = CharacterGuardKind::kNone;
}
