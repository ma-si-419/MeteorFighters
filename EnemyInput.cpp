#include "EnemyInput.h"
#include "GameManager.h"

namespace
{
	//‰“‚¢‚Æ”»’f‚·‚é‹——£
	constexpr float kFarDistance = 200.0f;

	//’†‹——£‚Æ”»’f‚·‚é‹——£
	constexpr float kMiddleDistance = 100.0f;

	//‹ß‹——£‚Æ”»’f‚·‚é‹——£
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

	//ƒvƒŒƒCƒ„[‚Æ‚Ì‹——£‚Ås‚¤s“®‚ð•Ï‰»‚·‚é
	float toPlayerLength = (player->GetPos() - enemy->GetPos()).Length();

	//‹ß‚¢‚Æ”»’f‚µ‚½Žž
	if (toPlayerLength < kNearDistance)
	{

	}
	//’†‹——£‚¾‚Æ”»’f‚µ‚½Žž
	else if (toPlayerLength < kMiddleDistance)
	{

	}
	//‰“‹——£‚¾‚Æ”»’f‚µ‚½Žž
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
