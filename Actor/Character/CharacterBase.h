#pragma once
#include "Actor.h"
#include "LocalPos.h"
#include <vector>
#include <list>
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
	/// �U���̎��
	/// </summary>
	enum class AttackHitKind
	{
		kLow,//��
		kMiddle,//��
		kUpBurst,//�㐁�����
		kDownBurst,//���������
		kFarBurst,//���������
		kBottomStan,//���i�X�^��
		kMiddleStan,//���i�X�^��
		kKindNum
	};

	/// <summary>
	/// �����̏��
	/// </summary>
	enum class HitReactionKind
	{
		kNone,//�Ȃɂ�����Ă��Ȃ����
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

	enum class AnimKind
	{
		kIdle,
		kSkyDash,
		kGuard,
		kDodgeLeft,
		kDodgeRight,
		kDodgeBack,
		kDodgeFront,
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
		kSkyIdle,
		kFrontBurst,
		kBackBurst,
		kLowHit1,
		kLowHit2,
		kLowHit3,
		kBackLowHit1,
		kBackLowHit2,
		kBackLowHit3,
		kMiddleHit,
		kBackMiddleHit,
		kEnergyAttackLeft,
		kEnergyAttackRight,
		kUpperAttack,
		kStanAttack,
		kLegSweepAttack,
		kBottomStan,
		kFrontMiddleStan,
		kBackMiddleStan,
		kDownChargeAttack,
		kMiddleChargeAttack,
		kUpChargeAttack,
		kEnergyChargeAttack,
		kRun,
		kJumpStart,
		kJumping,
		kJumpEnd,
		kGroundDash,
		kRushStart,
		kRushEnd,
		kDashAttack
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
		kStartMp,
		kAtk,
		kDef,
		kSpd,
		kChargeSpd,
		kChaseNum,
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
	/// �U���̎��
	/// </summary>
	enum class AttackKind
	{
		kEnergy,//�C�e�U��
		kPhysical,//�i���U��
		kBeam,//���[�U�[��̍U��
		kRush,//�Ԃ���Ɖ��o�ɐ؂�ւ��U��
		kThrow,//�K�[�h�ł��Ȃ��Ԃ���Ɖ��o�ɐ؂�ւ��U��
		kAssault//�Ԃ���Ɖ��o�����ɂ��̂܂܃_���[�W���󂯂�U��
	};

	/// <summary>
	/// �ǂ̃L�����N�^�[��
	/// </summary>
	enum class PlayerNumber
	{
		kOnePlayer,
		kTwoPlayer,
		kPlayerNum
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
		float moveSpeed = -1;
		float attackMoveSpeed = -1;
		bool isTeleportation = false;
		AttackHitKind attackHitKind = AttackHitKind::kLow;
		AttackKind attackKind = AttackKind::kPhysical;
		std::string nextComboName = "empty";
		std::string targetHitReaction = "empty";
		std::string animationName = "empty";
	};

	/// <summary>
	/// �K�E�Z�̏��
	/// </summary>
	struct SpecialAttackData
	{
		std::string name = "empty";
		AttackKind kind = AttackKind::kBeam;//�K�E�Z�̎�ނɂ���Đ�����΂����Ȃǂ𔻕ʂ���
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
		float startMp = 0;
		float atk = 0;
		float def = 0;
		float spd = 0;
		float chargeSpd = 0;
		int chaseAttackNum = 0;
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
		AttackKind attackKind = AttackKind::kPhysical;
		AttackHitKind attackHitKind = AttackHitKind::kLow;
	};

	/// <summary>
	/// �c���̏��
	/// </summary>
	struct AfterImage
	{
		int handle = -1;
		float nowOpacityRate = 1.0f;
		float maxOpacityRate = 1.0f;
		float DeleteSpeed = 0.0f;
	};

public:

	CharacterBase(ObjectTag tag, CharacterKind kind);
	~CharacterBase();

	void Init() override;

	void Update() override;

	void Draw() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

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
	/// ���݂̈ړ��x�N�g����Ԃ�
	/// </summary>
	/// <returns>���g�̈ړ��x�N�g��</returns>
	MyEngine::Vector3 GetVelo();

	/// <summary>
	/// �X�e�[�^�X�̃Z�b�g����
	/// </summary>
	/// <param name="status">�L�����N�^�[�̃X�e�[�^�X</param>
	void SetStatus(CharacterStatus status) { m_status = status; }

	/// <summary>
	/// �̗͂����炷
	/// </summary>
	/// <param name="subHp">������</param>
	void SubHp(int subHp) { m_nowHp -= subHp; }

	/// <summary>
	/// �C�͂����炷
	/// </summary>
	/// <param name="subMp">������</param>
	/// <returns>����Ȃ����false</returns>
	bool SubMp(int subMp);

	/// <summary>
	/// �C�͂𒙂߂�
	/// </summary>
	void ChargeMp();

	/// <summary>
	/// �L�����N�^�[�̍U���͂��擾����
	/// </summary>
	/// <returns>�L�����N�^�[�̍U����</returns>
	float GetPower() { return m_status.atk; }

	/// <summary>
	/// �L�����N�^�[�̈ړ����x���擾����
	/// </summary>
	/// <returns>�L�����N�^�[�̈ړ����x</returns>
	float GetSpeed() { return m_status.spd; }

	/// <summary>
	/// �L�����N�^�[�̏u�Ԉړ��U���ł���񐔂��擾
	/// </summary>
	/// <returns>�u�Ԉړ��U���ł����</returns>
	int GetChaseNum() { return m_status.chaseAttackNum; }

	/// <summary>
	/// �Đ�����A�j���[�V������ς���(�u�����h�X�s�[�h�Œ�)
	/// </summary>
	/// <param name="animKind">�A�j���[�V�������w��</param>
	/// <param name="loop">�J��Ԃ����ǂ���</param>
	void ChangeAnim(AnimKind animKind, bool loop);

	/// <summary>
	/// �Đ�����A�j���[�V������ς���(�u�����h�X�s�[�h��)
	/// </summary>
	/// <param name="animKind">�A�j���[�V�������w��</param>
	/// <param name="loop">�J��Ԃ����ǂ���</param>
	/// <param name="blendSpeed">�J��Ԃ����ǂ���</param>
	void ChangeAnim(AnimKind animKind, bool loop ,float blendSpeed);

	/// <summary>
	/// �A�j���[�V�������Đ�����
	/// </summary>
	void PlayAnim();

	/// <summary>
	/// �A�j���[�V�����̍Đ����x��ݒ肷��
	/// </summary>
	/// <param name="speed">�Đ����x</param>
	void SetAnimPlaySpeed(float speed);

	/// <summary>
	/// �A�j���[�V�����̍Đ����x�������l�ɖ߂�
	/// </summary>
	void SetAnimPlaySpeed();

	/// <summary>
	/// �A�j���[�V�������I�����������擾����
	/// </summary>
	/// <returns>true:�A�j���[�V�����I�����@false:�A�j���[�V�����Đ���</returns>
	bool IsEndAnim() { return m_isEndAnim; }

	/// <summary>
	/// csv���玝���Ă���string�f�[�^�̕K�E�Z�̎�ނ�SpecialAttackKind�ɕϊ�����
	/// </summary>
	/// <param name="kind">�K�E�Z�̎�ނ�\��string�̃f�[�^</param>
	/// <returns>SpecialAttackKind�^�̕K�E�Z�̎��</returns>
	AttackKind GetSpecialAttackKind(std::string kind);

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

	/// <summary>
	/// �A�j���[�V�����u�����h���I���������ǂ������擾����
	/// </summary>
	/// <returns>�I�����Ă�����true</returns>
	bool IsEndAnimationBlend() { return m_isEndAnimationBlend; }
	
	/// <summary>
	/// �A�j���[�V�������I�����Ă��邩�ǂ������擾����
	/// </summary>
	/// <returns>�A�j���[�V�������I�����Ă�����true</returns>
	bool IsAnimEnd();

	/// <summary>
	/// ���ݍĐ����Ă���A�j���[�V�������擾����
	/// </summary>
	/// <returns>�A�j���[�V�����̎��</returns>
	AnimKind GetPlayAnimKind() { return m_playAnimKind; }

	/// <summary>
	/// ���g���ǂ���������Ă��邩��ݒ肷��
	/// </summary>
	/// <param name="frontPos">�O���̍��W</param>
	void SetFrontPos(MyEngine::Vector3 frontPos);

	/// <summary>
	/// �O���Ƀ^�[�Q�b�g�����邩�ǂ������擾
	/// </summary>
	/// <returns>�O���ɂ���Ȃ�true</returns>
	bool IsFrontTarget();

	/// <summary>
	/// �`�悷����W��ݒ肷��(MV1SetPosition�̑���)
	/// </summary>
	/// <param name="pos">�`�悵�������W</param>
	void SetDrawPos(MyEngine::Vector3 pos);

	/// <summary>
	/// �U���̃A�j���[�V������Ԃ�
	/// </summary>
	/// <param name="animName">�A�j���[�V�����̖��O</param>
	/// <returns>�A�j���[�V�����ԍ�</returns>
	AnimKind GetAttackAnimKind(std::string animName);

	/// <summary>
	/// ���݂̂����Ԃ��擾����
	/// </summary>
	/// <returns>���݂̂�����</returns>
	HitReactionKind GetHitReaction() { return m_nowHitReaction; }

	/// <summary>
	/// �����Ԃ�ݒ肷��
	/// </summary>
	/// <param name="kind">���̂�����</param>
	void SetHitReaction(HitReactionKind kind) { m_nowHitReaction = kind; }

	/// <summary>
	/// �L�����N�^�[���n��ɂ��邩�ǂ�����Ԃ�
	/// </summary>
	/// <returns>�n��ɂ���̂Ȃ�true</returns>
	bool IsGround() { return m_pColData->IsGround(); }

	/// <summary>
	/// �^�[�Q�b�g�̕����������悤�ɂ���
	/// </summary>
	void LookTarget();

	/// <summary>
	/// �c�����쐬����
	/// </summary>
	void CreateAfterImage();

	/// <summary>
	/// ����ݒ肵�Ďc�����쐬����
	/// </summary>
	/// <param name="afterImageInfo">�n���h���ȊO�̎c���̏��</param>
	void CreateAfterImage(AfterImage afterImageInfo);

	/// <summary>
	/// �J�����𑬂����������ɌĂԊ֐�(�_�b�V������Ƃ��Ȃ�)
	/// </summary>
	void StartFastCameraMove();

	/// <summary>
	/// �w��̍��W���擾����
	/// </summary>
	/// <returns>�L�����N�^�[�������Ă�������Ƌt�����̍��W</returns>
	/// <param name="distance">�n���h���ȊO�̎c���̏��</param>
	MyEngine::Vector3 GetBackPos(float distance);

	/// <summary>
	/// �Ȃɂ��ƏՓ˂������ɕ����������s������ݒ肷��
	/// </summary>
	/// <param name="flag">�Ԃ��邩�ǂ���</param>
	void SetIsTrigger(bool flag);

	/// <summary>
	/// �L�����N�^�[�̔ԍ���ݒ�
	/// </summary>
	/// <param name="num">1P��2P����ݒ肷��</param>
	void SetCharacterNumber(PlayerNumber num) { m_playerNumber = num; }
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
		kMoveSpeed,//�ړ����x
		kAttackMoveSpeed,//�U���̈ړ����x
		kIsTeleportation,
		kAttackHitKind,//���̍U�����󂯂����̂�����
		kAttackKind,//�U���̎��
		kNextComboName,//���ɏo��U���̖��O
		kTargetHitReaction,//�G�̏�Ԃ�����������U�����o���邩
		kAnimationName//�A�j���[�V�����̖��O
	};

	
	//���g���ǂ��瑤�̃L�����N�^�[��
	PlayerNumber m_playerNumber;
	//���f���n���h��
	int m_modelHandle;
	//�V�[���̃|�C���^
	std::shared_ptr<SceneGame> m_pScene;
	//�}�l�[�W���[�̃|�C���^
	std::shared_ptr<GameManager> m_pGameManager;
	//State
	std::shared_ptr<CharacterStateBase> m_pState;
	//�쐬�����c��
	std::list<AfterImage> m_afterImageList;
	//�g���L�����N�^�[�̎��
	CharacterKind m_characterKind;
	//���g�̃X�e�[�^�X
	CharacterStatus m_status;
	//���݂̗̑�
	float m_nowHp;
	//���݂̋C��
	float m_nowMp;
	//�������Ă���������W
	LocalPos m_lookPos;
	//���݂̂�����
	HitReactionKind m_nowHitReaction;
	//���ׂẴL�����ŋ��ʂŎg���ʏ�U���̏��
	std::map<std::string, NormalAttackData> m_normalAttackData;
	//���g�̌����Ă�������Ȃǂ�ۑ����邽�߂Ƀ��[�J�����W�������Ă���
	LocalPos m_targetLocalPos;
	//���Đ����Ă���A�j��
	int m_attachAnim;
	//���Đ����Ă���A�j���̎��
	AnimKind m_playAnimKind;
	//�A�j���[�V�������I��������ǂ���
	bool m_isEndAnim;
	//�A�j���[�V�����̍��v����
	float m_totalAnimTime;
	//�A�j���[�V�����̌��݂̍Đ�����
	float m_nowPlayAnimTime;
	//�O�̃A�j���[�V����
	float m_lastPlayAnimTime;
	//�A�j���[�V�����̍Đ����x
	float m_animPlaySpeed;
	//�A�j���[�V���������[�v���邩�ǂ���
	bool m_isLoop;
	//�A�j���[�V�����̃u�����h��
	float m_animBlendRate;
	//�A�j���[�V�����̃u�����h���x
	float m_animBlendSpeed;
	//1�O�̃A�j���[�V����
	int m_lastAnim;
	//�A�j���[�V�����u�����h���I��������ǂ���
	bool m_isEndAnimationBlend;

	friend CharacterStateBase;
};