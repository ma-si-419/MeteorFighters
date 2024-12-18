#include "EnemyInput.h"
#include "GameManager.h"
#include "CharacterStateBase.h"
#include "CharacterStateNormalAttack.h"
#include "LoadCsv.h"
#include "GameSceneConstant.h"

namespace
{
	//�s�����s���܂ł̎���
	constexpr int kActionTime = 60;

	//�`���[�W���s������
	constexpr int kChargeTime = 120;

	//�������Ɣ��f���鋗��
	constexpr float kMiddleDistance = 150.0f;

	//�ߋ����Ɣ��f���鋗��
	constexpr float kNearDistance = 80.0f;

	//�ړ��������X�V���鎞��
	constexpr int kMoveUpdateTime = 120;

	//�ړ������̃����_���̐�
	constexpr int kMoveDirRandomNum = 3;

	//�ߋ����őI���ł���ړ�����
	const EnemyInput::MoveDir kNearMoveDir[kMoveDirRandomNum] =
	{
		EnemyInput::MoveDir::kBack,
		EnemyInput::MoveDir::kRight,
		EnemyInput::MoveDir::kLeft
	};

	//�������őI���ł���ړ�����
	const EnemyInput::MoveDir kMiddleMoveDir[kMoveDirRandomNum] =
	{
		EnemyInput::MoveDir::kFront,
		EnemyInput::MoveDir::kRight,
		EnemyInput::MoveDir::kLeft
	};

	//�������őI���ł���ړ�����
	const EnemyInput::MoveDir kFarMoveDir[kMoveDirRandomNum] =
	{
		EnemyInput::MoveDir::kFront,
		EnemyInput::MoveDir::kRight,
		EnemyInput::MoveDir::kLeft
	};
}

EnemyInput::EnemyInput(std::shared_ptr<MyEngine::InputData> inputData) :
	m_moveTime(0),
	m_stateTime(0),
	m_actionTime(0),
	m_isCountActionTime(true)
{
	m_pInputData = inputData;
	m_moveFunc = &EnemyInput::None;
	m_actionFunc = &EnemyInput::None;

	LoadCsv load;

	auto aiData = load.LoadFile("data/csv/enemyAiData.csv");

	for (auto item : aiData)
	{
		AiData pushData;

		pushData.nearRate = stoi(item[static_cast<int>(DataIndex::kNearRate)]);
		pushData.middleRate = stoi(item[static_cast<int>(DataIndex::kMiddleRate)]);
		pushData.farRate = stoi(item[static_cast<int>(DataIndex::kFarRate)]);
		pushData.minMp = stoi(item[static_cast<int>(DataIndex::kMinMp)]);

		m_aiData.push_back(pushData);
	}

}

EnemyInput::~EnemyInput()
{

}

void EnemyInput::Update()
{
	auto player = m_pManager->GetOnePlayerPointer();
	auto enemy = m_pManager->GetTwoPlayerPointer();

	//�v���C���[�Ƃ̋���
	float toPlayerLength = (player->GetPos() - enemy->GetPos()).Length();
	m_moveTime--;

	//��莞�Ԃ��ƂɈړ�������ݒ肷��
	if (m_moveTime < 0)
	{
		m_moveTime = kMoveUpdateTime;

		//�v���C���[�Ƃ̋����ňړ������𔻒f����

		//�����_���Ȑ����擾����
		int rand = GetRand(kMoveDirRandomNum - 1);
		//�ړ�����
		MoveDir moveDir;

		//�߂��Ɣ��f������
		if (toPlayerLength < kNearDistance)
		{
			moveDir = kNearMoveDir[rand];
		}
		//���������Ɣ��f������
		else if (toPlayerLength < kMiddleDistance)
		{
			moveDir = kMiddleMoveDir[rand];
		}
		//���������Ɣ��f������
		else
		{
			moveDir = kFarMoveDir[rand];
		}

		if (moveDir == MoveDir::kFront)
		{
			m_moveFunc = &EnemyInput::MoveFront;
		}
		else if (moveDir == MoveDir::kBack)
		{
			m_moveFunc = &EnemyInput::MoveBack;
		}
		else if (moveDir == MoveDir::kRight)
		{
			m_moveFunc = &EnemyInput::MoveRight;
		}
		else if (moveDir == MoveDir::kLeft)
		{
			m_moveFunc = &EnemyInput::MoveLeft;
		}
	}

	//State�����ꂩ������^�C�~���O��
	if (m_pEnemyState != m_pEnemyState->m_pNextState)
	{
		//State�ɂ��鎞�Ԃ����Z�b�g����
		//m_stateTime = 0;
	}

	if (m_isCountActionTime)
	{
		m_actionTime++;

		m_actionFunc = &EnemyInput::None;

		//�s���s����I������Ƃ�
		if (m_actionTime > kActionTime)
		{
			//�����_���Ŏg�p���鐔���v�Z
			int randNum = 0;

			for (auto item : m_aiData)
			{
				//�߂��Ɣ��f������
				if (toPlayerLength < kNearDistance)
				{
					//�C�͗ʂ�����Ă���Ȃ�
					if (enemy->GetMp() >= item.minMp)
					{
						randNum += item.nearRate;
					}
				}
				//���������Ɣ��f������
				else if (toPlayerLength < kMiddleDistance)
				{
					//�C�͗ʂ�����Ă���Ȃ�
					if (enemy->GetMp() >= item.minMp)
					{
						randNum += item.middleRate;
					}
				}
				//���������Ɣ��f������
				else
				{
					//�C�͗ʂ�����Ă���Ȃ�
					if (enemy->GetMp() >= item.minMp)
					{
						randNum += item.farRate;
					}
				}
			}

			//�x�̍s�������邩�������_���Ō���
			int actionNum = GetRand(randNum);

			Action action = Action::PhysicalAttack;

			for (auto item : m_aiData)
			{
				//�߂��Ɣ��f������
				if (toPlayerLength < kNearDistance)
				{
					//�C�͗ʂ�����Ă���Ȃ�
					if (enemy->GetMp() >= item.minMp)
					{
						actionNum -= item.nearRate;
					}
				}
				//���������Ɣ��f������
				else if (toPlayerLength < kMiddleDistance)
				{
					//�C�͗ʂ�����Ă���Ȃ�
					if (enemy->GetMp() >= item.minMp)
					{
						actionNum -= item.middleRate;
					}
				}
				//���������Ɣ��f������
				else
				{
					//�C�͗ʂ�����Ă���Ȃ�
					if (enemy->GetMp() >= item.minMp)
					{
						actionNum -= item.farRate;
					}
				}

				//�s�������܂�����
				if (actionNum <= 0)
				{
					switch (action)
					{
					case EnemyInput::Action::PhysicalAttack:
						m_actionFunc = &EnemyInput::PhysicalAttack;
						break;
					case EnemyInput::Action::EnergyAttack:
						m_actionFunc = &EnemyInput::EnergyAttack;
						break;
					case EnemyInput::Action::Dash:
						m_actionFunc = &EnemyInput::Dash;
						break;
					case EnemyInput::Action::Rush:
						m_actionFunc = &EnemyInput::Rush;
						break;
					case EnemyInput::Action::SpecialAttack:
						m_actionFunc = &EnemyInput::SpecialAttack;
						break;
					case EnemyInput::Action::EnergyCharge:
						m_actionFunc = &EnemyInput::EnergyCharge;
						break;
					default:
						m_actionFunc = &EnemyInput::None;
						break;
					}
					//���[�v���甲����
					break;
				}

				action = static_cast<Action>(static_cast<int>(action) + 1);
			}

			m_isCountActionTime = false;
		}
	}
	else
	{
		m_actionTime = 0;
	}

	//�ړ�����
	//(this->*m_moveFunc)();

	//�A�N�V��������
	//(this->*m_actionFunc)();
}

