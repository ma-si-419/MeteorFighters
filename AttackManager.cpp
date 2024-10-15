#include "AttackManager.h"
#include "LoadCsv.h"
#include "Attack.h"

void AttackManager::Init(int playerCharacterNumber, int enemyCharacterNumber)
{
	LoadCsv load;


	//�K�E�Z�̏��ݒ�
	{
		std::vector<std::vector<std::string>> data = load.LoadFile("data/csv/specialAttackData.csv");

		//[0]�͈�ڂ̕K�E�Z�̏��A[1]�͓�ڂ̕K�E�Z�̏��

		//�v���C���[�̕K�E�Z�̏����Z�b�g
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

		//�v���C���[�Ɠ��l�ɃG�l�~�[�̋Z���Z�b�g
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
	//�e�L�������ʂŎg�p����ʏ�U���̏��ݒ�
	{
		std::vector<std::vector<std::string>> data = load.LoadFile("data/csv/normalAttackData.csv");

		for (auto item : data)
		{
			m_normalAttackInfo.damageRate = stof(item[static_cast<int>(NormalAttackDataSort::kDamageRate)]);
			m_normalAttackInfo.totalFrame = stoi(item[static_cast<int>(NormalAttackDataSort::kTotalFrame)]);
			m_normalAttackInfo.attackFrame = stoi(item[static_cast<int>(NormalAttackDataSort::kAttackFrame)]);
			m_normalAttackInfo.animationName = item[static_cast<int>(NormalAttackDataSort::kAttackFrame)];

			//��U����ۑ�����ϐ�
			std::string temp;

			//�󂯂����̂�����
			temp = item[static_cast<int>(NormalAttackDataSort::kHitReaction)];

			HitReaction hitReaction;
			if (temp == "��")
			{
				hitReaction = HitReaction::kRow;
			}
			else if (temp == "��")
			{
				hitReaction = HitReaction::kMid;
			}
			else if (temp == "��")
			{
				hitReaction = HitReaction::kHigh;
			}
			else if (temp == "�������")
			{
				hitReaction = HitReaction::kBurst;
			}

			m_normalAttackInfo.hitReaction = hitReaction;

			//�U�����󂯂����̐�����ԕ���
			temp = item[static_cast<int>(NormalAttackDataSort::kHitDirection)];

			HitDirection hitDirection;

			if (temp == "��")
			{
				hitDirection = HitDirection::kFar;
			}
			else if (temp == "��")
			{
				hitDirection = HitDirection::kUp;
			}
			else if (temp == "��")
			{
				hitDirection = HitDirection::kDown;
			}

			m_normalAttackInfo.hitDirection = hitDirection;

			//������ԋ���
			temp = item[static_cast<int>(NormalAttackDataSort::kBurstPower)];

			BurstPower burstPower;

			if (temp == "��")
			{
				burstPower = BurstPower::kNone;
			}
			else if (temp == "��")
			{
				burstPower = BurstPower::kRow;
			}
			else if (temp == "��")
			{
				burstPower = BurstPower::kMid;
			}
			else if (temp == "��")
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
	//�ǉ�����U���̍쐬
	std::shared_ptr<Attack> attack = std::make_shared<Attack>();

	//�ǉ�����U���̃X�e�[�^�X
	Attack::AttackStatus status;


	attack->Init(status);
}
