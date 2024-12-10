#pragma once
#include "InputData.h"
#include <memory>

class GameManager;
class EnemyInput
{
public:

	EnemyInput(std::shared_ptr<MyEngine::InputData> inputData);
	~EnemyInput();


private:

	std::shared_ptr<GameManager> m_pManager;

	std::shared_ptr<MyEngine::InputData> m_pInputData;

};

