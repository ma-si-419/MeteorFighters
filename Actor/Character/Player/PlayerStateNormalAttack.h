#pragma once
#include "PlayerStateBase.h"
#include <string>
class PlayerStateNormalAttack : public PlayerStateBase, public std::enable_shared_from_this<PlayerStateNormalAttack>
{
public:

	PlayerStateNormalAttack(std::shared_ptr<Player> player);

	/// <summary>
	/// ����State�ɓ���Ƃ��ɍŏ��ɌĂԊ֐�
	/// </summary>
	/// <param name="key">���̃{�^�����������̂�</param>
	/// <param name="isCharge">�`���[�W�����̂�</param>
	void SetAttack(std::string key,bool isCharge);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
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

	//�i���U���ňړ�������W
	MyEngine::Vector3 m_moveTargetPos;
};

