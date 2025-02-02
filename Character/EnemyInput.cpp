#include "EnemyInput.h"
#include "GameManagerBase.h"
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

	//�ő剽�t���[���U�����s����
	constexpr int kMaxAttackTime = 180;

	//�������Ɣ��f���鋗��
	constexpr float kMiddleDistance = 150.0f;

	//�ߋ����Ɣ��f���鋗��
	constexpr float kNearDistance = 80.0f;

	//�ړ��������X�V���鎞��
	constexpr int kMoveUpdateTime = 120;

	//�ړ������̃����_���̐�
	constexpr int kMoveDirRandomNum = 3;

	//�ő剽�t���[���K�[�h���s����
	constexpr int kMaxGuardTime = 180;

	//�Œች�t���[���K�[�h���s����
	constexpr int kMinGuardTime = 60;

	//�X�[�p�[�_�b�V���̃A�N�V�����ɂ͂����ĉ��t���[���ڂŃX�[�p�[�_�b�V�����s����
	constexpr int kSuperDashActionTime = 45;

	//�ő剽�t���[���X�[�p�[�_�b�V�����s����
	constexpr int kMaxSuperDashTime = 120;

	//�Œች�t���[���X�[�p�[�_�b�V�����s����
	constexpr int kMinSuperDashTime = 60;

	//�A�C�h����Ԃɂ��鎞��
	constexpr int kNoneTime = 120;

	//��Փx�ɂ���ēǂݎ��ꏊ��ς���
	constexpr int kLevelShiftIndex = 4;

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

	m_loadAiData = load.LoadFile("data/csv/enemyAiData.csv");

	//AI�̓�Փx��Ⴂ�ɐݒ肵�Ă���
	m_aiLevel = AiLevel::kEasy;

	//AI�̃f�[�^��ǂݍ���
	for (auto data : m_loadAiData)
	{
		AiData aiData;
		aiData.nearRate = std::stoi(data[static_cast<int>(DataIndex::kNearRate)]);
		aiData.middleRate = std::stoi(data[static_cast<int>(DataIndex::kMiddleRate)]);
		aiData.farRate = std::stoi(data[static_cast<int>(DataIndex::kFarRate)]);
		aiData.minMp = std::stoi(data[static_cast<int>(DataIndex::kMinMp)]);
		m_aiData.push_back(aiData);
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

			Action action = Action::kPhysicalAttack;

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
					//�`���[�g���A���̎��͍s����ύX���Ȃ�
					if (m_pManager->GetGameKind() == GameManagerBase::GameKind::kTutorial)
					{
						action = m_tutorialAction;
					}

					m_stateTime = 0;

			//		action = Action::kSpecialAttack;

					//�s����ύX����
					SetAction(action);

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

	if (m_pManager->GetGameKind() != GameManagerBase::GameKind::kTutorial)
	{
		//�ړ�����
		(this->*m_moveFunc)();
	}

	//�A�N�V��������
	(this->*m_actionFunc)();

}

void EnemyInput::SetAiLevel(AiLevel level)
{
	//��Փx�ɂ���ēǂݎ��ꏊ��ς���
	int shiftIndex = static_cast<int>(level) * kLevelShiftIndex;

	//AI�f�[�^��ǂݍ���
	for (auto item : m_loadAiData)
	{
		AiData pushData;

		//�ߋ����̊m��
		pushData.nearRate = stoi(item[static_cast<int>(DataIndex::kNearRate) + shiftIndex]);

		//�������̊m��
		pushData.middleRate = stoi(item[static_cast<int>(DataIndex::kMiddleRate) + shiftIndex]);

		//�������̊m��
		pushData.farRate = stoi(item[static_cast<int>(DataIndex::kFarRate) + shiftIndex]);

		//�Œ�C�͗�
		pushData.minMp = stoi(item[static_cast<int>(DataIndex::kMinMp) + shiftIndex]);

		//AI�f�[�^��ۑ�
		m_aiData.push_back(pushData);
	}
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
#ifdef _DEBUG

	printfDx("�_�b�V��\n");

#endif // _DEBUG


}

