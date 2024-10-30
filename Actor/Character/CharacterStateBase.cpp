#include "CharacterStateBase.h"

void CharacterStateBase::ChangeState(std::shared_ptr<CharacterStateBase> nextState)
{
	//I—¹ˆ—
	Exit();

	//ŽŸ‚ÌState‚ÌÝ’è
	m_pNextState = nextState;

	//ŽŸ‚ÌState‚ÌŠJŽnˆ—
	m_pNextState->Enter();
}
