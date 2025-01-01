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
	/// �󋵂�ω�������
	/// </summary>
	/// <param name="next">���̏�</param>
	void ChangeSituation(TutorialSituation next);

private:

	//�֐��|�C���^
	using UpdateSituationFunc = void (TutorialManager::*)();
	using DrawSituationFunc = void (TutorialManager::*)();

	//�󋵂ɂ���ĕύX����X�V����
	UpdateSituationFunc m_updateSituationFunc;

	//�󋵂ɂ���ĕύX����`�揈��
	DrawSituationFunc m_drawSituationFunc;

	//���݉��̃`���[�g���A�����s���Ă��邩
	TutorialKind m_nowTutorial;

	//���݂̏�
	TutorialSituation m_tutorialSituation;

	//�`���[�g���A�����N���A�������ǂ���
	bool m_isSuccessTutorial;

	//�`���[�g���A����UI
	std::shared_ptr<TutorialUi> m_pTutorialUi;
};

