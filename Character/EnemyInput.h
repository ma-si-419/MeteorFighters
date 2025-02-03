#pragma once
#include "InputData.h"
#include <memory>

class GameManagerBase;
class CharacterStateBase;
class EnemyInput
{
public:

	//�ړ�����
	enum class MoveDir
	{
		kFront,
		kBack,
		kRight,
		kLeft
	};

	//�����_���ōs����I�����鎞�̃A�N�V����
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

	//�󋵂ɂ���čs����ύX���鎞�̃A�N�V����
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
	/// �Q�[���}�l�[�W���[���Z�b�g����
	/// </summary>
	/// <param name="manager">�}�l�[�W���[�̃|�C���^</param>
	void SetGameManager(std::shared_ptr<GameManagerBase> manager) { m_pManager = manager; };

	/// <summary>
	/// �v���C���[�̃X�e�[�g���Z�b�g����
	/// </summary>
	/// <param name="state">�v���C���[�̃X�e�[�g</param>
	void SetPlayerState(std::shared_ptr<CharacterStateBase> state) { m_pPlayerState = state; }

	/// <summary>
	/// �G�l�~�[�̃X�e�[�g���Z�b�g����
	/// </summary>
	/// <param name="state">�G�l�~�[�̃X�e�[�g</param>
	void SetEnemyState(std::shared_ptr<CharacterStateBase> state) { m_pEnemyState = state; }

	/// <summary>
	/// �`���[�g���A���ōs���A�N�V�������Z�b�g����
	/// </summary>
	/// <param name="action">�A�N�V�����̎��</param>
	void SetTutorialAction(Action action) { m_tutorialAction = action; }

	void Update();

	/// <summary>
	/// ��Փx��ݒ肷��
	/// </summary>
	/// <param name="level">��Փx</param>
	void SetAiLevel(AiLevel level);

	/// <summary>
	/// ���݂̏󋵂݂čs���s�����擾����
	/// </summary>
	/// <returns>�����s���s�����Ȃ�������kNone��Ԃ�</returns>
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
	/// �A�N�V������ύX����
	/// </summary>
	/// <param name="action">���ɍs���A�N�V����</param>
	void SetAction(Action action);

	//�������Ȃ�
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

	//�ړ��֐�
	using MoveFunc = void(EnemyInput::*)();

	//�A�N�V�����֐�
	using ActionFunc = void(EnemyInput::*)();

private:

	//�ړ��֐�
	MoveFunc m_moveFunc;

	//�A�N�V�����֐�
	ActionFunc m_actionFunc;

	//�󋵂ɉ������A�N�V����
	SituationAction m_situationAction;

	//�`���[�g���A���A�N�V����
	Action m_tutorialAction;

	//�O������ǂݍ���AI�f�[�^
	std::vector<std::vector<std::string>> m_loadActionAiData;
	
	//�󋵂ɉ������A�N�V�����̊m��
	int m_situationActionRate[static_cast<int>(SituationAction::kSituationActionNum)][static_cast<int>(AiLevel::kLevelNum)];

	//�󋵂ɉ������A�N�V�������s���܂ł̎���
	int m_situationActionTime;

	//�s������
	int m_actionTime;

	//���̍s���܂ł̎��Ԃ��v�����邩�ǂ���
	bool m_isCountActionTime;

	//�ړ�����
	int m_moveTime;

	//�K�[�h���s������
	int m_guardTime;

	//�X�[�p�[�_�b�V�����s������
	int m_superDashTime;

	//�Q�[���}�l�[�W��
	std::shared_ptr<GameManagerBase> m_pManager;

	//���̓f�[�^
	std::shared_ptr<MyEngine::InputData> m_pInputData;

	//�X�e�[�g�ɂ��鎞��
	int m_stateTime;

	//�O�̃t���[���̎��g�̃X�e�[�g�̎��
	int m_lastStateKind;

	//�G�l�~�[�̃X�e�[�g
	std::shared_ptr<CharacterStateBase> m_pEnemyState;

	//�v���C���[�̃X�e�[�g
	std::shared_ptr<CharacterStateBase> m_pPlayerState;

	//AI�f�[�^
	std::vector<AiData> m_aiData;

	//AI���x��
	AiLevel m_aiLevel;
};