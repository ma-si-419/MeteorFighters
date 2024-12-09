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
}

CharacterStateGuard::CharacterStateGuard(std::shared_ptr<CharacterBase> character) :
	CharacterStateBase(character)
{
}

void CharacterStateGuard::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kGuard;
	m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kGuard, true, kBlendSpeed);
}

void CharacterStateGuard::Update()
{
	//ガードボタンが押されている間このStateにいる
	if (m_isPlayer && !GetCharacterInput()->IsPress("B"))
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
