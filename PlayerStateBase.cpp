#include "PlayerStateBase.h"
#include "PlayerStateIdle.h"
#include "PlayerStateMove.h"
#include "PlayerStateGuard.h"
#include "PlayerStateAttack.h"
#include "PlayerStateRush.h"
#include "PlayerStateHitAttack.h"
#include "PlayerStateButtonBashing.h"
#include "Player.h"
#include "GameManager.h"

PlayerStateBase::PlayerStateBase(std::shared_ptr<Player> player)
{
	m_pPlayer = player;
}

MyEngine::Vector3 PlayerStateBase::GetEnemyPos()
{
	return m_pPlayer->m_pGameManager->GetEnemyPos();
}

void PlayerStateBase::SetPlayerVelo(MyEngine::Vector3 velo)
{
	m_pPlayer->m_rigidbody.SetVelo(velo);
}

void PlayerStateBase::ChangeState(CharacterStateKind nextState)
{
	//�I������
	Exit();

	//�A�C�h�����
	if (nextState == CharacterStateKind::kIdle)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<PlayerStateIdle>(m_pPlayer);
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
	//�ړ����
	else if (nextState == CharacterStateKind::kMove)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<PlayerStateMove>(m_pPlayer);
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
	//�K�[�h���
	else if (nextState == CharacterStateKind::kGuard)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<PlayerStateGuard>(m_pPlayer);
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
	//�U�����
	else if (nextState == CharacterStateKind::kAttack)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<PlayerStateAttack>(m_pPlayer);
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
	//�ˌ����
	else if (nextState == CharacterStateKind::kRush)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<PlayerStateRush>(m_pPlayer);
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
	//�U����e���
	else if (nextState == CharacterStateKind::kHitAttack)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<PlayerStateHitAttack>(m_pPlayer);
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
	//�{�^���A�ŏ��(�U���̉��������Ȃ�)
	else if (nextState == CharacterStateKind::kButtonBashing)
	{
		//�|�C���^�̍쐬
		m_pNextState = std::make_shared<PlayerStateButtonBashing>(m_pPlayer);
		//����State�̊J�n����
		m_pNextState->Enter();
		return;
	}
}
