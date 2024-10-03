#include "PlayerStateMove.h"
#include "DxLib.h"
#include "Input.h"

void PlayerStateMove::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kMove;
}

void PlayerStateMove::Update()
{
#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Move", GetColor(255, 255, 255));

	if (MyEngine::Input::GetInstance().IsTrigger("A"))
	{
		ChangeState(CharacterStateKind::kIdle);
	}
#endif // _DEBUG

}
void PlayerStateMove::Exit()
{
}
