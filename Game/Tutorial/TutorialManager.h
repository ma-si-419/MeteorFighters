#pragma once
#include "GameManagerBase.h"

class TutorialUi;
class TutorialManager : public GameManagerBase
{
public:
	enum class TutorialKind
	{
		kMove,
		kStep,
		kDash,
		kSkyMove,
		kPhysicalAttack,
		kChargePhysicalAttack,
		kChaseAttack,
		kEnergyCharge,
		kEnergyAttack,
		kChargeEnergyAttack,
		kSpecialAttack,
		kGuard,
		kFalls,
		kReturn,
		kSuperDash,
		kRocketDash,
		kButtonBashing,
		kUpperAttack,
		kMiddleAttack,
		kDownAttack,
		kGuardUp,
		kGuardDown,
		kJustGuard,
		kTutorialEnd,
		kTutorialNum = kTutorialEnd
	};

	enum class TutorialSuccessKind
	{
		kMove,
		kStep,
		kDash,
		kJump,
		kSkyIdle,
		kUp,
		kDown,
		kPhysicalAttack1,
		kPhysicalAttack2,
		kPhysicalAttack3,
		kPhysicalAttack4,
		kPhysicalAttack5,
		kPhysicalAttack6,
		kPhysicalAttack7,
		kPhysicalAttack8,
		kChargePhysicalAttack,
		kChaseAttack,
		kEnergyCharge,
		kEnergyAttack,
		kChargeEnergyAttack,
		kSpecialAttack,
		kGuard,
		kJustGuard,
		kFalls,
		kReturn,
		kSuperDash,
		kRocketDash,
		kButtonBashing,
		kUpperAttack,
		kDekaKick,
		kCycloneKick,
		kUpGuard,
		kDownGuard
	};

	enum class TutorialPlayDataIndex
	{
		kTutorialName,
		kDescription,
		kPlayingString,
		kEnemySituation,
		kStartDistance,
		kIsGround,
		kButton
	};

	//チュートリアルの進め方
	enum class TutorialMode
	{
		kAuto,
		kRepeat,
		kStop,
		kModeNum
	};


private:

	enum class TutorialSituation
	{
		kStartMenu,
		kSelectMenu,
		kPlayMenu,
		kStart,
		kPlaying,
		kSuccess
	};

public:

	TutorialManager(std::shared_ptr<GameCamera> camera);

	~TutorialManager();

	virtual void Init() override;

	virtual void Update() override;

	virtual void Draw() override;

	virtual void Final() override;

	void SuccessTutorial(TutorialSuccessKind kind) { m_successTutorialKinds[kind] = true; }

private:

	void UpdateStartMenu();

	void UpdatePlayMenu();

	void UpdateStart();

	void UpdatePlaying();

	void UpdateSuccess();

	void UpdateSelectMenu();

	void DrawStartMenu();

	void DrawPlayMenu();

	void DrawStart();
	 
	void DrawPlaying();

	void DrawSuccess();

	void DrawSelectMenu();

	/// <summary>
	/// 状況を変化させる
	/// </summary>
	/// <param name="next">次の状況</param>
	void ChangeSituation(TutorialSituation next);

	/// <summary>
	/// string型のクリア条件をTutorialSuccessKindに変更する
	/// </summary>
	/// <param name="kind">外部データに書いてあるクリア条件</param>
	/// <returns>変更後のクリア条件</returns>
	TutorialSuccessKind ChangeStringToSuccessKind(std::string kind);

	/// <summary>
	/// string型のチュートリアルの種類をTutorialKindに変更する
	/// </summary>
	/// <param name="kind">外部データに書いてあるクリア条件</param>
	/// <returns>変更後のチュートリアルの種類</returns>
	TutorialKind ChangeStringToTutorialKind(std::string kind);

	/// <summary>
	/// エネミーの行動を変更する
	/// </summary>
	/// <param name="action">変更するアクション名</param>
	void ChangeEnemyAction(std::string action);

private:

	//関数ポインタ
	using UpdateSituationFunc = void (TutorialManager::*)();
	using DrawSituationFunc = void (TutorialManager::*)();

	//状況によって変更する更新処理
	UpdateSituationFunc m_updateSituationFunc;

	//状況によって変更する描画処理
	DrawSituationFunc m_drawSituationFunc;

	//現在何のチュートリアルを行っているか
	TutorialKind m_nowTutorial;

	//現在の状況
	TutorialSituation m_tutorialSituation;

	//チュートリアルのUI
	std::shared_ptr<TutorialUi> m_pTutorialUi;

	//チュートリアルのクリア条件で何をクリアしたかを保存する
	std::map<TutorialSuccessKind, bool> m_successTutorialKinds;

	//チュートリアルのデータ
	std::vector<std::vector<std::string>> m_tutorialPlayData;

	//エネミーのインプットデータ
	std::shared_ptr<EnemyInput> m_pEnemyInput;
};