#include "PlayerStateBase.h"
#include "Player.h"
#include "GameManager.h"

PlayerStateBase::PlayerStateBase(std::shared_ptr<Player> player)
{
	m_pPlayer = player;
}

MyEngine::Vector3 PlayerStateBase::GetEnemyPos()
{
	return m_pPlayer->m_pGameManager->GetEnemyPos();
}

MyEngine::Vector3 PlayerStateBase::GetEnemyVelo()
{
	return m_pPlayer->m_pGameManager->GetEnemyVelo();
}

int PlayerStateBase::GetEnemyHitReaction()
{
	return m_pPlayer->m_pGameManager->GetEnemyReactionKind();
}

void PlayerStateBase::SetPlayerVelo(MyEngine::Vector3 velo)
{
	m_pPlayer->m_rigidbody.SetVelo(velo);
}

void PlayerStateBase::SetPlayerPos(MyEngine::Vector3 pos)
{
	m_pPlayer->m_rigidbody.SetPos(pos);
}

void PlayerStateBase::SetPlayerBlur(bool flag)
{
	m_pPlayer->SetBlur(flag);
}

int PlayerStateBase::GetAttackAnimKind(std::string animName)
{
	return static_cast<int>(m_pPlayer->GetAttackAnimKind(animName));
}

float PlayerStateBase::GetSpeed()
{
	return m_pPlayer->GetSpeed();
}
