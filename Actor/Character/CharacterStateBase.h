#pragma once
#include "StateBase.h"
#include <string>
class Collidable;
class CharacterStateBase : public StateBase
{
public:
	enum class CharacterStateKind
	{
		kIdle,//�������Ă��Ȃ����
		kMove,//�ړ����
		kJump,//�W�����v���
		kGuard,//�K�[�h���
		kNormalAttack,//�ʏ�U�����
		kSpecialAttack,//�K�E�Z���
		kRush,//�ˌ����
		kHitAttack,//�U�����󂯂���
		kButtonBashing,//�G�Ƃ̉�������
		kEnergyAttack,//�C�e�U��
		kCharge,//�`���[�W���
		kStateKindNum//State�̐�
	};

public:

	virtual void OnCollide(std::shared_ptr<Collidable> collider) abstract;

	CharacterStateKind GetKind() { return m_kind; }

	//����State
	std::shared_ptr<CharacterStateBase> m_pNextState;

protected:

	virtual void ChangeState(std::shared_ptr<CharacterStateBase> nextState);

	//���݂̏��
	CharacterStateKind m_kind = CharacterStateKind::kIdle;

	//State�ɓ����ĉ��t���[����������
	float m_time = 0.0f;
};

