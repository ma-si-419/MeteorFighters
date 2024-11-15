#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Vector3.h"
#include "CharacterBase.h"

class Player;
class Enemy;
class GameCamera;
class Attack;
class Stage;
class GameManager : public std::enable_shared_from_this<GameManager>
{
public:

	GameManager(std::shared_ptr<GameCamera> camera);
	~GameManager();

	/// <summary>
	/// �Q�[���}�l�[�W���[�̏�����
	/// </summary>
	void Init();

	/// <summary>
	/// �Q�[���}�l�[�W���[�̍X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �U���̕`��Ȃǂ��s��
	/// </summary>
	void Draw();

	/// <summary>
	/// �v���C���[�����삷��L�����N�^�[��ݒ肷��
	/// </summary>
	/// <param name="number">�v���C���[���g���L�����N�^�[�̔ԍ�</param>
	void SetPlayerStatus(int number, std::vector<std::string> statusData);

	/// <summary>
	/// �G�l�~�[�����삷��L�����N�^�[��ݒ肷��
	/// </summary>
	/// <param name="number">�G�l�~�[���g���L�����N�^�[�̔ԍ�</param>
	void SetEnemyStatus(int number, std::vector<std::string> statusData);

	/// <summary>
	/// 1P�̃|�C���^��Ԃ�
	/// </summary>
	/// <returns>1P�̃|�C���^</returns>
	std::shared_ptr<CharacterBase> GetOnePlayerPointer() { return m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kOnePlayer)]; }

	/// <summary>
	/// 2P�̃|�C���^��Ԃ�
	/// </summary>
	/// <returns>2P�̃|�C���^</returns>
	std::shared_ptr<CharacterBase> GetTwoPlayerPointer() { return m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kTwoPlayer)]; }

	/// <summary>
	/// �ΐ푊��̍��W���擾����
	/// </summary>
	/// <param name="character">���̊֐����Ă񂾃L�����N�^�[�̃|�C���^</param>
	/// <returns>�ΐ푊��̍��W</returns>
	MyEngine::Vector3 GetTargetPos(std::shared_ptr<CharacterBase> character);

	/// <summary>
	/// �ΐ푊��̈ړ��x�N�g�����擾����
	/// </summary>
	/// <param name="character">���̊֐����Ă񂾃L�����N�^�[�̃|�C���^</param>
	/// <returns>�ΐ푊��̍��W</returns>
	MyEngine::Vector3 GetTargetVelo(std::shared_ptr<CharacterBase> character);

	/// <summary>
	/// �ΐ푊��̍��̂����Ԃ��擾����
	/// </summary>
	/// <param name="character">���̊֐����Ă񂾃L�����N�^�[�̃|�C���^</param>
	/// <returns>�ΐ푊��̂�����</returns>
	CharacterBase::HitReactionKind GetTargetHitReaction(std::shared_ptr<CharacterBase> character);

	/// <summary>
	/// �ΐ푊��̔w����W���擾����
	/// </summary>
	/// <param name="distance">�w��̋���</param>
	/// <param name="character">���g�̃|�C���^</param>
	/// <returns>�ΐ푊��̔w����W</returns>
	MyEngine::Vector3 GetTargetBackPos(float distance, std::shared_ptr<CharacterBase> character);

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
	/// �J�����̈ړ����~�߂�
	/// </summary>
	void StopMoveCamera();

	/// <summary>
	/// �J�����̈ړ����n�߂�
	/// </summary>
	void StartMoveCamera();


	/// <summary>
	/// �J������h���ݒ肷��
	/// </summary>
	/// <param name="time">�h�炷����</param>
	void ShakeCamera(int time);

private:

	//�L�����N�^�[�̃|�C���^
	std::shared_ptr<CharacterBase> m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kPlayerNum)];
	//�U���̏��������Ă���}�l�[�W���[
	std::vector<std::shared_ptr<Attack>> m_pAttacks;
	//�X�e�[�W�N���X
	std::shared_ptr<Stage> m_pStage;
	//�J�����N���X
	std::shared_ptr<GameCamera> m_pCamera;
	
};