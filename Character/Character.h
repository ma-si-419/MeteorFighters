#pragma once
#include "Collidable.h"
#include "LocalPos.h"
#include <vector>
#include <list>
#include <string>
#include <map>
#include "InputData.h"

class CharacterStateBase;
class SceneGame;
class GameManagerBase;
class Attack;
class EnemyInput;
class Character : public Collidable
{
public:

	enum class VoiceKind
	{
		kStart,
		kLowAttack,
		kMiddleAttack,
		kHighAttack,
		kSpecialAttack,
		kDodge1,
		kDodge2,
		kWinBashing,
		kVoiceNum
	};

	/// <summary>
	/// �U���̎��
	/// </summary>
	enum class AttackHitKind
	{
		kLow,//��
		kMiddle,//��
		kWeakUpBurst,//��Ɏ㐁�����
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
		kWeakUpBurst,//��Ɏ㐁�����
		kUpBurst,//�㐁�����
		kDownBurst,//���������
		kFarBurst,//���������
		kWeakBurst,//�㐁�����
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
		kGuardMiddle,
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
		kDashAttack,
		kStartFirstSpecialAttack,
		kOnFirstSpecialAttack,
		kEndFirstSpecialAttack,
		kStartSecondSpecialAttack,
		kOnSecondSpecialAttack,
		kEndSecondSpecialAttack,
		kStartPose,
		kWinPose,
		kLosePose,
		kGuardLow,
		kGuardHigh,
		kButtonBashingHitBack,
		kButtonBashingAttack,
		kOnButtonBashing,
		kBackDown,
		kFrontDown
	};

	enum class CharacterKind
	{
		kMouse,
		kTheBlue
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
		kFirstSpecialEffectName,
		kFirstSpecialCost,
		kFirstSpecialDamageRate,
		kFirstSpecialRadius,
		kFirstSpecialKind,
		kFirstSpecialStartFrame,
		kFirstSpecialEndFrame,
		kSecondSpecialName,
		kSecondSpecialEffectName,
		kSecondSpecialCost,
		kSecondSpecialDamageRate,
		kSecondSpecialRadius,
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
		kLaser,//���[�U�[��̍U��
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
		float animationSpeed = -1;
		bool isTeleportation = false;
		AttackHitKind attackHitKind = AttackHitKind::kLow;
		AttackKind attackKind = AttackKind::kPhysical;
		std::string attackName = "empty";
		std::string nextComboName = "empty";
		std::string targetHitReaction = "empty";
		std::string animationName = "empty";
		std::string effectName = "empty";
		std::string soundName = "empty";
	};

	/// <summary>
	/// �K�E�Z�̏��
	/// </summary>
	struct SpecialAttackData
	{
		std::string name = "empty";
		std::string path = "empty";
		AttackKind kind = AttackKind::kLaser;//�K�E�Z�̎�ނɂ���Đ�����΂����Ȃǂ𔻕ʂ���
		int cost = -1;
		float damageRate = -1;
		float radius = -1;
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
		std::string attackName = "empty";
		std::string effectName = "empty";
		std::string hitSoundName = "empty";
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

	Character(ObjectTag tag, CharacterKind kind);
	~Character();

	void Init();

	/// <summary>
	/// enemyInput���Z�b�g����
	/// </summary>
	/// <param name="level">�G�̋���</param>
	void SetEnemyInput(int level);

	void Update();

	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	void Final();

	/// <summary>
	/// ������x�o�g�����s���ۂɎg�p����֐�
	/// </summary>
	void RetryInit();

	/// <summary>
	/// �c���̍X�V���s��
	/// </summary>
	void UpdateAfterImage();

	/// <summary>
	/// �G�̍��W�Ȃǂ��擾���邽�߂ɃQ�[���}�l�[�W���[�̃|�C���^���Z�b�g����
	/// </summary>
	/// <param name="manager">�Q�[���}�l�[�W���[�̃|�C���^</param>
	void SetGameManager(std::shared_ptr<GameManagerBase> manager);

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
	/// ���݂̗̑͂��擾����
	/// </summary>
	/// <returns>���݂̗̑�</returns>
	float GetHp() { return m_nowHp; }

