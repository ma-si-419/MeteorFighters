#pragma once
#include "CharacterStateBase.h"

class Enemy;
class EnemyStateBase : public CharacterStateBase
{
protected:
	//Stateの切替
	void ChangeState(std::shared_ptr<CharacterStateBase> nextState) override;

	//そのStateに入って何フレーム立ったか
	int m_time = 0;
	
	/// <summary>
	/// プレイヤーの座標を取得する
	/// </summary>
	/// <returns>プレイヤーの座標</returns>
	MyEngine::Vector3 GetPlayerPos();

	//エネミーのポインタ
	std::shared_ptr<Enemy> m_pEnemy;
	
};

