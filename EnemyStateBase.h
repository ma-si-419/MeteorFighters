#pragma once
#include "CharacterStateBase.h"

class EnemyStateBase : public CharacterStateBase
{
protected:
	//State�̐ؑ�
	void ChangeState(CharacterStateKind nextState) override;
#ifdef _DEBUG

	//����State�ɓ����ĉ��t���[����������
	int m_time = 0;

#endif // _DEBUG

	
};

