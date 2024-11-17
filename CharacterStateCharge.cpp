#include "CharacterStateCharge.h"
#include "CharacterStateIdle.h"
#include "CharacterStateRush.h"
#include "CharacterBase.h"
#include "Input.h"
#include "DxLib.h"

namespace
{
	//チャージ終了アニメーションの時間
	constexpr int kEndChargeTime = 10;
}

CharacterStateCharge::CharacterStateCharge(std::shared_ptr<CharacterBase> character) :
	CharacterStateBase(character)
{
}

void CharacterStateCharge::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kCharge;
	m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kStartCharge, false);
}

void CharacterStateCharge::Update()
{

	auto& input = MyEngine::Input::GetInstance();

	m_pCharacter->ChargeMp();

	//チャージ開始を再生していて
	if (m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kStartCharge)
	{
		//アニメーションが終わっていたら
		if (m_pCharacter->IsEndAnim())
		{
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kInCharge, true);
		}
	}

	//レフトショルダーが押されなくなったらアイドルStateに戻る
	if (m_isPlayer && !input.IsPushTrigger(false))
	{
		auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		next->SetEndAnim(static_cast<int>(CharacterBase::AnimKind::kEndCharge), kEndChargeTime);

		ChangeState(next);

		return;
	}

	//レフトショルダーを押しながらAボタンが押されたら
	if (m_isPlayer && input.IsTrigger("A"))
	{
		MyEngine::Vector3 moveDir(input.GetStickInfo().leftStickX, 0, input.GetStickInfo().leftStickY);

		//突撃Stateに移行する
		auto next = std::make_shared<CharacterStateRush>(m_pCharacter);

		if (moveDir.SqLength() > 0.01f)
		{
			moveDir = moveDir.Normalize();
			next->SetMoveDir(moveDir);

			ChangeState(next);

			return;
		}

		moveDir = MyEngine::Vector3(0, 0, 1);

		next->SetMoveDir(moveDir);

		ChangeState(next);

		return;

	}

	SetCharacterVelo(MyEngine::Vector3(0, 0, 0));


#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Charge", GetColor(255, 255, 255));

#endif // _DEBUG
}

void CharacterStateCharge::Exit()
{
}