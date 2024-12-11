#pragma once
#include "InputData.h"
#include <memory>

class GameManager;
class EnemyInput
{
public:

	EnemyInput(std::shared_ptr<MyEngine::InputData> inputData);
	~EnemyInput();

	void SetGameManager(std::shared_ptr<GameManager> manager) { m_pManager = manager; };

	void Update();

private:

	void MoveFront();

	void MoveRight();

	void MoveLeft();

	void Dash();

	void PhysicalAttack();

	void EnergyAttack();

	void Guard();

private:

	std::shared_ptr<GameManager> m_pManager;

	std::shared_ptr<MyEngine::InputData> m_pInputData;

};

