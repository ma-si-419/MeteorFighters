#pragma once
#include "GameManagerBase.h"
#include <memory>
#include <vector>
#include <string>
#include "Vector3.h"
#include "Character.h"
#include "Game.h"

class Player;
class Enemy;
class GameCamera;
class Attack;
class Stage;
class EffectManager;
class Effect;
class GameUi;
class BattleManager : public GameManagerBase
{
public:

	BattleManager(std::shared_ptr<GameCamera> camera);
	~BattleManager();

	/// <summary>
	/// �Q�[���}�l�[�W���[�̏�����
	/// </summary>
	void Init() override;

	/// <summary>
	/// �Q�[���}�l�[�W���[�̍X�V
	/// </summary>
	void Update() override;

	/// <summary>
	/// �U���̕`��Ȃǂ��s��
	/// </summary>
	void Draw() override;

	/// <summary>
	/// �V�[�����甲����Ƃ��ȂǂɎg�p
	/// </summary>
	void Final();

private:

	/// <summary>
	/// �J�n���̉��o���̃A�b�v�f�[�g
	/// </summary>
	void UpdateStart();

	/// <summary>
	/// �o�g�����A�b�v�f�[�g
	/// </summary>
	void UpdateBattle();

	/// <summary>
	/// �m�b�N�A�E�g���̃A�b�v�f�[�g
	/// </summary>
	void UpdateKnockOut();

	/// <summary>
	/// ���U���g���̃A�b�v�f�[�g
	/// </summary>
	void UpdateResult();

	/// <summary>
	/// ���g���C���̃A�b�v�f�[�g
	/// </summary>
	void UpdateRetry();

	/// <summary>
	/// ���j���[���J���Ă��鎞�̃A�b�v�f�[�g
	/// </summary>
	void UpdateMenu();

	/// <summary>
	/// �󋵂�ω�������
	/// </summary>
	/// <param name="situation">�ύX��̏�</param>
	void ChangeSituation(BattleSituation situation);

private:

	using UpdateSituationFunc = void (BattleManager::*)();

	UpdateSituationFunc m_updateSituationFunc;

};
