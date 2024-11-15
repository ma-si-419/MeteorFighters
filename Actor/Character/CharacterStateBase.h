#pragma once
#include "StateBase.h"
#include "Vector3.h"
#include <string>
class CharacterBase;
class Collidable;
class Attack;
class Input;
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
		kCharge,//�`���[�W���
		kStateKindNum//State�̐�
	};

public:

	CharacterStateBase(std::shared_ptr<CharacterBase> character);

	virtual void OnCollide(std::shared_ptr<Collidable> collider) abstract;

	CharacterStateKind GetKind() { return m_kind; }

	//����State
	std::shared_ptr<CharacterStateBase> m_pNextState;

protected:

	virtual void ChangeState(std::shared_ptr<CharacterStateBase> nextState);

	/// <summary>
	/// �ΐ푊��̍��W���擾����
	/// </summary>
	/// <returns>�ΐ푊��̍��W</returns>
	MyEngine::Vector3 GetTargetPos();

	/// <summary>
	/// �ΐ푊��̈ړ��x�N�g�����擾����
	/// </summary>
	/// <returns>�ΐ푊��̈ړ��x�N�g��</returns>
	MyEngine::Vector3 GetTargetVelo();

	/// <summary>
	/// �ΐ푊��̂����Ԃ��擾����
	/// </summary>
	/// <returns>�ΐ푊��̂�����</returns>
	int GetTargetHitReaction();

	/// <summary>
	/// �v���C���[�̈ړ��x�N�g����ݒ肷��
	/// </summary>
	/// <param name="velo">�ړ��x�N�g��</param>
	void SetCharacterVelo(MyEngine::Vector3 velo);

	/// <summary>
	/// �v���C���[�̍��W��ݒ肷��
	/// </summary>
	/// <param name="pos">���[���h���W</param>
	void SetCharacterPos(MyEngine::Vector3 pos);

	/// <summary>
	/// �ΐ푊��̔w����W���擾����
	/// </summary>
	/// <param name="distance">����</param>
	/// <returns>�ΐ푊��̔w����W</returns>
	MyEngine::Vector3 GetTargetBackPos(float distance);

	/// <summary>
	/// �v���C���[�̎c�����쐬����
	/// </summary>
	void CreateAfterImage();

	/// <summary>
	/// �A�j���[�V�����ԍ����擾����
	/// </summary>
	/// <param name="animName">�A�j���[�V�����̖��O</param>
	/// <returns>�A�j���[�V�����ԍ�</returns>
	int GetAttackAnimKind(std::string animName);

	/// <summary>
	/// �L�����N�^�[�̈ړ����x���擾����
	/// </summary>
	/// <returns>�g�p���Ă���L�����N�^�[�̈ړ����x</returns>
	float GetSpeed();

	/// <summary>
	/// �J�����̈ړ����~�߂�
	/// </summary>
	void StopMoveCamera();

	/// <summary>
	/// �J�����̈ړ����n�߂�
	/// </summary>
	void StartMoveCamera();

	/// <summary>
	/// �J������h���ݒ肷��
	/// </summary>
	/// <param name="time">�h�炷����</param>
	void ShakeCamera(int time);

	/// <summary>
	/// �U�����󂯂����ɌĂԊ֐�
	/// </summary>
	/// <param name="attack">�󂯂��U���̃N���X</param>
	/// <param name="stateKind">���݂�State</param>
	void HitAttack(std::shared_ptr<Attack> attack, CharacterStateBase::CharacterStateKind stateKind);

	//���݂̏��
	CharacterStateKind m_kind = CharacterStateKind::kIdle;

	//State�ɓ����ĉ��t���[����������
	float m_time = 0.0f;

	//�����̃L�����N�^�[�̃|�C���^
	std::shared_ptr<CharacterBase> m_pCharacter;

	//�������l�ɂ���đ��삳��Ă��邩
	bool m_isPlayer;
};