	/// <summary>
	/// ���݂̋C�͂��擾����
	/// </summary>
	/// <returns>���݂̋C��</returns>
	float GetMp() { return m_nowMp; }

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
	bool SubMp(float subMp);

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
	void ChangeAnim(AnimKind animKind, bool loop, float blendSpeed);

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
	/// <returns>�쐬�����U���̃|�C���^</returns>
	std::shared_ptr<Attack> CreateAttack(AttackData attackData);

	/// <summary>
	/// �ʏ�U���̏����擾����
	/// </summary>
	/// <param name="attackName">�擾�������ʏ�U���̖��O</param>
	/// <returns>�ʏ�U���𐶐�����ۂ̏��</returns>
	NormalAttackData GetNormalAttackData(std::string attackName);

	/// <summary>
	/// �K�E�Z�̏����擾����
	/// </summary>
	/// <param name="specialAttackName">�擾�������K�E�Z�̔ԍ�(1��2)</param>
	/// <returns>�ݒ肳��Ă���K�E�Z�̏��</returns>
	SpecialAttackData GetSpecialAttackData(int specialNumber);

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
	/// ���g�̌����Ă���������擾����
	/// </summary>
	/// <returns>���g�̑O���̍��W</returns>
	MyEngine::Vector3 GetFrontPos();

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
	/// ���݂�State�̎�ނ�Ԃ�
	/// </summary>
	int GetStateKind();

	/// <summary>
	/// �����Ԃ�ݒ肷��
	/// </summary>
	/// <param name="kind">���̂�����</param>
	void SetHitReaction(HitReactionKind kind) {m_nowHitReaction = kind; }

	/// <summary>
	/// �L�����N�^�[���n��ɂ��邩�ǂ�����Ԃ�
	/// </summary>
	/// <returns>�n��ɂ���̂Ȃ�true</returns>
	bool IsGround() { return m_pColData->IsGround(); }

	/// <summary>
	/// �L�����N�^�[���ǂɂ��邩�ǂ�����Ԃ�
	/// </summary>
	bool IsWall() { return m_pColData->IsWall(); }

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

	/// <summary>
	/// �L�����N�^�[�̔ԍ����擾����
	/// </summary>
	/// <returns>1P��2P����Ԃ�</returns>
	PlayerNumber GetPlayerNumber() { return m_playerNumber; }

	/// <summary>
	/// �󋵂ɂ���ĕω�������A�b�v�f�[�g��ύX����
	/// </summary>
	/// <param name="situation">GameManagerBase::Situation��int�^�ɃL���X�g���Ĉ����ɂ���</param>
	void ChangeSituationUpdate(int situation);

	/// <summary>
	/// ���͂��擾����
	/// </summary>
	/// <returns>���͏��</returns>
	std::shared_ptr<MyEngine::InputData> GetInputData() { return m_input; }

	/// <summary>
	/// ���f���̃p�X���擾����
	/// </summary>
	/// <returns>���f���̃p�X</returns>
	std::string GetModelPath() { return m_modelPath; }

	/// <summary>
	/// ���f���̃n���h����ݒ肷��
	/// </summary>
	/// <param name="handle">���f���̃n���h��</param>
	void SetModelHandle(int handle);

	/// <summary>
	/// ��ԋ߂��X�e�[�W�̃|���S���̍��W���擾����
	/// </summary>
	/// <returns>�X�e�[�W�̃|���S���̍��W</returns>
	MyEngine::Vector3 GetNearStagePos();	

	/// <summary>
	/// �G�l�~�[�̃C���v�b�g�N���X���擾����
	/// </summary>
	/// <returns>�G�l�~�[�̃C���v�b�g�N���X</returns>
	std::shared_ptr<EnemyInput> GetEnemyInput();


