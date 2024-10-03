#include "PlayerStateRush.h"
#include "DxLib.h"
#include "Input.h"

void PlayerStateRush::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kRush;
}

void PlayerStateRush::Update()
{
#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Rush", GetColor(255, 255, 255));

	if (MyEngine::Input::GetInstance().IsTrigger("A"))
	{
		ChangeState(CharacterStateKind::kIdle);
	}
#endif // _DEBUG

}
void PlayerStateRush::Exit()
{
}
