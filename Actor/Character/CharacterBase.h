#pragma once
#include "Actor.h"
#include <vector>
#include <string>
#include <map>
class SceneGame;
class GameManager;
class CharacterStateBase;
class Attack;
class CharacterBase : public Actor
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
		kTopStan//��i�X�^��
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

	enum class AnimKind
	{
		kIdle,
		kAssault,
		kGuard,
		kDodge,
		kStartCharge,
		kInCharge,
		kEndCharge,
		kLowAttack1,
		kLowAttack2,
		kLowAttack3,
		kLowAttack4,
		kLowAttack5,
		kLowAttack6,
		kLowAttack7,
		kLowAttack8,
		kSkyIdle
	};

	enum class CharacterKind
	{
		kMouse,
		kBigBoy,
		kBlueHead
	};

	enum class CharacterStatusDataSort
	{
		kName,
		kHp,
		kAtk,
		kDef,
		kSpd,
		kChargeSpd,
		kFirstSpecialName,
		kFirstSpecialCost,
		kFirstSpecialDamageRate,
		kFirstSpecialKind,
		kFirstSpecialStartFrame,
		kFirstSpecialEndFrame,
		kSecondSpecialName,
		kSecondSpecialCost,
		kSecondSpecialDamageRate,
		kSecondSpecialKind,
		kSecondSpecialStartFrame,
		kSecondSpecialEndFrame
	};

	/// <summary>
	/// �K�E�Z�̎��
	/// </summary>
	enum class SpecialAttackKind
	{
		kBeam,//���[�U�[��̍U��
		kRush,//�Ԃ���Ɖ��o�ɐ؂�ւ��U��
		kEnergy,//�C�e�U��
		kThrow,//�K�[�h�ł��Ȃ��Ԃ���Ɖ��o�ɐ؂�ւ��U��
		kAssault//�Ԃ���Ɖ��o�����ɂ��̂܂܃_���[�W���󂯂�U��
	};

	/// <summary>
	/// �ʏ�U���̏��
	/// </summary>
	struct NormalAttackData
	{
		float damageRate = -1;
		int totalFrame = -1;
		int attackFrame = -1;
		int cancelFrame = -1;
		HitReaction hitReaction = HitReaction::kLow;
		HitDirection hitDirection = HitDirection::kFar;
		BurstPower burstPower = BurstPower::kNone;
		std::string nextLowComboName = "empty";
		std::string nextHighComboName = "empty";
		std::string animationName = "empty";
	};

	/// <summary>
	/// �K�E�Z�̏��
	/// </summary>
	struct SpecialAttackData
	{
		std::string name;
		SpecialAttackKind kind;//�K�E�Z�̎�ނɂ���Đ�����΂����Ȃǂ𔻕ʂ���
		int cost = -1;
		float damageRate = -1;
		int startFrame = -1;//�����t���[��
		//�U�����p�����鎞�Ԃ͕K�E�Z�̎�ނ���擾����
		int endFrame = -1;//�U���I����̍d���t���[��
	};

	/// <summary>
	/// �L�����N�^�[�̃X�e�[�^�X
	/// </summary>
	struct CharacterStatus
	{
		std::string name = "empty";
		float hp = 0;
		float atk = 0;
		float def = 0;
		float spd = 0;
		float chargeSpd = 0;
		SpecialAttackData firstSpecialAttackData;
		SpecialAttackData secondSpecialAttackData;
	};

	/// <summary>
	/// �U�����쐬����Ƃ��ɕK�v�ȕϐ����܂Ƃ߂��\����
	/// </summary>
	struct AttackData
	{
		int damage = 0;
		float speed = 0;
		int lifeTime = 0;
		bool isPlayer = true;
		float radius = 0;
		HitReaction hitReaction = HitReaction::kLow;
		HitDirection hitDirection = HitDirection::kFar;
		BurstPower burstPower = BurstPower::kNone;
	};

	CharacterBase(ObjectTag tag, CharacterKind kind);
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
	/// <param name="animKind">�A�j���[�V�������w��</param>
	/// <param name="loop">�J��Ԃ����ǂ���</param>
	void ChangeAnim(AnimKind animKind, bool loop);

	/// <summary>
	/// �A�j���[�V�������Đ�����
	/// </summary>
	void PlayAnim();

	/// <summary>
	/// �A�j���[�V�������I�����������擾����
	/// </summary>
	/// <returns>true:�A�j���[�V�����I�����@false:�A�j���[�V�����Đ���</returns>
	bool IsGetAnimEnd();

	/// <summary>
	/// csv���玝���Ă���string�f�[�^�̕K�E�Z�̎�ނ�SpecialAttackKind�ɕϊ�����
	/// </summary>
	/// <param name="kind">�K�E�Z�̎�ނ�\��string�̃f�[�^</param>
	/// <returns>SpecialAttackKind�^�̕K�E�Z�̎��</returns>
	SpecialAttackKind GetSpecialAttackKind(std::string kind);

	/// <summary>
	/// �e�L�������ʂŎg�p����ʏ�U����ݒ肷��
	/// </summary>
	/// <param name="normalAttackData">csv�t�@�C�����玝���Ă����ʏ�U���̏��</param>
	void SetNormalAttackData(std::vector<std::vector<std::string>> normalAttackData);

	/// <summary>
	/// �U�����쐬���ăV�[����ɏo��
	/// </summary>
	/// <param name="attackData">�U���̏��</param>
	void CreateAttack(AttackData attackData);

	/// <summary>
	/// �ʏ�U���̏����擾����
	/// </summary>
	/// <param name="attackName">�擾�������ʏ�U���̖��O</param>
	/// <returns>�ʏ�U���𐶐�����ۂ̏��</returns>
	NormalAttackData GetNormalAttackData(std::string attackName);

protected:

	/// <summary>
	/// �e�L�������ʂ̍U���̏���csv�t�@�C���ɓ����Ă��鏇��
	/// </summary>
	enum class NormalAttackDataSort
	{
		kAttackName,//�U���̖��O
		kDamageRate,//�_���[�W�{��
		kTotalFrame,//���t���[��
		kAttackFrame,//�U�������t���[��
		kCancelFrame,//���̍U���Ɉڍs�ł���悤�ɂȂ�t���[��
		kHitReaction,//�U�����󂯂��ۂ̔���
		kHitDirection,//�U�����󂯂����̐�����ԕ���
		kBurstPower,//������ԋ���
		kLowComboName,//X�{�^�����������Ƃ��Ɏ��ɏo��U���̖��O
		kHighComboName,//Y�{�^�����������Ƃ��Ɏ��ɏo��U���̖��O
		kAnimationName//�A�j���[�V�����̖��O
	};


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
	//���ׂẴL�����ŋ��ʂŎg���ʏ�U���̏��
	std::map<std::string,NormalAttackData> m_normalAttackData;

	//�A�j���[�V�����̏��
	int m_attachAnim;
	float m_totalAnimTime;
	float m_playAnimTime;
	float m_animPlaySpeed;
	bool m_isLoop;
	float m_animBlendRate;
	int m_lastAnim;

};