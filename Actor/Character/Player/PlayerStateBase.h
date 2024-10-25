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
	/// �v���C���[�̈ړ��x�N�g����ݒ肷��
	/// </summary>
	/// <param name="velo">�ړ��x�N�g��</param>
	void SetPlayerVelo(MyEngine::Vector3 velo);

	/// <summary>
	/// �v���C���[�̍��W��ݒ肷��
	/// </summary>
	/// <param name="pos">���[���h���W</param>
	void SetPlayerPos(MyEngine::Vector3 pos);

	void ChangeState(std::shared_ptr<CharacterStateBase> nextState) override;
	
	//�v���C���[�̃|�C���^
	std::shared_ptr<Player> m_pPlayer;

};

