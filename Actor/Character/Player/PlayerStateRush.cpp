#include "PlayerStateRush.h"
#include "PlayerStateIdle.h"
#include "DxLib.h"
#include "Input.h"

PlayerStateRush::PlayerStateRush(std::shared_ptr<Player> player):
	PlayerStateBase(player)
{
}

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
		std::shared_ptr<PlayerStateIdle> next = std::make_shared<PlayerStateIdle>(m_pPlayer);

		ChangeState(next);
	}
#endif // _DEBUG

}
void PlayerStateRush::Exit()
{
}

void PlayerStateRush::OnCollide(std::shared_ptr<Collidable> collider)
{
}
