#pragma once
#include "CharacterStateBase.h"
class Effect;
class Attack;
class CharacterStateSpecialAttack : public CharacterStateBase,public std::enable_shared_from_this<CharacterStateSpecialAttack>
{
public:
	CharacterStateSpecialAttack(std::shared_ptr<Character> character);

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

	//�G�̏�Ԃ����ɂȂ�����Z���o���̂���߂邩
	int m_endHitReaction;

	//�U�����o�������ǂ���
	bool m_isAttacked;

	//�U���̃G�t�F�N�g
	std::shared_ptr<Effect> m_pEffect;

	//�o�����U���̃|�C���^
	std::shared_ptr<Attack> m_pAttack;
};