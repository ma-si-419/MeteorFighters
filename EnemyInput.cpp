#include "EnemyInput.h"
#include "GameManager.h"

namespace
{
	//遠いと判断する距離
	constexpr float kFarDistance = 200.0f;

	//中距離と判断する距離
	constexpr float kMiddleDistance = 100.0f;

	//近距離と判断する距離
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

	//プレイヤーとの距離で行う行動を変化する
	float toPlayerLength = (player->GetPos() - enemy->GetPos()).Length();

	//近いと判断した時
	if (toPlayerLength < kNearDistance)
	{

	}
	//中距離だと判断した時
	else if (toPlayerLength < kMiddleDistance)
	{

	}
	//遠距離だと判断した時
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
