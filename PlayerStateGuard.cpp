#include "PlayerStateGuard.h"
#include "DxLib.h"
#include "Input.h"


void PlayerStateGuard::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kGuard;
}

void PlayerStateGuard::Update()
{
#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Guard", GetColor(255, 255, 255));

	if (MyEngine::Input::GetInstance().IsTrigger("A"))
	{
		ChangeState(CharacterStateKind::kIdle);
	}
#endif // _DEBUG

}
void PlayerStateGuard::Exit()
{
}
