#include "PlayerStateIdle.h"
#include "DxLib.h"
#include "Input.h"


void PlayerStateIdle::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kIdle;
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

}
void PlayerStateIdle::Exit()
{
}
