#pragma once
#include "Collidable.h"
#include "Character.h"
class Effect;
class EffectManager;
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
		std::string attackName = "empty";
		std::string effectName = "empty";
		std::string hitSoundName = "empty";
		Character::AttackKind attackKind = Character::AttackKind::kPhysical;
		Character::AttackHitKind attackHitKind = Character::AttackHitKind::kLow;
	};

public:
	Attack(ObjectTag tag,MyEngine::Vector3 pos);

	virtual ~Attack();

	/// <summary>
	/// �U���̃X�e�[�^�X�̐ݒ�
	/// </summary>
	void Init(AttackStatus status,std::shared_ptr<EffectManager> manager);

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
	/// �G�t�F�N�g�̍Đ����~�߂�
	/// </summary>
	void StopEffect();

	/// <summary>
	/// �G�t�F�N�g�̍Đ����Ԃ�ݒ肷��
	/// </summary>
	/// <param name="time">�Đ�����</param>
	void SetEffectLifeTime(int time);

	/// <summary>
	/// �U�������������Ƃ��̃G�t�F�N�g�̎�ނ�Ԃ�
	/// </summary>
	/// <returns>Effeck::EffeckKind��int�ɃL���X�g�������̂�Ԃ�</returns>
	int GetHitEffeckKind();

	/// <summary>
	/// ���݂��Ă��邩��Ԃ�
	/// </summary>
	/// <returns>�U�������݂��Ă��邩�ǂ���</returns>
	bool IsExist() { return m_isExist; }

	/// <summary>
	/// �U��������
	/// </summary>
	void DeleteAttack() { m_isExist = false; }

	/// <summary>
	/// �U���̃X�e�[�^�X��Ԃ�
	/// </summary>
	/// <returns>�U���̃X�e�[�^�X</returns>
	AttackStatus GetStatus() { return m_status; }

	/// <summary>
	/// �U���̍��W��Ԃ�
	/// </summary>
	/// <returns>�U���̒��S���W</returns>
	MyEngine::Vector3 GetPos() { return m_rigidbody.GetPos(); }

	/// <summary>
	/// 1�t���[���O�̍��W��Ԃ�
	/// </summary>
	/// <returns>1�t���[���O�̍��W</returns>
	MyEngine::Vector3 GetLastPos() { return m_lastPos; }

	/// <summary>
	/// ���g�ȊO��Collidable�ƂԂ��������̏���������
	/// </summary>
	/// <param name="collider">�Ԃ��������葤�̃R���C�_�[</param>
	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

private:

	/// <summary>
	/// �G�̍U���ƂԂ������Ƃ��Ɏ��g�̍U�����������ǂ���
	/// </summary>
	/// <param name="attack">����̍U��</param>
	/// <returns>�U���������Ȃ�true</returns>
	bool IsDelete(std::shared_ptr<Attack> attack);

private:

	//�O�̃t���[���̍��W
	MyEngine::Vector3 m_lastPos;

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

	//�G�t�F�N�g
	std::shared_ptr<Effect> m_pEffect;

	//�G�t�F�N�g�}�l�[�W���[�������Ă���
	std::shared_ptr<EffectManager> m_pEffectManager;
};