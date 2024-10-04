#include "PlayerStateHitAttack.h"
#include "DxLib.h"
#include "Input.h"


PlayerStateHitAttack::PlayerStateHitAttack(std::shared_ptr<Player> player):
	PlayerStateBase(player)
{
}

void PlayerStateHitAttack::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kHitAttack;
}

void PlayerStateHitAttack::Update()
{
#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:HitAttack", GetColor(255, 255, 255));

	if (MyEngine::Input::GetInstance().IsTrigger("A"))
	{
		ChangeState(CharacterStateKind::kIdle);
	}
#endif // _DEBUG

}
void PlayerStateHitAttack::Exit()
{
}
