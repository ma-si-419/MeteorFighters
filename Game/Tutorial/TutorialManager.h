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

	//�`���[�g���A���̐i�ߕ�
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
	/// �󋵂�ω�������
	/// </summary>
	/// <param name="next">���̏�</param>
	void ChangeSituation(TutorialSituation next);

	/// <summary>
	/// string�^�̃N���A������TutorialSuccessKind�ɕύX����
	/// </summary>
	/// <param name="kind">�O���f�[�^�ɏ����Ă���N���A����</param>
	/// <returns>�ύX��̃N���A����</returns>
	TutorialSuccessKind ChangeStringToSuccessKind(std::string kind);

	/// <summary>
	/// string�^�̃`���[�g���A���̎�ނ�TutorialKind�ɕύX����
	/// </summary>
	/// <param name="kind">�O���f�[�^�ɏ����Ă���N���A����</param>
	/// <returns>�ύX��̃`���[�g���A���̎��</returns>
	TutorialKind ChangeStringToTutorialKind(std::string kind);

	/// <summary>
	/// �G�l�~�[�̍s����ύX����
	/// </summary>
	/// <param name="action">�ύX����A�N�V������</param>
	void ChangeEnemyAction(std::string action);

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

	//�`���[�g���A���̃f�[�^
	std::vector<std::vector<std::string>> m_tutorialPlayData;

	//�G�l�~�[�̃C���v�b�g�f�[�^
	std::shared_ptr<EnemyInput> m_pEnemyInput;
};