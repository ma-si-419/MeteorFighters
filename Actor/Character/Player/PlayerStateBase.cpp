#include "PlayerStateBase.h"
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

void PlayerStateBase::ChangeState(std::shared_ptr<CharacterStateBase> nextState)
{
	//終了処理
	Exit();

	//次のStateを変更する
	m_pNextState = nextState;

	//次のStateの開始処理
	m_pNextState->Enter();
}
