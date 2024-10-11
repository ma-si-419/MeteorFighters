#include "AttackManager.h"
#include "LoadCsv.h"
#include "Attack.h"

void AttackManager::Init(int playerCharacterNumber, int enemyCharacterNumber)
{
	LoadCsv load;

	std::vector<std::vector<std::string>> data = load.LoadFile("data/csv/specialAttackData.csv");

	//[0]は一つ目の必殺技の情報、[1]は二つ目の必殺技の情報

	//プレイヤーの必殺技の情報をセット
	m_playerSpecialAttackInfo[0].name = data[playerCharacterNumber][static_cast<int>(SpecialAttackDataSort::kFirstName)];
	m_playerSpecialAttackInfo[0].cost = std::stoi(data[playerCharacterNumber][static_cast<int>(SpecialAttackDataSort::kFirstCost)]);
	m_playerSpecialAttackInfo[0].damageRate = std::stof(data[playerCharacterNumber][static_cast<int>(SpecialAttackDataSort::kFirstDamageRate)]);
	m_playerSpecialAttackInfo[0].startFrame = std::stoi(data[playerCharacterNumber][static_cast<int>(SpecialAttackDataSort::kFirstStartFrame)]);
	m_playerSpecialAttackInfo[0].endFrame = std::stoi(data[playerCharacterNumber][static_cast<int>(SpecialAttackDataSort::kFirstEndFrame)]);
	m_playerSpecialAttackInfo[1].name = data[playerCharacterNumber][static_cast<int>(SpecialAttackDataSort::kSecondName)];
	m_playerSpecialAttackInfo[1].cost = std::stoi(data[playerCharacterNumber][static_cast<int>(SpecialAttackDataSort::kSecondCost)]);
	m_playerSpecialAttackInfo[1].damageRate = std::stof(data[playerCharacterNumber][static_cast<int>(SpecialAttackDataSort::kSecondDamageRate)]);
	m_playerSpecialAttackInfo[1].startFrame = std::stoi(data[playerCharacterNumber][static_cast<int>(SpecialAttackDataSort::kSecondStartFrame)]);
	m_playerSpecialAttackInfo[1].endFrame = std::stoi(data[playerCharacterNumber][static_cast<int>(SpecialAttackDataSort::kSecondEndFrame)]);

	//プレイヤーと同様にエネミーの技もセット
	m_enemySpecialAttackInfo[0].name = data[enemyCharacterNumber][static_cast<int>(SpecialAttackDataSort::kFirstName)];
	m_enemySpecialAttackInfo[0].cost = std::stoi(data[enemyCharacterNumber][static_cast<int>(SpecialAttackDataSort::kFirstCost)]);
	m_enemySpecialAttackInfo[0].damageRate = std::stof(data[enemyCharacterNumber][static_cast<int>(SpecialAttackDataSort::kFirstDamageRate)]);
	m_enemySpecialAttackInfo[0].startFrame = std::stoi(data[enemyCharacterNumber][static_cast<int>(SpecialAttackDataSort::kFirstStartFrame)]);
	m_enemySpecialAttackInfo[0].endFrame = std::stoi(data[enemyCharacterNumber][static_cast<int>(SpecialAttackDataSort::kFirstEndFrame)]);
	m_enemySpecialAttackInfo[1].name = data[enemyCharacterNumber][static_cast<int>(SpecialAttackDataSort::kSecondName)];
	m_enemySpecialAttackInfo[1].cost = std::stoi(data[enemyCharacterNumber][static_cast<int>(SpecialAttackDataSort::kSecondCost)]);
	m_enemySpecialAttackInfo[1].damageRate = std::stof(data[enemyCharacterNumber][static_cast<int>(SpecialAttackDataSort::kSecondDamageRate)]);
	m_enemySpecialAttackInfo[1].startFrame = std::stoi(data[enemyCharacterNumber][static_cast<int>(SpecialAttackDataSort::kSecondStartFrame)]);
	m_enemySpecialAttackInfo[1].endFrame = std::stoi(data[enemyCharacterNumber][static_cast<int>(SpecialAttackDataSort::kSecondEndFrame)]);

}

void AttackManager::Update()
{
	for (auto& item : m_pAttacks)
	{
		item->Update();
	}
}

void AttackManager::Draw()
{
	for (auto& item : m_pAttacks)
	{
		item->Draw();
	}
}

void AttackManager::AddAttack(int attackNumber, bool isSpecial, bool isPlayer,float power)
{
	
}