#include "EnemyStateBase.h"
#include "EnemyStateIdle.h"
#include "EnemyStateMove.h"
#include "EnemyStateGuard.h"
#include "EnemyStateAttack.h"
#include "EnemyStateRush.h"
#include "EnemyStateHitAttack.h"
#include "EnemyStateButtonBashing.h"

void EnemyStateBase::ChangeState(std::shared_ptr<CharacterStateBase> nextState)
{
	//�I������
	Exit();

	m_pNextState = nextState;

	//����State�̊J�n����
	m_pNextState->Enter();
}
