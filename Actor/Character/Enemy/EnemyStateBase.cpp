#include "EnemyStateBase.h"
#include "Enemy.h"
#include "GameManager.h"

void EnemyStateBase::ChangeState(std::shared_ptr<CharacterStateBase> nextState)
{
	//I—¹ˆ—
	Exit();

	m_pNextState = nextState;

	//ŽŸ‚ÌState‚ÌŠJŽnˆ—
	m_pNextState->Enter();
}

MyEngine::Vector3 EnemyStateBase::GetPlayerPos()
{
	return m_pEnemy->m_pGameManager->GetPlayerPos();
}
