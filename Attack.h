#pragma once
#include "Collidable.h"
class Attack : public Collidable
{
public:

	struct AttackStatus
	{
		int damage
	};

	/// <summary>
	/// �U���̃X�e�[�^�X�̐ݒ�
	/// </summary>
	void Init();

	/// <summary>
	/// �U���̍X�V(���W�̍X�V�Ȃ�)
	/// </summary>
	void Update();

	/// <summary>
	/// �`�悷����̂�����Ε`�悷��
	/// </summary>
	void Draw();
private:



};

