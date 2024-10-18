#include "PlayerStateHitAttack.h"
#include "PlayerStateIdle.h"
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
		std::shared_ptr<PlayerStateIdle> next = std::make_shared<PlayerStateIdle>(m_pPlayer);

		ChangeState(next);
	}
#endif // _DEBUG

}
void PlayerStateHitAttack::Exit()
{
}

void PlayerStateHitAttack::OnCollide(std::shared_ptr<Collidable> collider)
{
}
