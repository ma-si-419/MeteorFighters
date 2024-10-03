#include "PlayerStateBase.h"
#include "PlayerStateIdle.h"
#include "PlayerStateMove.h"
#include "PlayerStateGuard.h"
#include "PlayerStateAttack.h"
#include "PlayerStateRush.h"
#include "PlayerStateHitAttack.h"
#include "PlayerStateButtonBashing.h"

void PlayerStateBase::ChangeState(CharacterStateKind nextState)
{
	//�I������
	Exit();

	//�A�C�h�����
	if (nextState == CharacterStateKind::kIdle)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<PlayerStateIdle>();
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
	//�ړ����
	else if (nextState == CharacterStateKind::kMove)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<PlayerStateMove>();
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
	//�K�[�h���
	else if (nextState == CharacterStateKind::kGuard)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<PlayerStateGuard>();
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
	//�U�����
	else if (nextState == CharacterStateKind::kAttack)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<PlayerStateAttack>();
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
	//�ˌ����
	else if (nextState == CharacterStateKind::kRush)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<PlayerStateRush>();
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
	//�U����e���
	else if (nextState == CharacterStateKind::kHitAttack)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<PlayerStateHitAttack>();
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
	//�{�^���A�ŏ��(�U���̉��������Ȃ�)
	else if (nextState == CharacterStateKind::kButtonBashing)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<PlayerStateButtonBashing>();
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
}
