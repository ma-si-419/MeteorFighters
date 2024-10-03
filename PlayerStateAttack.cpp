#include "PlayerStateAttack.h"
#include "DxLib.h"
#include "Input.h"

void PlayerStateAttack::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kAttack;
}

void PlayerStateAttack::Update()
{
#ifdef _DEBUG

	DrawString(0,16,"PlayerState:Attack", GetColor(255, 255, 255));

	if (MyEngine::Input::GetInstance().IsTrigger("A"))
	{
		ChangeState(CharacterStateKind::kIdle);
	}

#endif // _DEBUG

}

void PlayerStateAttack::Exit()
{
}
