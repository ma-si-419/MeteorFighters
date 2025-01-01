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
		kPlaying
	};

public:

	TutorialManager(std::shared_ptr<GameCamera> camera);

	~TutorialManager();

	void Init() override;

	void Update() override;

	void Draw() override;

	void Final() override;

	void SuccessTutorial() { m_isSuccessTutorial = true; }


private:

	void UpdateMenu();

	void UpdateStart();

	void UpdatePlaying();

	void DrawMenu();

	void DrawStart();
	 
	void DrawPlaying();

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

	//チュートリアルをクリアしたかどうか
	bool m_isSuccessTutorial;

	//チュートリアルのUI
	std::shared_ptr<TutorialUi> m_pTutorialUi;
};

