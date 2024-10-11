#include "GameManager.h"
#include "Player.h"
#include "Enemy.h"
#include "AttackManager.h"
#include "LoadCsv.h"

GameManager::GameManager()
{
	
}

GameManager::~GameManager()
{
}

void GameManager::Update()
{
	m_pAttackManager->Update();
}

void GameManager::SetPlayerStatus(int number,std::vector<std::string> statusData)
{
	//プレイヤー作成
	m_pPlayer = std::make_shared<Player>(static_cast<CharacterBase::CharacterKind>(number));
	//プレイヤーに自分のポインターを渡しておく
	m_pPlayer->SetGameManager(shared_from_this());

	CharacterBase::CharacterStatus status;

	status.hp = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kHp)]);
	status.atk = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kAtk)]);
	status.def = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kDef)]);
	status.spd = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSpd)]);
	status.chargeSpd = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kChargeSpd)]);

	//プレイヤーのステータスを設定する
	m_pPlayer->SetStatus(status);
}

void GameManager::SetEnemyStatus(int number,std::vector<std::string> statusData)
{
	//エネミー作成
	m_pEnemy = std::make_shared<Enemy>(static_cast<CharacterBase::CharacterKind>(number));
	//エネミーに自分のポインターを渡しておく
	m_pEnemy->SetGameManager(shared_from_this());
	CharacterBase::CharacterStatus status;

	status.hp = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kHp)]);
	status.atk = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kAtk)]);
	status.def = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kDef)]);
	status.spd = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSpd)]);
	status.chargeSpd = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kChargeSpd)]);

	//エネミーのステータスを設定する
	m_pEnemy->SetStatus(status);
}

MyEngine::Vector3 GameManager::GetPlayerPos()
{
	return m_pPlayer->GetPos();
}

MyEngine::Vector3 GameManager::GetEnemyPos()
{
	return m_pEnemy->GetPos();
}

void GameManager::AddAttack(int attackNumber, bool isSpecial, bool isPlayer)
{
	float power = 0;
	if (isPlayer)
	{
		power = m_pPlayer->GetPower();
	}
	else
	{
		power = m_pEnemy->GetPower();
	}

	m_pAttackManager->AddAttack(attackNumber,isSpecial,isPlayer,power);
}
