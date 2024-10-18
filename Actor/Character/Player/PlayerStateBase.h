#pragma once
#include "CharacterStateBase.h"
#include "Vector3.h"

class Player;
class PlayerStateBase : public CharacterStateBase
{
public:
	PlayerStateBase(std::shared_ptr<Player> player);
protected:

	MyEngine::Vector3 GetEnemyPos();

	void SetPlayerVelo(MyEngine::Vector3 velo);

	void ChangeState(std::shared_ptr<CharacterStateBase> nextState) override;
	
	//�v���C���[�̃|�C���^
	std::shared_ptr<Player> m_pPlayer;

};

