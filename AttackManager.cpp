#include "AttackManager.h"
#include "LoadCsv.h"
#include "Attack.h"

void AttackManager::Init(int playerCharacterNumber, int enemyCharacterNumber)
{
	LoadCsv load;


	//必殺技の情報設定
	{
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
	//各キャラ共通で使用する通常攻撃の情報設定
	{
		std::vector<std::vector<std::string>> data = load.LoadFile("data/csv/normalAttackData.csv");

		for (auto item : data)
		{
			m_normalAttackInfo.damageRate = stof(item[static_cast<int>(NormalAttackDataSort::kDamageRate)]);
			m_normalAttackInfo.totalFrame = stoi(item[static_cast<int>(NormalAttackDataSort::kTotalFrame)]);
			m_normalAttackInfo.attackFrame = stoi(item[static_cast<int>(NormalAttackDataSort::kAttackFrame)]);
			m_normalAttackInfo.animationName = item[static_cast<int>(NormalAttackDataSort::kAttackFrame)];

			//一旦情報を保存する変数
			std::string temp;

			//受けた時のやられ状態
			temp = item[static_cast<int>(NormalAttackDataSort::kHitReaction)];

			HitReaction hitReaction;
			if (temp == "弱")
			{
				hitReaction = HitReaction::kRow;
			}
			else if (temp == "中")
			{
				hitReaction = HitReaction::kMid;
			}
			else if (temp == "強")
			{
				hitReaction = HitReaction::kHigh;
			}
			else if (temp == "吹っ飛び")
			{
				hitReaction = HitReaction::kBurst;
			}

			m_normalAttackInfo.hitReaction = hitReaction;

			//攻撃を受けた時の吹き飛ぶ方向
			temp = item[static_cast<int>(NormalAttackDataSort::kHitDirection)];

			HitDirection hitDirection;

			if (temp == "奥")
			{
				hitDirection = HitDirection::kFar;
			}
			else if (temp == "上")
			{
				hitDirection = HitDirection::kUp;
			}
			else if (temp == "下")
			{
				hitDirection = HitDirection::kDown;
			}

			m_normalAttackInfo.hitDirection = hitDirection;

			//吹き飛ぶ強さ
			temp = item[static_cast<int>(NormalAttackDataSort::kBurstPower)];

			BurstPower burstPower;

			if (temp == "無")
			{
				burstPower = BurstPower::kNone;
			}
			else if (temp == "弱")
			{
				burstPower = BurstPower::kRow;
			}
			else if (temp == "中")
			{
				burstPower = BurstPower::kMid;
			}
			else if (temp == "強")
			{
				burstPower = BurstPower::kHigh;
			}

			m_normalAttackInfo.burstPower = burstPower;
		}

	}

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

void AttackManager::AddAttack(AddAttackInfo attackInfo)
{
	//追加する攻撃の作成
	std::shared_ptr<Attack> attack = std::make_shared<Attack>();

	//追加する攻撃のステータス
	Attack::AttackStatus status;


	attack->Init(status);
}
