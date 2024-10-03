#include "PlayerStateButtonBashing.h"
#include "DxLib.h"
#include "Input.h"

void PlayerStateButtonBashing::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kButtonBashing;
}

void PlayerStateButtonBashing::Update()
{
#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:ButtonBashing", GetColor(255, 255, 255));
	
	if (MyEngine::Input::GetInstance().IsTrigger("A"))
	{
		ChangeState(CharacterStateKind::kIdle);
	}
#endif // _DEBUG

}
void PlayerStateButtonBashing::Exit()
{
}
