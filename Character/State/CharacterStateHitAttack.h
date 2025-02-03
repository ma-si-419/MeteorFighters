#pragma once
#include "CharacterStateBase.h"
#include <list>
class CharacterStateHitAttack : public CharacterStateBase,public std::enable_shared_from_this<CharacterStateHitAttack>
{
public:

	CharacterStateHitAttack(std::shared_ptr<Character> character);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	/// <summary>
	/// �U�����󂯂����ɌĂԊ֐�
	/// </summary>
	/// <param name="reaction">�U�����󂯂����̔���(Character::HitReactionKind��int�ɃL���X�g����)</param>
	void HitAttack(int reaction);

	/// <summary>
	/// ���Đ����Ă���A�j���[�V�����Ǝ󂯂��U�����玟�ɍĐ�����A�j���[�V�������l���ĕԂ�
	/// </summary>
	/// <param name="kind">�󂯂��U���̎��</param>
	/// <returns>���ɍĐ�����A�j���[�V����</returns>
	int GetNextAnimKind(int kind);

	/// <summary>
	/// �󂯐g�ł���󋵂��ǂ�����Ԃ�
	/// </summary>
	/// <returns>�󂯐g�ł���̂Ȃ�true��Ԃ�</returns>
	bool IsFalls() { return m_isFalls; }

private:

	//��������鎞��
	int m_moveTime;

	//�ړ��x�N�g��
	MyEngine::Vector3 m_moveVec;

	//�ړ�����
	float m_moveLength;

	//�����x�N�g��
	MyEngine::Vector3 m_decelerationVec;

	//���܂Ŏ󂯂��U��
	std::list<int> m_hitReactions;

	//�O�����牣��ꂽ���ǂ���
	bool m_isFrontHit;

	//�X�e�[�W�ƂԂ��邩�ǂ���
	bool m_isStageBump;

	//�󂯐g�ł��邩�ǂ���
	bool m_isFalls;
};