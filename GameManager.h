#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Vector3.h"

class Player;
class Enemy;
class Attack;
class GameManager : public std::enable_shared_from_this<GameManager>
{
public:

	GameManager();
	~GameManager();


	/// <summary>
	/// �Q�[���}�l�[�W���[�̍X�V
	/// </summary>
	void Update();

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
	/// �v���C���[�̃|�C���^��Ԃ�
	/// </summary>
	/// <returns>�v���C���[�̃|�C���^</returns>
	std::shared_ptr<Player> GetPlayerPointer() { return m_pPlayer; }

	/// <summary>
	/// �G�l�~�[�̃|�C���^��Ԃ�
	/// </summary>
	/// <returns>�G�l�~�[�̃|�C���^</returns>
	std::shared_ptr<Enemy> GetEnemyPointer() { return m_pEnemy; }

	/// <summary>
	/// �v���C���[�̍��W���擾����
	/// </summary>
	/// <returns>�v���C���[�̍��W</returns>
	MyEngine::Vector3 GetPlayerPos();

	/// <summary>
	/// �G�l�~�[�̍��W���擾����
	/// </summary>
	/// <returns>�G�l�~�[�̍��W</returns>
	MyEngine::Vector3 GetEnemyPos();

	/// <summary>
	/// �Ǘ�����U����ǉ�����
	/// </summary>
	/// <param name="attack">�ǉ�����U���N���X�̃|�C���^</param>
	void AddAttack(std::shared_ptr<Attack> attack);

private:

	//�v���C���[�̃|�C���^
	std::shared_ptr<Player> m_pPlayer;
	//�G�l�~�[�̃|�C���^
	std::shared_ptr<Enemy> m_pEnemy;
	//�U���̏��������Ă���}�l�[�W���[
	std::vector<std::shared_ptr<Attack>> m_pAttacks;
};