#pragma once
#include "CharacterStateBase.h"
#include "Vector3.h"
class Enemy;
class Attack;
class EnemyStateBase : public CharacterStateBase
{

public:
	EnemyStateBase(std::shared_ptr<Enemy> enemy);

protected:

	/// <summary>
	/// �G�l�~�[�̈ړ��x�N�g����ύX����
	/// </summary>
	/// <param name="moveVec">�ړ��x�N�g��</param>
	void SetEnemyVelo(MyEngine::Vector3 moveVec);

	/// <summary>
	/// �U�����󂯂����ɌĂԊ֐�
	/// </summary>
	/// <param name="attack">�󂯂��U���̃N���X</param>
	/// <param name="stateKind">���݂�State</param>
	void HitAttack(std::shared_ptr<Attack> attack,CharacterStateBase::CharacterStateKind stateKind);

	//����State�ɓ����ĉ��t���[����������
	int m_time = 0;
	
	/// <summary>
	/// �v���C���[�̍��W���擾����
	/// </summary>
	/// <returns>�v���C���[�̍��W</returns>
	MyEngine::Vector3 GetPlayerPos();

	//�G�l�~�[�̃|�C���^
	std::shared_ptr<Enemy> m_pEnemy;
	
};

