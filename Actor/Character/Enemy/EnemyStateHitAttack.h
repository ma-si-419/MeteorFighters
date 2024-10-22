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
	enum class HitKind
	{
		kLow,//��
		kMiddle,//��
		kUpBurst,//�㐁�����
		kDownBurst,//���������
		kFarBurst,//���������
		kBottomStan,//���i�X�^��
		kMiddleStan,//���i�X�^��
		kGuard,//�K�[�h���
		kGuardBreak,//�K�[�h�u���C�N���
		kKindNum
	};

public:

	EnemyStateHitAttack(std::shared_ptr<Enemy> enemy);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	/// <summary>
	/// �U�����󂯂����ɌĂԊ֐�
	/// </summary>
	/// <param name="reaction">�U�����󂯂����̔���</param>
	void HitAttack(HitKind reaction);
	
	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

private:

	//�����Ȃ�����
	int m_downTime;

	//��������鎞��
	int m_moveTime;

	//�ړ��x�N�g��
	MyEngine::Vector3 m_moveVec;

	//���܂Ŏ󂯂��U��
	std::list<HitKind> m_hitReactions;

	//���݂̂�����
	HitKind m_hitReaction;
};