void EnemyInput::SuperDash()
{
	//�X�[�p�[�_�b�V���̃A�N�V�����ɓ����ĉ��t���[���ڂ��v��
	m_stateTime++;

	//�܂��X�[�p�[�_�b�V���̃A�N�V�����ɓ����Ă��Ȃ��Ȃ�
	if (m_pEnemyState->GetKind() != CharacterStateBase::CharacterStateKind::kRush)
	{
		m_pInputData->PushTrigger(true);

		if (m_stateTime > kSuperDashActionTime)
		{
			m_pInputData->PushButton("A");
		}
	}

	//��莞�Ԉȏ�X�[�p�[�_�b�V�����s���Ă�����
	if (m_stateTime > m_superDashTime)
	{
		m_isCountActionTime = true;
	}

#ifdef _DEBUG

	printfDx("�X�[�p�[�_�b�V��\n");

#endif // _DEBUG


}

void EnemyInput::RocketDash()
{
	//���P�b�g�_�b�V�����s��
	m_pInputData->PushTrigger(true);
	m_pInputData->BashButton("A");

	//�v���C���[�Ƃ̋���
	auto player = m_pManager->GetOnePlayerPointer();
	auto enemy = m_pManager->GetTwoPlayerPointer();

	float distance = (player->GetPos() - enemy->GetPos()).Length();

	//�G�̋߂��ɋ߂Â�����
	if (distance < kNearDistance)
	{
		m_isCountActionTime = true;
	}

#ifdef _DEBUG

	printfDx("���P�b�g�_�b�V��\n");

#endif // _DEBUG

}

void EnemyInput::SpecialAttack()
{

	m_pInputData->PushTrigger(true);

	if (m_pEnemyState->GetKind() == CharacterStateBase::CharacterStateKind::kCharge)
	{
		m_pInputData->PushButton("Y");

		m_isCountActionTime = true;
	}

#ifdef _DEBUG

	printfDx("�K�E�Z\n");

#endif // _DEBUG
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
	}

#ifdef _DEBUG

	printfDx("�C�̓`���[�W\n");

#endif // _DEBUG

}

void EnemyInput::PhysicalAttack()
{
	m_stateTime++;

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

	//�ő厞�Ԃ��������U�����s���Ă�����
	if (m_stateTime > kMaxAttackTime)
	{
		m_isCountActionTime = true;
	}

#ifdef _DEBUG

	printfDx("�i���U��\n");

#endif // _DEBUG

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

#ifdef _DEBUG

	printfDx("�C�e�U��\n");

#endif // _DEBUG
}

void EnemyInput::Guard()
{
	m_stateTime++;

	m_pInputData->PushButton("B");

	//�K�[�h����߂�
	if (m_stateTime > m_guardTime)
	{
		m_isCountActionTime = true;
	}
#ifdef _DEBUG

	printfDx("�K�[�h\n");

#endif // _DEBUG
}

void EnemyInput::UpChargeAttack()
{
	//���Ԍv��
	m_stateTime++;

	m_pInputData->TiltStick(MyEngine::Vector2(0, -1000), true);

	m_pInputData->PushButton("X");

	//�A�b�p�[�U�����s���Ă�����
	if (m_pEnemyState->GetKind() == CharacterStateBase::CharacterStateKind::kNormalAttack)
	{
		auto attackState = std::dynamic_pointer_cast<CharacterStateNormalAttack>(m_pEnemyState);
		if (attackState->GetNowAttackName() == "UpCharge" || attackState->GetEndAttack())
		{
			//m_isCountActionTime = true;
		}
	}

	//��莞�Ԉȏ�U�����s���Ă�����
	if (m_stateTime > kMaxAttackTime)
	{
		m_isCountActionTime = true;
	}

#ifdef _DEBUG

	printfDx("�ソ�ߍU��\n");

#endif // _DEBUG
}

