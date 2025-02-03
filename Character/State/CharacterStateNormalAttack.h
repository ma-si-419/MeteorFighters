#pragma once
#include "CharacterStateBase.h"
class CharacterStateNormalAttack : public CharacterStateBase,public std::enable_shared_from_this<CharacterStateNormalAttack>
{
public:

	CharacterStateNormalAttack(std::shared_ptr<Character> character);

	/// <summary>
	/// ����State�ɓ���Ƃ��ɍŏ��ɌĂԊ֐�
	/// </summary>
	/// <param name="key">�ǂ̃L�[�������ꂽ�̂�</param>
	/// <param name="attackName">���̍U�����s���̂�</param>
	void SetAttack(std::string key, std::string attackName);

	/// <summary>
	/// �_�b�V������U���Ȃǈړ����ω�����U���̍ۂɌĂԊ֐�
	/// </summary>
	/// <param name="velo">�U���̈ړ��x�N�g��</param>
	void SetAttackVelo(MyEngine::Vector3 velo);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	/// <summary>
	/// �U���܂ł̎��Ԃ��擾����
	/// </summary>
	/// <returns>�U���܂ł̎���(�U�������łɏo���Ă�����-1��Ԃ�)</returns>
	int GetTimeToAttack();

	std::string GetNowAttackName() { return m_nowAttackName; }

	bool GetEndAttack() { return m_isEndAttack; };
private:

	//���ݍs���Ă���U���̖��O
	std::string m_nowAttackName;

	//���ɍs���U���̖��O
	std::string m_nextAttackName;

	//���̍U���Ƀ`���[�W���ł��邩�ǂ���
	bool m_isNextCharge;

	//�`���[�W�U�����s���Ă��邩�ǂ���
	bool m_isCharge;

	//���t���[���`���[�W���s������
	float m_chargeTime;

	//�U�����s�������ǂ���
	bool m_isAttacked;

	//���L�[�������čU�����s���Ă��邩
	std::string m_attackKey;

	//���ɍU�����s�����ǂ���
	bool m_isNextAttack;

	//�u�Ԉړ��U���̒ǌ����s�������ǂ���
	int m_chaseAttackNum;

	//�ŏ��̍U�������ړ����ς��\�������邽��
	MyEngine::Vector3 m_firstAttackMoveVec;

	//�i���U���ňړ�������W
	MyEngine::Vector3 m_moveTargetPos;

	//�U�����I���������ǂ���
	bool m_isEndAttack;
};