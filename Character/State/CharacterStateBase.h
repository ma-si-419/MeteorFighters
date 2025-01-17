#pragma once
#include "StateBase.h"
#include "Vector3.h"
#include <string>
#include "InputData.h"
class Character;
class Collidable;
class Attack;
class Input;
class Effect;
class EffectManager;
class GameManagerBase;
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

	CharacterStateBase(std::shared_ptr<Character> character);

	virtual void OnCollide(std::shared_ptr<Collidable> collider);

	/// <summary>
	/// ��ɍX�V���s�����̂����X�V����
	/// </summary>
	void UpdateCommon();

	/// <summary>
	///	State�̎�ނ��擾����
	/// </summary>
	/// <returns>State�̎��</returns>
	CharacterStateKind GetKind() { return m_kind; }

	//����State
	std::shared_ptr<CharacterStateBase> m_pNextState;

protected:

	virtual void ChangeState(std::shared_ptr<CharacterStateBase> nextState);

	std::shared_ptr<MyEngine::InputData> GetCharacterInput();

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
	/// �U�����󂯂����ɌĂԊ֐�
	/// </summary>
	/// <param name="attack">�󂯂��U���̃N���X</param>
	/// <param name="stateKind">���݂�State</param>
	void HitAttack(std::shared_ptr<Attack> attack, CharacterStateBase::CharacterStateKind stateKind);

	/// <summary>
	/// �`���[�g���A���N���A������B�������Ƃ���
	/// </summary>
	/// <param name="tutorialNumber">�`���[�g���A���̔ԍ�(TutorialManager::TutorialSuccessKind��int�ɃL���X�g����)</param>
	void SuccessTutorial(int tutorialNumber);

protected:

	//���݂̏��
	CharacterStateKind m_kind = CharacterStateKind::kIdle;

	//State�ɓ����ĉ��t���[����������
	float m_time = 0.0f;

	//�����̃L�����N�^�[�̃|�C���^
	std::shared_ptr<Character> m_pCharacter;

	//�o�g���}�l�[�W���[
	std::shared_ptr<GameManagerBase> m_pManager;

	//�����ɂ��Ă���G�t�F�N�g(��{�I��1�����Đ�����悤�ɂ���)
	std::shared_ptr<Effect> m_pEffect;
};

