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

void PlayerStateBase::SetPlayerVelo(MyEngine::Vector3 velo)
{
	m_pPlayer->m_rigidbody.SetVelo(velo);
}

void PlayerStateBase::ChangeState(std::shared_ptr<CharacterStateBase> nextState)
{
	//�I������
	Exit();

	//����State��ύX����
	m_pNextState = nextState;

	//����State�̊J�n����
	m_pNextState->Enter();
}
