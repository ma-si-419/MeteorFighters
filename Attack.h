#pragma once
#include "Collidable.h"
#include "CharacterBase.h"
class Attack : public Collidable
{
public:

	/// <summary>
	/// �U���̃X�e�[�^�X
	/// </summary>
	struct AttackStatus
	{
		int damage = 0;
		float speed = 0;
		int lifeTime = 0;
		float radius = 0;
		CharacterBase::BurstPower burstPower = CharacterBase::BurstPower::kNone;
		CharacterBase::HitDirection hitDirection = CharacterBase::HitDirection::kFar;
		CharacterBase::HitReaction hitReaction = CharacterBase::HitReaction::kLow;
	};

	Attack(ObjectTag tag,MyEngine::Vector3 pos);

	virtual ~Attack();

	/// <summary>
	/// �U���̃X�e�[�^�X�̐ݒ�
	/// </summary>
	void Init(AttackStatus status);

	/// <summary>
	/// �U���̍X�V(���W�̍X�V�Ȃ�)
	/// </summary>
	void Update();

	/// <summary>
	/// �`�悷����̂�����Ε`�悷��
	/// </summary>
	void Draw();

	/// <summary>
	/// ���̃N���X���������ɌĂяo��
	/// </summary>
	void Final();

	bool IsGetExist() { return m_isExist; }

	/// <summary>
	/// ���g�ȊO��Collidable�ƂԂ��������̏���������
	/// </summary>
	/// <param name="collider">�Ԃ��������葤�̃R���C�_�[</param>
	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
private:

	//�U���̃X�e�[�^�X
	AttackStatus m_status;

	//�V�[���ɏo�Ă��牽�t���[����������
	int m_lifeTime;

	//���݂��Ă��邩�ǂ���
	bool m_isExist;

};

