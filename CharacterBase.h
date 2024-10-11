#pragma once
#include "Actor.h"
class SceneGame;
class GameManager;
class CharacterStateBase;
class CharacterBase : public Actor
{
public: 

	enum class AnimKind
	{
		kIdle,
		kGuard,
		kStep,
		kMove
	};

	enum class CharacterKind
	{
		kMouse,
		kBigBoy,
		kBlueHead
	};

	enum class CharacterStatusDataSort
	{
		kHp,
		kAtk,
		kDef,
		kSpd,
		kChargeSpd
	};

	struct CharacterStatus
	{
		float hp = 0;
		float atk = 0;
		float def = 0;
		float spd = 0;
		float chargeSpd = 0;
	};

	CharacterBase(ObjectTag tag,CharacterKind kind);
	~CharacterBase();

	/// <summary>
	/// �G�̍��W�Ȃǂ��擾���邽�߂ɃQ�[���}�l�[�W���[�̃|�C���^���Z�b�g����
	/// </summary>
	/// <param name="manager">�Q�[���}�l�[�W���[�̃|�C���^</param>
	void SetGameManager(std::shared_ptr<GameManager> manager);

	/// <summary>
	/// ���݂̃��[���h���W��Ԃ�
	/// </summary>
	/// <returns>���g�̃��[���h���W</returns>
	MyEngine::Vector3 GetPos();

	/// <summary>
	/// �X�e�[�^�X�̃Z�b�g����
	/// </summary>
	/// <param name="status">�L�����N�^�[�̃X�e�[�^�X</param>
	void SetStatus(CharacterStatus status) { m_status = status; }

	/// <summary>
	/// �L�����N�^�[�̍U���͂��擾����
	/// </summary>
	/// <returns>�L�����N�^�[�̍U����</returns>
	float GetPower() { return m_status.atk; }

	/// <summary>
	/// �Đ�����A�j���[�V������ς���
	/// </summary>
	/// <param name="animNumber">�A�j���[�V�����ԍ�</param>
	/// <param name="loop">�J��Ԃ����ǂ���</param>
	void ChangeAnim(int animNumber,bool loop);

	/// <summary>
	/// �A�j���[�V�������Đ�����
	/// </summary>
	void PlayAnim();

	/// <summary>
	/// �A�j���[�V�������I�����������擾����
	/// </summary>
	/// <returns>true:�A�j���[�V�����I�����@false:�A�j���[�V�����Đ���</returns>
	bool GetAnimEnd();

protected:

	

	//���f���n���h��
	int m_modelHandle;
	//�V�[���̃|�C���^
	std::shared_ptr<SceneGame> m_pScene;
	//�}�l�[�W���[�̃|�C���^
	std::shared_ptr<GameManager> m_pGameManager;
	//State
	std::shared_ptr<CharacterStateBase> m_pState;
	//�g���L�����N�^�[�̎��
	CharacterKind m_characterKind;
	//���g�̃X�e�[�^�X
	CharacterStatus m_status;


	//�A�j���[�V�����̏��
	int m_attachAnim;
	float m_totalAnimTime;
	float m_playAnimTime;
	float m_animPlaySpeed;
	bool m_isLoop;

};