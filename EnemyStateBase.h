#pragma once
#include "CharacterStateBase.h"

class EnemyStateBase : public CharacterStateBase
{
protected:
	//Stateの切替
	void ChangeState(CharacterStateKind nextState) override;
#ifdef _DEBUG

	//そのStateに入って何フレーム立ったか
	int m_time = 0;

#endif // _DEBUG

	
};

