#pragma once
#include "CharacterStateBase.h"
#include <list>
class CharacterStateHitAttack : public CharacterStateBase,std::enable_shared_from_this<CharacterStateHitAttack>
{
public:

	CharacterStateHitAttack(std::shared_ptr<CharacterBase> character);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	/// <summary>
	/// �U�����󂯂����ɌĂԊ֐�
	/// </summary>
	/// <param name="reaction">�U�����󂯂����̔���</param>
	void HitAttack(int reaction);

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	/// <summary>
	/// ���Đ����Ă���A�j���[�V�����Ǝ󂯂��U�����玟�ɍĐ�����A�j���[�V�������l���ĕԂ�
	/// </summary>
	/// <param name="kind">�󂯂��U���̎��</param>
	/// <returns>���ɍĐ�����A�j���[�V����</returns>
	int GetNextAnimKind(int kind);

private:

	//�����Ȃ�����
	int m_downTime;

	//��������鎞��
	int m_moveTime;

	//�ړ��x�N�g��
	MyEngine::Vector3 m_moveVec;

	//���܂Ŏ󂯂��U��
	std::list<int> m_hitReactions;

	//�O�����牣��ꂽ���ǂ���
	bool m_isFrontHit;
};
