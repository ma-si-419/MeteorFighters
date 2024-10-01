#pragma once
#include <memory>
#include "Vector3.h"
class Player;
class Enemy;
class GameManager
{
public:

	GameManager();
	~GameManager();

	
	void SetPlayer(std::shared_ptr<Player> player);

	void SetEnemy(std::shared_ptr<Enemy> enemy);

	MyEngine::Vector3 GetPlayerPos();
	MyEngine::Vector3 GetEnemyPos();

private:

	//プレイヤーのポインタ
	std::shared_ptr<Player>  m_pPlayer;
	//エネミーのポインタ
	std::shared_ptr<Enemy> m_pEnemy;

};