#pragma once
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
class BattleManager : public std::enable_shared_from_this<BattleManager>
{
public:

	enum class Situation
	{
		kRetry,
		kStart1P,
		kStart2P,
		kBattle,
		kKnockOut,
		kResult
	};

public:

	BattleManager(std::shared_ptr<GameCamera> camera);
	~BattleManager();

	/// <summary>
	/// �Q�[���}�l�[�W���[�̏�����
	/// </summary>
	void Init();

	/// <summary>
	/// �Q�[����������x�s���ۂ̏��������s��
	/// </summary>
	void RetryInit();

	/// <summary>
	/// �Q�[���}�l�[�W���[�̍X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �U���̕`��Ȃǂ��s��
	/// </summary>
	void Draw();

	/// <summary>
	/// �V�[�����甲����Ƃ��ȂǂɎg�p
	/// </summary>
	void Final();

	/// <summary>
	/// �v���C���[�����삷��L�����N�^�[��ݒ肷��
	/// </summary>
	/// <param name="number">�v���C���[���g���L�����N�^�[�̔ԍ�</param>
	void SetOnePlayerStatus(int number, std::vector<std::string> statusData);

	/// <summary>
	/// �G�l�~�[�����삷��L�����N�^�[��ݒ肷��
	/// </summary>
	/// <param name="number">�G�l�~�[���g���L�����N�^�[�̔ԍ�</param>
	void SetTwoPlayerStatus(int number, std::vector<std::string> statusData);

	/// <summary>
	/// 1P�̃|�C���^��Ԃ�
	/// </summary>
	/// <returns>1P�̃|�C���^</returns>
	std::shared_ptr<Character> GetOnePlayerPointer() { return m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]; }

	/// <summary>
	/// 2P�̃|�C���^��Ԃ�
	/// </summary>
	/// <returns>2P�̃|�C���^</returns>
	std::shared_ptr<Character> GetTwoPlayerPointer() { return m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]; }

	/// <summary>
	/// �G�t�F�N�g�}�l�[�W���[�̃|�C���^��Ԃ�
	/// </summary>
	/// <returns>�G�t�F�N�g�Ǘ��N���X�̃|�C���^</returns>
	std::shared_ptr<EffectManager> GetEffectManagerPointer() { return m_pEffectManager; }

	/// <summary>
	/// �ΐ푊��̍��W���擾����
	/// </summary>
	/// <param name="character">���̊֐����Ă񂾃L�����N�^�[�̃|�C���^</param>
	/// <returns>�ΐ푊��̍��W</returns>
	MyEngine::Vector3 GetTargetPos(std::shared_ptr<Character> character);

	/// <summary>
	/// �ΐ푊��̈ړ��x�N�g�����擾����
	/// </summary>
	/// <param name="character">���̊֐����Ă񂾃L�����N�^�[�̃|�C���^</param>
	/// <returns>�ΐ푊��̍��W</returns>
	MyEngine::Vector3 GetTargetVelo(std::shared_ptr<Character> character);

	/// <summary>
	/// �ΐ푊��̍��̂����Ԃ��擾����
	/// </summary>
	/// <param name="character">���̊֐����Ă񂾃L�����N�^�[�̃|�C���^</param>
	/// <returns>�ΐ푊��̂�����</returns>
	Character::HitReactionKind GetTargetHitReaction(std::shared_ptr<Character> character);

	/// <summary>
	/// �ΐ푊��̔w����W���擾����
	/// </summary>
	/// <param name="distance">�w��̋���</param>
	/// <param name="character">���g�̃|�C���^</param>
	/// <returns>�ΐ푊��̔w����W</returns>
	MyEngine::Vector3 GetTargetBackPos(float distance, std::shared_ptr<Character> character);

	/// <summary>
	/// �Ǘ�����U����ǉ�����
	/// </summary>
	/// <param name="attack">�ǉ�����U���N���X�̃|�C���^</param>
	void AddAttack(std::shared_ptr<Attack> attack);

	/// <summary>
	/// �J�����������ړ�������
	/// </summary>
	void StartFastCameraMove();

	/// <summary>
	/// �J�����̕␳���~�߂�
	/// </summary>
	void StopCameraCorrection();

	/// <summary>
	/// �J�����̓������~�߂�
	/// </summary>
	/// <param name="time">�~�߂鎞��</param>
	void StopCamera(int time);

	/// <summary>
	/// �J�����̕␳���n�߂�
	/// </summary>
	void StartCameraCorrection();

	/// <summary>
	/// �J������h���ݒ肷��
	/// </summary>
	/// <param name="time">�h�炷����</param>
	void ShakeCamera(int time);

	/// <summary>
	/// �J�������ɂ₩�ɗh�炷
	/// </summary>
	void SwayCamera();

	/// <summary>
	/// �G�t�F�N�g��o�^����
	/// </summary>
	/// <param name="effect">�o�^�������G�t�F�N�g�̃|�C���^</param>
	void EntryEffect(std::shared_ptr<Effect> effect);

	/// <summary>
	/// �G�t�F�N�g���폜����
	/// </summary>
	/// <param name="effect">�폜�������G�t�F�N�g</param>
	void ExitEffect(std::shared_ptr<Effect> effect);

	/// <summary>
	/// ���̏󋵂��擾����
	/// </summary>
	/// <returns>���݃Q�[���V�[���ŉ����s���Ă��邩��Ԃ�</returns>
	Situation GetNowSituation() { return m_situation; }

	/// <summary>
	/// �Z���N�g�V�[���ɖ߂�Ƃ���true��Ԃ�
	/// </summary>
	/// <returns>�Z���N�g�V�[���ɖ߂鎞��true�ɂȂ�</returns>
	Game::Scene GetNextScene() { return m_nextScene; }

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
	/// �󋵂�ω�������
	/// </summary>
	/// <param name="situation">�ύX��̏�</param>
	void ChangeSituation(Situation situation);

	/// <summary>
	/// ������̃X�e�[�^�X��CharacterStatus�ɕϊ�����
	/// </summary>
	/// <param name="statusData">�X�e�[�^�X�̏���������������</param>
	/// <returns>CharacterStatus�^�̃X�e�[�^�X���</returns>
	Character::CharacterStatus GetCharacterStatus(std::vector<std::string> statusData);

private:

	using UpdateSituationFunc = void (BattleManager::*)();

	UpdateSituationFunc m_updateSituationFunc;

	//�L�����N�^�[�̃|�C���^
	std::shared_ptr<Character> m_pCharacters[static_cast<int>(Character::PlayerNumber::kPlayerNum)];
	//�U���̏��������Ă���}�l�[�W���[
	std::vector<std::shared_ptr<Attack>> m_pAttacks;
	//�X�e�[�W�N���X
	std::shared_ptr<Stage> m_pStage;
	//�J�����N���X
	std::shared_ptr<GameCamera> m_pCamera;
	//�G�t�F�N�g�Ǘ��N���X
	std::shared_ptr<EffectManager> m_pEffectManager;
	//UI�Ǘ��N���X
	std::shared_ptr<GameUi> m_pGameUi;
	//�Q�[���̏�
	Situation m_situation;
	//���Ԃ��v��ۂɎg�p����ϐ�
	int m_time;
	//���o���̃J�������W
	MyEngine::Vector3 m_poseCameraPos;
	//�t�F�[�h���s���Ƃ��̕ϐ�
	int m_alpha;
	//�V�[�����Z���N�g�V�[���ɖ߂�����true�ɂ���
	Game::Scene m_nextScene;
};