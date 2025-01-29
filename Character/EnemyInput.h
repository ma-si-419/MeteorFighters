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
		kPhysicalAttack,
		kEnergyAttack,
		kDash,
		kSuperDash,
		kRocketDash,
		kSpecialAttack,
		kEnergyCharge,
		kGuard,
		kMiddleChargeAttack,
		kDownChargeAttack,
		kUpChargeAttack,
		kNone
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

	void SetTutorialAction(Action action) { m_tutorialAction = action; }

	void Update();

private:

	void MoveFront();

	void MoveRight();

	void MoveLeft();

	void MoveBack();

	void PhysicalAttack();

	void EnergyAttack();

	void Dash();

	void SuperDash();

	void RocketDash();

	void SpecialAttack();

	void EnergyCharge();

	void Guard();

	void UpChargeAttack();

	void MiddleChargeAttack();

	void DownChargeAttack();

	/// <summary>
	/// アクションを変更する
	/// </summary>
	/// <param name="action">次に行うアクション</param>
	void SetAction(Action action);

	//何もしない
	void None();

private:

	struct AiData
	{
		int nearRate = 0;
		int middleRate = 0;
		int farRate = 0;

		int minMp = 0;
	};

private:

	//移動関数
	using MoveFunc = void(EnemyInput::*)();

	//アクション関数
	using ActionFunc = void(EnemyInput::*)();

private:

	//移動関数
	MoveFunc m_moveFunc;

	//アクション関数
	ActionFunc m_actionFunc;

	//チュートリアルアクション
	Action m_tutorialAction;

	//行動時間
	int m_actionTime;

	//次の行動までの時間を計測するかどうか
	bool m_isCountActionTime;

	//移動時間
	int m_moveTime;

	//ガードを行う時間
	int m_guardTime;

	//スーパーダッシュを行う時間
	int m_superDashTime;

	//ゲームマネージャ
	std::shared_ptr<GameManagerBase> m_pManager;

	//入力データ
	std::shared_ptr<MyEngine::InputData> m_pInputData;

	//ステートにいる時間
	int m_stateTime;

	//ステート
	std::shared_ptr<CharacterStateBase> m_pEnemyState;

	//AIデータ
	std::vector<AiData> m_aiData;

};

