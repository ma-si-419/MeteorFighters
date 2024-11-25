#pragma once
#include "CharacterStateBase.h"
class CharacterStateSpecialAttack : public CharacterStateBase
{
public:
	CharacterStateSpecialAttack(std::shared_ptr<CharacterBase> character);

	/// <summary>
	/// �K�E�Z�̔ԍ���ݒ肷��
	/// </summary>
	/// <param name="number">�K�E�Z�̔ԍ�</param>
	void SetSpecialAttackNumber(int number) { m_specialAttackNumber = number; }

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

private:

	//�L�����N�^�[�ɐݒ肳��Ă���K�E�Z�̉��Ԗڂ��g�p���邩
	int m_specialAttackNumber;

	//�Z���o���Ă��鎞��(�㌄�̎��Ԃ͋Z���Ƃɐݒ�)
	int m_attackTime;

};