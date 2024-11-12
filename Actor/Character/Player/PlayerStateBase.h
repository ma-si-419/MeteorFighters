#pragma once
#include "CharacterStateBase.h"
#include "Vector3.h"

class Player;
class PlayerStateBase : public CharacterStateBase
{
public:
	PlayerStateBase(std::shared_ptr<Player> player);
protected:

	/// <summary>
	/// �G�̍��W���擾����
	/// </summary>
	/// <returns></returns>
	MyEngine::Vector3 GetEnemyPos();
	
	/// <summary>
	/// �G�̈ړ��x�N�g�����擾����
	/// </summary>
	/// <returns></returns>
	MyEngine::Vector3 GetEnemyVelo();

	/// <summary>
	/// �G�l�~�[�̂����Ԃ��擾����
	/// </summary>
	/// <returns>�G�l�~�[�̂�����</returns>
	int GetEnemyHitReaction();

	/// <summary>
	/// �v���C���[�̈ړ��x�N�g����ݒ肷��
	/// </summary>
	/// <param name="velo">�ړ��x�N�g��</param>
	void SetPlayerVelo(MyEngine::Vector3 velo);

	/// <summary>
	/// �v���C���[�̍��W��ݒ肷��
	/// </summary>
	/// <param name="pos">���[���h���W</param>
	void SetPlayerPos(MyEngine::Vector3 pos);

	/// <summary>
	/// �G�l�~�[�̔w����W���擾����
	/// </summary>
	/// <param name="distance">����</param>
	/// <returns>�G�l�~�[�̔w����W</returns>
	MyEngine::Vector3 GetEnemyBackPos(float distance);

	/// <summary>
	/// �v���C���[�̎c�����쐬����
	/// </summary>
	void CreateAfterImage();

	/// <summary>
	/// �A�j���[�V�����ԍ����擾����
	/// </summary>
	/// <param name="animName">�A�j���[�V�����̖��O</param>
	/// <returns>�A�j���[�V�����ԍ�</returns>
	int GetAttackAnimKind(std::string animName);

	/// <summary>
	/// �L�����N�^�[�̈ړ����x���擾����
	/// </summary>
	/// <returns>�g�p���Ă���L�����N�^�[�̈ړ����x</returns>
	float GetSpeed();

	/// <summary>
	/// �J�����̈ړ����~�߂�
	/// </summary>
	void StopMoveCamera();

	/// <summary>
	/// �J�����̈ړ����n�߂�
	/// </summary>
	void StartMoveCamera();

	//�v���C���[�̃|�C���^
	std::shared_ptr<Player> m_pPlayer;

};