	/// <summary>
	/// �����o���ۂɌĂԊ֐�
	/// </summary>
	/// <param name="kind">�b�����̎��</param>
	void PlayVoice(VoiceKind kind);

	/// <summary>
	/// �ǂ̂��炢�`����W�����炷����ݒ肷��
	/// </summary>
	/// <param name="vec">���炷�x�N�g��</param>
	void SetDrawShiftVec(MyEngine::Vector3 vec) { m_drawShiftVec = vec; }

private:

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
		kAnimationName,//�A�j���[�V�����̖��O
		kAnimationSpeed,//�A�j���[�V�����̍Đ����x
		kEffectName,//�U���̃G�t�F�N�g�̖��O
		kSoundName//�U���̉��̖��O
	};

private:

	/// <summary>
	/// �J�n���̏�����
	/// </summary>
	void InitStart();

	/// <summary>
	/// �J�n���̉��o���̃A�b�v�f�[�g
	/// </summary>
	void UpdateStart();

	/// <summary>
	/// �o�g�����A�b�v�f�[�g
	/// </summary>
	void UpdateBattle();

	/// <summary>
	/// �m�b�N�A�E�g���̃A�b�v�f�[�g
	/// </summary>
	void UpdateKnockOut();

	/// <summary>
	/// ���U���g���̃A�b�v�f�[�g
	/// </summary>
	void UpdateResult();

	/// <summary>
	/// �A�C�h�����̃A�b�v�f�[�g
	/// </summary>
	void UpdateIdle();

	/// <summary>
	/// �������Ȃ����̃A�b�v�f�[�g
	/// </summary>
	void UpdateNone();

private:
	using UpdateSituationFunc = void (Character::*)();

	UpdateSituationFunc m_updateSituationFunc;

	//���f���̃p�X
	std::string m_modelPath;
	//�G�l�~�[�̓��͂��󂯕t����N���X(1P���͎g�p���Ȃ�)
	std::shared_ptr<EnemyInput> m_pEnemyInput;
	//���͏��
	std::shared_ptr<MyEngine::InputData> m_input;
	//���g���ǂ��瑤�̃L�����N�^�[��
	PlayerNumber m_playerNumber;
	//���f���n���h��
	int m_modelHandle;
	//�V�[���̃|�C���^
	std::shared_ptr<SceneGame> m_pScene;
	//�}�l�[�W���[�̃|�C���^
	std::shared_ptr<GameManagerBase> m_pBattleManager;
	//State
	std::shared_ptr<CharacterStateBase> m_pState;
	//�쐬�����c��
	std::list<AfterImage> m_afterImageList;
	//�g���L�����N�^�[�̎��
	CharacterKind m_characterKind;
	//���g�̃X�e�[�^�X
	CharacterStatus m_status;
	//�`����W���ǂ̂��炢���炷��(�󒆂ŗh���ۂȂǂɎg�p)
	MyEngine::Vector3 m_drawShiftVec;
	//���݂̗̑�
	float m_nowHp;
	//���݂̋C��
	float m_nowMp;
	//�������Ă���������W
	MyEngine::LocalPos m_lookPos;
	//���݂̂�����
	HitReactionKind m_nowHitReaction;
	//���ׂẴL�����ŋ��ʂŎg���ʏ�U���̏��
	std::map<std::string, NormalAttackData> m_normalAttackData;
	//���g�̌����Ă�������Ȃǂ�ۑ����邽�߂Ƀ��[�J�����W�������Ă���
	MyEngine::LocalPos m_targetLocalPos;
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
	//�m�b�N�A�E�g���̃x���V�e�B
	MyEngine::Vector3 m_knockOutVelo;
	//�L�����N�^�[��`�悷�邩�ǂ���
	bool m_isDrawCharacter;
	//�b���Ă��鐺�̃v���C�n���h��
	int m_voiceHandle;

	friend CharacterStateBase;
};