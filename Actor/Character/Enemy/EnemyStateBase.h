#pragma once
#include "CharacterStateBase.h"

class Enemy;
class EnemyStateBase : public CharacterStateBase
{
protected:
	//State�̐ؑ�
	void ChangeState(std::shared_ptr<CharacterStateBase> nextState) override;

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

