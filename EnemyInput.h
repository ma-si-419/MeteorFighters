#pragma once
#include "InputData.h"
#include <memory>

class GameManager;
class EnemyInput
{
public:
	enum class MoveDir
	{
		kFront,
		kBack,
		kRight,
		kLeft
	};

public:

	EnemyInput(std::shared_ptr<MyEngine::InputData> inputData);
	~EnemyInput();

	void SetGameManager(std::shared_ptr<GameManager> manager) { m_pManager = manager; };

	void Update();

private:

	void MoveFront();

	void MoveRight();

	void MoveLeft();

	void MoveBack();

	void Dash();

	void PhysicalAttack();

	void EnergyAttack();

	void Guard();

private:

	using MoveFunc = void(EnemyInput::*)();

	MoveFunc m_moveFunc;

	int m_moveTime;

	std::shared_ptr<GameManager> m_pManager;

	std::shared_ptr<MyEngine::InputData> m_pInputData;



};

