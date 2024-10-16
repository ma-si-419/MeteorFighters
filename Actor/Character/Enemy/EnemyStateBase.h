#pragma once
#include "CharacterStateBase.h"

class EnemyStateBase : public CharacterStateBase
{
protected:
	//State�̐ؑ�
	void ChangeState(std::shared_ptr<CharacterStateBase> nextState) override;
#ifdef _DEBUG

	//����State�ɓ����ĉ��t���[����������
	int m_time = 0;

#endif // _DEBUG

	
};

