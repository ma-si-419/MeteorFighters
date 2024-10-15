#pragma once
#include "Collidable.h"
class Attack : public Collidable
{
public:

	/// <summary>
	/// �U���̃X�e�[�^�X
	/// </summary>
	struct AttackStatus
	{
		int damage = 0;
		float speed = 0;
	};

	/// <summary>
	/// �U���̃X�e�[�^�X�̐ݒ�
	/// </summary>
	void Init(AttackStatus status);

	/// <summary>
	/// �U���̍X�V(���W�̍X�V�Ȃ�)
	/// </summary>
	void Update();

	/// <summary>
	/// �`�悷����̂�����Ε`�悷��
	/// </summary>
	void Draw();
private:

	AttackStatus m_status;
};

