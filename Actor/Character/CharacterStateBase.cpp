#include "CharacterStateBase.h"

void CharacterStateBase::ChangeState(std::shared_ptr<CharacterStateBase> nextState)
{
	//�I������
	Exit();

	//����State�̐ݒ�
	m_pNextState = nextState;

	//����State�̊J�n����
	m_pNextState->Enter();
}
