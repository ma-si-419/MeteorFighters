#include "EnemyInput.h"
#include "GameManager.h"

namespace
{
	//�����Ɣ��f���鋗��
	constexpr float kFarDistance = 200.0f;

	//�������Ɣ��f���鋗��
	constexpr float kMiddleDistance = 100.0f;

	//�ߋ����Ɣ��f���鋗��
	constexpr float kNearDistance = 50.0f;
}

EnemyInput::EnemyInput(std::shared_ptr<MyEngine::InputData> inputData)
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

	//�v���C���[�Ƃ̋����ōs���s����ω�����
	float toPlayerLength = (player->GetPos() - enemy->GetPos()).Length();

	//�߂��Ɣ��f������
	if (toPlayerLength < kNearDistance)
	{

	}
	//���������Ɣ��f������
	else if (toPlayerLength < kMiddleDistance)
	{

	}
	//���������Ɣ��f������
	else if (toPlayerLength > kFarDistance)
	{

	}

}

void EnemyInput::MoveFront()
{
	m_pInputData->TiltStick(MyEngine::Vector2(0, 1000), true);
}

void EnemyInput::MoveRight()
{
	m_pInputData->TiltStick(MyEngine::Vector2(1000, 0), true);
}

void EnemyInput::MoveLeft()
{
	m_pInputData->TiltStick(MyEngine::Vector2(-1000, 0), true);
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
