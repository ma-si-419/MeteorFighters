#include "EnemyStateBase.h"
#include "Enemy.h"
#include "GameManager.h"

void EnemyStateBase::ChangeState(std::shared_ptr<CharacterStateBase> nextState)
{
	//�I������
	Exit();

	m_pNextState = nextState;

	//����State�̊J�n����
	m_pNextState->Enter();
}

MyEngine::Vector3 EnemyStateBase::GetPlayerPos()
{
	return m_pEnemy->m_pGameManager->GetPlayerPos();
}