void EnemyInput::MoveFront()
{
	m_pInputData->TiltStick(MyEngine::Vector2(0, -1000), true);
}

void EnemyInput::MoveRight()
{
	m_pInputData->TiltStick(MyEngine::Vector2(1000, 0), true);
}

void EnemyInput::MoveLeft()
{
	m_pInputData->TiltStick(MyEngine::Vector2(-1000, 0), true);
}

void EnemyInput::MoveBack()
{
	m_pInputData->TiltStick(MyEngine::Vector2(0, 1000), true);
}

void EnemyInput::Dash()
{

	m_pInputData->PushButton("A");

	m_isCountActionTime = true;
}

void EnemyInput::Rush()
{

	m_pInputData->PushTrigger(true);

	m_pInputData->BashButton("A");

	auto player = m_pManager->GetOnePlayerPointer();
	auto enemy = m_pManager->GetTwoPlayerPointer();

	float distance = (player->GetPos() - enemy->GetPos()).Length();

	if (distance < kNearDistance)
	{
		m_isCountActionTime = true;
	}

}

void EnemyInput::SpecialAttack()
{

	m_pInputData->PushTrigger(true);

	if (m_pEnemyState->GetKind() == CharacterStateBase::CharacterStateKind::kCharge)
	{
		m_pInputData->PushButton("Y");

		m_isCountActionTime = true;
	}
}

void EnemyInput::EnergyCharge()
{
	m_stateTime++;

	m_pInputData->PushTrigger(true);

	auto player = m_pManager->GetOnePlayerPointer();
	auto enemy = m_pManager->GetTwoPlayerPointer();

	float distance = (player->GetPos() - enemy->GetPos()).Length();

	//��莞�ԃ`���[�W�A�G���߂��ɗ���AMP�}�b�N�X�̂ǂꂩ�ɂȂ�ΏI��
	if (m_stateTime > kChargeTime || distance < kNearDistance || enemy->GetMp() >= GameSceneConstant::kMaxMp)
	{
		//�`���[�W����߂�
		m_isCountActionTime = true;
		m_stateTime = 0;
	}

}

void EnemyInput::PhysicalAttack()
{

	m_pInputData->BashButton("X");

	//�U��State�ɓ����Ă�����
	if (m_pEnemyState->GetKind() == CharacterStateBase::CharacterStateKind::kNormalAttack)
	{
		auto attackState = std::dynamic_pointer_cast<CharacterStateNormalAttack>(m_pEnemyState);

		//�U���I���̃t���O�������Ă�����
		if (attackState->GetNowAttackName() == "Low8" || attackState->GetEndAttack())
		{
			m_isCountActionTime = true;
		}
	}
	
	auto player = m_pManager->GetOnePlayerPointer();
	auto enemy = m_pManager->GetTwoPlayerPointer();

	float distance = (player->GetPos() - enemy->GetPos()).Length();

	//��������������Ă�����U������߂�
	if (distance > kNearDistance)
	{
		m_isCountActionTime = true;
	}
}

void EnemyInput::EnergyAttack()
{

	m_pInputData->BashButton("Y");

	if (m_pEnemyState->GetKind() == CharacterStateBase::CharacterStateKind::kNormalAttack)
	{
		auto attackState = std::dynamic_pointer_cast<CharacterStateNormalAttack>(m_pEnemyState);

		if (attackState->GetNowAttackName() == "Energy6" || attackState->GetEndAttack())
		{
			m_isCountActionTime = true;
		}
	}
}

void EnemyInput::Guard()
{
	m_pInputData->PushButton("B");
}
