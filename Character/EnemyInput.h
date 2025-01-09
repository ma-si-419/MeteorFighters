#pragma once
#include "InputData.h"
#include <memory>

class GameManagerBase;
class CharacterStateBase;
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

	enum class Action
	{
		PhysicalAttack,
		EnergyAttack,
		Dash,
		Rush,
		SpecialAttack,
		EnergyCharge
	};

	enum class DataIndex
	{
		kActionName,
		kNearRate,
		kMiddleRate,
		kFarRate,
		kMinMp
	};

public:

	EnemyInput(std::shared_ptr<MyEngine::InputData> inputData);
	~EnemyInput();

	void SetGameManager(std::shared_ptr<GameManagerBase> manager) { m_pManager = manager; };

	void SetState(std::shared_ptr<CharacterStateBase> state) { m_pEnemyState = state; }

	void Update();

private:

	void MoveFront();

	void MoveRight();

	void MoveLeft();

	void MoveBack();

	void PhysicalAttack();

	void EnergyAttack();

	void Dash();

	void Rush();

	void SpecialAttack();

	void EnergyCharge();

	void Guard();

	//‰½‚à‚µ‚È‚¢
	void None() {};

private:

	struct AiData
	{
		int nearRate = 0;
		int middleRate = 0;
		int farRate = 0;

		int minMp = 0;
	};

private:

	using MoveFunc = void(EnemyInput::*)();

	using ActionFunc = void(EnemyInput::*)();

	MoveFunc m_moveFunc;

	ActionFunc m_actionFunc;

	int m_actionTime;

	bool m_isCountActionTime;

	int m_moveTime;

	std::shared_ptr<GameManagerBase> m_pManager;

	std::shared_ptr<MyEngine::InputData> m_pInputData;

	int m_stateTime;

	std::shared_ptr<CharacterStateBase> m_pEnemyState;

	std::vector<AiData> m_aiData;

};

