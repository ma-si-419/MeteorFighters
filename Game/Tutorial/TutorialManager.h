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

	//�`���[�g���A����UI
	std::shared_ptr<TutorialUi> m_pTutorialUi;

	//�`���[�g���A���̃N���A�����ŉ����N���A��������ۑ�����
	std::map<TutorialSuccessKind, bool> m_successTutorialKinds;
};