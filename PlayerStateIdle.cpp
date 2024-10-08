#include "PlayerStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"

PlayerStateIdle::PlayerStateIdle(std::shared_ptr<Player> player) :
	PlayerStateBase(player)
{
}

void PlayerStateIdle::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kIdle;
	m_pPlayer->ChangeAnim(0, true);
}

void PlayerStateIdle::Update()
{

#ifdef _DEBUG

		DrawString(0, 16, "PlayerState:Idle", GetColor(255, 255, 255));

	if (MyEngine::Input::GetInstance().IsTrigger("A"))
	{
		ChangeState(CharacterStateKind::kAttack);
	}
	else if (MyEngine::Input::GetInstance().IsTrigger("B"))
	{
		ChangeState(CharacterStateKind::kGuard);
	}
	else if (MyEngine::Input::GetInstance().IsTrigger("X"))
	{
		ChangeState(CharacterStateKind::kHitAttack);
	}
	else if (MyEngine::Input::GetInstance().IsTrigger("Y"))
	{
		ChangeState(CharacterStateKind::kMove);
	}
	else if (MyEngine::Input::GetInstance().IsTrigger("Lb"))
	{
		ChangeState(CharacterStateKind::kButtonBashing);
	}
	else if (MyEngine::Input::GetInstance().IsTrigger("Rb"))
	{
		ChangeState(CharacterStateKind::kRush);
	}
#endif // _DEBUG

	m_pPlayer->PlayAnim();

	auto& input = MyEngine::Input::GetInstance();

	//ˆÚ“®“ü—Í‚ª‚³‚ê‚Ä‚¢‚½‚ç
	if (input.GetStickInfo().leftStickX != 0 ||
		input.GetStickInfo().leftStickY != 0)
	{
		ChangeState(CharacterStateKind::kMove);
		return;
	}


	SetPlayerVelo(MyEngine::Vector3(0, 0, 0));

}
void PlayerStateIdle::Exit()
{

}
