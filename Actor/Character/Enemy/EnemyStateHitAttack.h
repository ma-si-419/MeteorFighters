#pragma once
#include "EnemyStateBase.h"
#include "Vector3.h"
#include <list>
class EnemyStateHitAttack : public EnemyStateBase, public std::enable_shared_from_this<EnemyStateHitAttack>
{
public:

	/// <summary>
	/// �U�����󂯂����̔���
	/// </summary>
	enum class HitReaction
	{
		kLow,//��
		kMid,//��
		kHigh,//��
		kBurst,//�������
		kBottomStan,//���i�X�^��
		kMiddleStan,//���i�X�^��
		kTopStan,//��i�X�^��
		kReactionNum
	};
	/// <summary>
	/// ������΂���
	/// </summary>
	enum class BurstPower
	{
		kNone,
		kLow,
		kMid,
		kHigh
	};

	/// <summary>
	/// �U�����󂯂��ۂɂǂ���̕����ɓ������̂�
	/// </summary>
	enum class HitDirection
	{
		kUp,
		kDown,
		kFar
	};

	/// <summary>
	/// �U�����󂯂����ɌĂԊ֐�
	/// </summary>
	/// <param name="reaction">�U�����󂯂����̔���</param>
	/// <param name="power">������ԋ���</param>
	/// <param name="direction">������ԕ���</param>
	void HitAttack(HitReaction reaction,BurstPower power,HitDirection direction);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

private:

	/// <summary>
	/// �����Ȃ����Ԃ��擾����
	/// </summary>
	/// <param name="hitReaction">�󂯂��U��</param>
	/// <returns>�����Ȃ�����</returns>
	int GetDownTime(HitReaction hitReaction);

	//�����Ȃ�����
	int m_downTime;

	//�ړ��x�N�g��
	MyEngine::Vector3 m_moveVec;

	//���܂Ŏ󂯂��U��
	std::list<HitReaction> m_hitReactions;

	//���݂̂�����
	HitReaction m_hitReaction;
};

