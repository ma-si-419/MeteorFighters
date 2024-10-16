#include "PlayerStateNormalAttack.h"
#include "PlayerStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"
PlayerStateNormalAttack::PlayerStateNormalAttack(std::shared_ptr<Player> player):
	PlayerStateBase(player),
	m_nowAttackName("empty")
{
}
void PlayerStateNormalAttack::SetAttack(std::string attackName)
{
	m_nowAttackName = attackName;
}
void PlayerStateNormalAttack::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kAttack;
}

void PlayerStateNormalAttack::Update()
{
	auto attackData = m_pPlayer->GetNormalAttackData(m_nowAttackName);

	//合計フレーム数よりも長くStateにいたら
	if (attackData.totalFrame < m_time)
	{
		std::shared_ptr<PlayerStateIdle> next = std::make_shared<PlayerStateIdle>();

		ChangeState(next);
		return;
	}

#ifdef _DEBUG

	DrawString(0,16,"PlayerState:Attack", GetColor(255, 255, 255));

#endif // _DEBUG

	//Stateにいるフレーム数を数えておく
	m_time++;
}

void PlayerStateNormalAttack::Exit()
{
}
