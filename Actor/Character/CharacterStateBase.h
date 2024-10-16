#pragma once
#include "StateBase.h"
class CharacterStateBase : public StateBase
{
public:
	enum class CharacterStateKind
	{
		kIdle,//�������Ă��Ȃ����
		kMove,//�ړ����
		kGuard,//�K�[�h���
		kAttack,//�U�����
		kRush,//�ˌ����
		kHitAttack,//�U�����󂯂���
		kButtonBashing//�G�Ƃ̉�������
	};

	CharacterStateKind GetKind() { return m_kind; }

	//����State
	std::shared_ptr<CharacterStateBase> m_pNextState;

protected:
	
	virtual void ChangeState(std::shared_ptr<CharacterStateBase> nextState) abstract;

	//���݂̏��
	CharacterStateKind m_kind;

	//State�ɓ����ĉ��t���[����������
	int m_time = 0;

};

