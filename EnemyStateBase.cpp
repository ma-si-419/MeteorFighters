#include "EnemyStateBase.h"
#include "EnemyStateIdle.h"
#include "EnemyStateMove.h"
#include "EnemyStateGuard.h"
#include "EnemyStateAttack.h"
#include "EnemyStateRush.h"
#include "EnemyStateHitAttack.h"
#include "EnemyStateButtonBashing.h"

void EnemyStateBase::ChangeState(CharacterStateKind nextState)
{
	//�I������
	Exit();

	//�A�C�h�����
	if (nextState == CharacterStateKind::kIdle)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<EnemyStateIdle>();
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
	//�ړ����
	else if (nextState == CharacterStateKind::kMove)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<EnemyStateMove>();
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
	//�K�[�h���
	else if (nextState == CharacterStateKind::kGuard)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<EnemyStateGuard>();
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
	//�U�����
	else if (nextState == CharacterStateKind::kAttack)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<EnemyStateAttack>();
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
	//�ˌ����
	else if (nextState == CharacterStateKind::kRush)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<EnemyStateRush>();
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
	//�U����e���
	else if (nextState == CharacterStateKind::kHitAttack)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<EnemyStateHitAttack>();
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
	//�{�^���A�ŏ��(�U���̉��������Ȃ�)
	else if (nextState == CharacterStateKind::kButtonBashing)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<EnemyStateButtonBashing>();
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
}
