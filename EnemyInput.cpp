#include "EnemyInput.h"
#include "GameManager.h"

namespace
{
	//�������Ɣ��f���鋗��
	constexpr float kMiddleDistance = 150.0f;

	//�ߋ����Ɣ��f���鋗��
	constexpr float kNearDistance = 75.0f;

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
	m_moveTime(0)
{
	m_pInputData = inputData;
}

EnemyInput::~EnemyInput()
{

}

void EnemyInput::Update()
{
	auto player = m_pManager->GetOnePlayerPointer();
	auto enemy = m_pManager->GetTwoPlayerPointer();

	m_moveTime--;

	//��莞�Ԃ��ƂɈړ�������ݒ肷��
	if (m_moveTime < 0)
	{
		m_moveTime = kMoveUpdateTime;

		//�v���C���[�Ƃ̋����ňړ������𔻒f����
		float toPlayerLength = (player->GetPos() - enemy->GetPos()).Length();

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
	//�����̏�Ԃ����čs������
	auto enemyState = enemy->GetStateKind();





	//�ړ�����
	(this->*m_moveFunc)();
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
}

void EnemyInput::PhysicalAttack()
{
	m_pInputData->BashButton("X");
}

void EnemyInput::EnergyAttack()
{
	m_pInputData->BashButton("Y");
}

void EnemyInput::Guard()
{
	m_pInputData->PushButton("B");
}
