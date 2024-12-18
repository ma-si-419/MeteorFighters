#include "CharacterStateGuard.h"
#include "CharacterStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "CharacterBase.h"

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

CharacterStateGuard::CharacterStateGuard(std::shared_ptr<CharacterBase> character) :
	CharacterStateBase(character)
{
}

void CharacterStateGuard::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kGuard;
	m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kGuardMiddle, true, kBlendSpeed);
}

void CharacterStateGuard::Update()
{
	//上下入力でガードの向きを変える
	int stickY = GetCharacterInput()->GetStickInfo().leftStickY;

	//上に傾いていたら
	if (stickY < -kStickTiltPower)
	{
		//上ガードに変更する
		//上ガードは下方向に向けての吹っ飛ばし攻撃をガードできる

		//TODO : 今はアニメーションだけ変更
		if (m_pCharacter->GetPlayAnimKind() != CharacterBase::AnimKind::kGuardHigh)
		{
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kGuardHigh, true, kGuardSwitchBlendSpeed);
		}

	}
	//下に傾いていたら
	else if (stickY > kStickTiltPower)
	{
		//下ガードに変更する
		//下ガードは上方向に向けての吹っ飛ばし攻撃と下段ため攻撃をガードできる

		//TODO : 今はアニメーションだけ変更
		if (m_pCharacter->GetPlayAnimKind() != CharacterBase::AnimKind::kGuardLow)
		{
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kGuardLow, true, kGuardSwitchBlendSpeed);
		}

	}
	//どちらにも傾いていなければ
	else
	{
		if (m_pCharacter->GetPlayAnimKind() != CharacterBase::AnimKind::kGuardMiddle)
		{
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kGuardMiddle, true, kGuardSwitchBlendSpeed);
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
}
