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

	void ChangeState(CharacterStateKind nextState) override;
	//プレイヤーのポインタ
	std::shared_ptr<Player> m_pPlayer;

};

