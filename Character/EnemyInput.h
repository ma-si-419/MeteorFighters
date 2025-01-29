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

	//�`���[�g���A���A�N�V����
	Action m_tutorialAction;

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

	//�X�e�[�g
	std::shared_ptr<CharacterStateBase> m_pEnemyState;

	//AI�f�[�^
	std::vector<AiData> m_aiData;

};

