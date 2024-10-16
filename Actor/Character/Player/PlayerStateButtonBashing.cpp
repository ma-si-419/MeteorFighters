#include "PlayerStateButtonBashing.h"
#include "PlayerStateIdle.h"
#include "DxLib.h"
#include "Input.h"

PlayerStateButtonBashing::PlayerStateButtonBashing(std::shared_ptr<Player> player):
	PlayerStateBase(player)
{
}

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
		std::shared_ptr<PlayerStateIdle> next = std::make_shared<PlayerStateIdle>(m_pPlayer);

		ChangeState(next);
	}
#endif // _DEBUG

}
void PlayerStateButtonBashing::Exit()
{
}
