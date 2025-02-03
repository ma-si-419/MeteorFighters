#pragma once
#include "InputData.h"
#include <memory>

class GameManagerBase;
class CharacterStateBase;
class EnemyInput
{
public:

	//移動方向
	enum class MoveDir
	{
		kFront,
		kBack,
		kRight,
		kLeft
	};

	//ランダムで行動を選択する時のアクション
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

	//状況によって行動を変更する時のアクション
	enum class SituationAction
	{
		kFalls,
		kReturn,
		kGuard,
		kJustGuard,
		kNone,
		kSituationActionNum = kNone
	};

	enum class DataIndex
	{
		kActionName,
		kNearRate,
		kMiddleRate,
		kFarRate,
		kMinMp
	};

	enum class AiLevel
	{
		kEasy,
		kNormal,
		kHard,
		kLevelNum
	};

public:

	EnemyInput(std::shared_ptr<MyEngine::InputData> inputData);
	~EnemyInput();

	/// <summary>
	/// ゲームマネージャーをセットする
	/// </summary>
	/// <param name="manager">マネージャーのポインタ</param>
	void SetGameManager(std::shared_ptr<GameManagerBase> manager) { m_pManager = manager; };

	/// <summary>
	/// プレイヤーのステートをセットする
	/// </summary>
	/// <param name="state">プレイヤーのステート</param>
	void SetPlayerState(std::shared_ptr<CharacterStateBase> state) { m_pPlayerState = state; }

	/// <summary>
	/// エネミーのステートをセットする
	/// </summary>
	/// <param name="state">エネミーのステート</param>
	void SetEnemyState(std::shared_ptr<CharacterStateBase> state) { m_pEnemyState = state; }

	/// <summary>
	/// チュートリアルで行うアクションをセットする
	/// </summary>
	/// <param name="action">アクションの種類</param>
	void SetTutorialAction(Action action) { m_tutorialAction = action; }

	void Update();

	/// <summary>
	/// 難易度を設定する
	/// </summary>
	/// <param name="level">難易度</param>
	void SetAiLevel(AiLevel level);

	/// <summary>
	/// 現在の状況みて行う行動を取得する
	/// </summary>
	/// <returns>何も行う行動がなかったらkNoneを返す</returns>
	SituationAction GetSituationAction();

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

	void JustGuard();

	void Return();

	void Falls();

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

	//状況に応じたアクション
	SituationAction m_situationAction;

	//チュートリアルアクション
	Action m_tutorialAction;

	//外部から読み込んだAIデータ
	std::vector<std::vector<std::string>> m_loadActionAiData;
	
	//状況に応じたアクションの確率
	int m_situationActionRate[static_cast<int>(SituationAction::kSituationActionNum)][static_cast<int>(AiLevel::kLevelNum)];

	//状況に応じたアクションを行うまでの時間
	int m_situationActionTime;

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

	//前のフレームの自身のステートの種類
	int m_lastStateKind;

	//エネミーのステート
	std::shared_ptr<CharacterStateBase> m_pEnemyState;

	//プレイヤーのステート
	std::shared_ptr<CharacterStateBase> m_pPlayerState;

	//AIデータ
	std::vector<AiData> m_aiData;

	//AIレベル
	AiLevel m_aiLevel;
};