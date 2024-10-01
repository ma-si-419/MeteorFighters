#include "GameManager.h"
#include "Player.h"
#include "Enemy.h"

GameManager::GameManager()
{
}

GameManager::~GameManager()
{
}

void GameManager::SetPlayer(std::shared_ptr<Player> player)
{
	m_pPlayer = player;
}

void GameManager::SetEnemy(std::shared_ptr<Enemy> enemy)
{
	m_pEnemy = enemy;
}

MyEngine::Vector3 GameManager::GetPlayerPos()
{
	return m_pPlayer->GetPos();
}

MyEngine::Vector3 GameManager::GetEnemyPos()
{
	return m_pEnemy->GetPos();
}