void EnemyInput::MiddleChargeAttack()
{
	m_stateTime++;

	m_pInputData->TiltStick(MyEngine::Vector2(0, 0), true);
	m_pInputData->PushButton("X");
	//�~�h���U�����s���Ă�����
	if (m_pEnemyState->GetKind() == CharacterStateBase::CharacterStateKind::kNormalAttack)
	{
		auto attackState = std::dynamic_pointer_cast<CharacterStateNormalAttack>(m_pEnemyState);
		if (attackState->GetNowAttackName() == "MiddleCharge" || attackState->GetEndAttack())
		{
			m_isCountActionTime = true;
		}
	}

	//��莞�Ԉȏ�U�����s���Ă�����
	if (m_stateTime > kMaxAttackTime)
	{
		m_isCountActionTime = true;
	}

#ifdef _DEBUG

	printfDx("�����ߍU��\n");

#endif // _DEBUG
}

void EnemyInput::DownChargeAttack()
{
	m_stateTime++;

	m_pInputData->TiltStick(MyEngine::Vector2(0, 1000), true);
	m_pInputData->PushButton("X");
	//�_�E���U�����s���Ă�����
	if (m_pEnemyState->GetKind() == CharacterStateBase::CharacterStateKind::kNormalAttack)
	{
		auto attackState = std::dynamic_pointer_cast<CharacterStateNormalAttack>(m_pEnemyState);
		if (attackState->GetNowAttackName() == "DownCharge" || attackState->GetEndAttack())
		{
			m_isCountActionTime = true;
		}
	}

	//��莞�Ԉȏ�U�����s���Ă�����
	if (m_stateTime > kMaxAttackTime)
	{
		m_isCountActionTime = true;
	}

#ifdef _DEBUG

	printfDx("�����ߍU��\n");

#endif // _DEBUG
}

void EnemyInput::None()
{
	m_isCountActionTime = true;

#ifdef _DEBUG
	printfDx("�������Ȃ�\n");
#endif // _DEBUG
}


void EnemyInput::SetAction(Action action)
{
	switch (action)
	{
		//�i���U��
	case EnemyInput::Action::kPhysicalAttack:
		m_actionFunc = &EnemyInput::PhysicalAttack;
		break;

		//�G�l���M�[�U��
	case EnemyInput::Action::kEnergyAttack:
		m_actionFunc = &EnemyInput::EnergyAttack;
		break;

		//�_�b�V��
	case EnemyInput::Action::kDash:
		m_actionFunc = &EnemyInput::Dash;
		break;

		//���b�V��
	case EnemyInput::Action::kSuperDash:
		//�X�[�p�[�_�b�V���̎��Ԑݒ�
		m_superDashTime = GetRand(kMaxSuperDashTime - kMinSuperDashTime) + kMinSuperDashTime;

		//�X�[�p�[�_�b�V�����n�߂�܂ł̎��Ԃ����Z
		m_superDashTime += kSuperDashActionTime;

		m_actionFunc = &EnemyInput::SuperDash;
		break;

		//�K�E�Z
	case EnemyInput::Action::kSpecialAttack:
		m_actionFunc = &EnemyInput::SpecialAttack;
		break;

		//�G�l���M�[�`���[�W
	case EnemyInput::Action::kEnergyCharge:
		m_actionFunc = &EnemyInput::EnergyCharge;
		break;

		//�K�[�h
	case EnemyInput::Action::kGuard:
		m_actionFunc = &EnemyInput::Guard;
		//�K�[�h���s�����Ԃ�ݒ�
		m_guardTime = GetRand(kMaxGuardTime - kMinGuardTime) + kMinGuardTime;
		break;

		//�������Ȃ�
	case EnemyInput::Action::kNone:
		m_actionFunc = &EnemyInput::None;
		break;

		//���i�`���[�W�U��
	case EnemyInput::Action::kMiddleChargeAttack:
		m_actionFunc = &EnemyInput::MiddleChargeAttack;
		break;

		//���`���[�W�U��
	case EnemyInput::Action::kDownChargeAttack:
		m_actionFunc = &EnemyInput::DownChargeAttack;
		break;

		//��`���[�W�U��
	case EnemyInput::Action::kUpChargeAttack:
		m_actionFunc = &EnemyInput::UpChargeAttack;
		break;
		//���P�b�g�_�b�V��
	case EnemyInput::Action::kRocketDash:
		m_actionFunc = &EnemyInput::RocketDash;
		break;
		//����ȊO
	default:
		m_actionFunc = &EnemyInput::None;
		break;
	}
}