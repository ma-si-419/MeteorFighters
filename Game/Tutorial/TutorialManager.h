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
		kJump,
		kSkyMove,
		kPhysicalAttack,
		kChargePhysicalAttack,
		kEnergyCharge,
		kEnergyAttack,
		kChargeEnergyAttack,
		kGuard,
		kSpecialAttack,
		kTutorialNum
	};

	enum class TutorialSuccessKind
	{
		kMove,
		kStep,
		kJump,
		kUp,
		kDown,
		kPhysicalAttack,
		kChargePhysicalAttack,
		kEnergyAttack,
		kChargeEnergyAttack,
		kEnergyCharge,
		kGuard,
		kSpecialAttack
	};

	enum class TutorialDataIndex
	{
		kTutorialName,
		kStartString,
		kPlayingString,
		kButton
	};

private:

	enum class TutorialSituation
	{
		kMenu,
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

	void UpdateMenu();

	void UpdateStart();

	void UpdatePlaying();

	void UpdateSuccess();

	void DrawMenu();

	void DrawStart();
	 
	void DrawPlaying();

	void DrawSuccess();

	/// <summary>
	/// 状況を変化させる
	/// </summary>
	/// <param name="next">次の状況</param>
	void ChangeSituation(TutorialSituation next);

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
};