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
		MyEngine::Vector3 targetPos;
		int damage = 0;
		float speed = 0;
		int lifeTime = 0;
		float radius = 0;
		CharacterBase::AttackKind attackKind = CharacterBase::AttackKind::kPhysical;
		CharacterBase::AttackHitKind attackHitKind = CharacterBase::AttackHitKind::kLow;
	};

public:
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

	/// <summary>
	/// ���݂��Ă��邩��Ԃ�
	/// </summary>
	/// <returns>�U�������݂��Ă��邩�ǂ���</returns>
	bool IsExist() { return m_isExist; }

	/// <summary>
	/// �U���̃X�e�[�^�X��Ԃ�
	/// </summary>
	/// <returns>�U���̃X�e�[�^�X</returns>
	AttackStatus GetStatus() { return m_status; }

	/// <summary>
	/// ���g�ȊO��Collidable�ƂԂ��������̏���������
	/// </summary>
	/// <param name="collider">�Ԃ��������葤�̃R���C�_�[</param>
	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
private:

	//�U���̃X�e�[�^�X
	AttackStatus m_status;

	//�U���̈ړ������x�N�g��
	MyEngine::Vector3 m_dir;

	//�V�[���ɏo�Ă��牽�t���[����������
	int m_lifeTime;

	//���݂��Ă��邩�ǂ���
	bool m_isExist;

	//�ǔ����邩�ǂ���
	bool m_isTrack;

};

