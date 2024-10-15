#pragma once
#include <string>
#include <vector>
#include <memory>

namespace
{
	constexpr int kSpecialAttackNum = 2;
}

class Attack;
class AttackManager
{
public:

	/// <summary>
	/// �K�E�Z�̎��
	/// </summary>
	enum class SpecialAttackKind
	{
		kBeam,
		kPhysical,
		kEnergy,
		kEnergyRush
	};
	/// <summary>
	/// �U�����󂯂����̔���
	/// </summary>
	enum class HitReaction
	{
		kRow,//��
		kMid,//��
		kHigh,//��
		kBurst,//�������
		kStun//����
	};

	/// <summary>
	/// ������΂���
	/// </summary>
	enum class BurstPower
	{
		kNone,
		kRow,
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



	/// <summary>
	/// �K�E�Z�̏��
	/// </summary>
	struct SpecialAttackInfo
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
	/// �ʏ�U���̏��
	/// </summary>
	struct NormalAttackInfo
	{
		float damageRate = -1;
		int totalFrame = -1;
		int attackFrame = -1;
		HitReaction hitReaction = HitReaction::kRow;
		HitDirection hitDirection = HitDirection::kFar;
		BurstPower burstPower = BurstPower::kNone;
		std::string animationName = "empty";
	};

	/// <summary>
	/// �U�����V�[���ɒǉ�����ۂɎg�p����\����
	/// </summary>
	struct AddAttackInfo
	{
		MyEngine::Vector3 pos;
		int attackNumber = 0;
		bool isSpecial = false;
		bool isPlayer = true;
		float power = 0.0f;
	};


	/// <summary>
	/// �v���C���[�ƃG�l�~�[���g�p����K�E�Z�̏���ݒ肷��
	/// </summary>
	/// <param name="playerCharacterNumber">�v���C���[�̃L�����N�^�[�ԍ�</param>
	/// <param name="enemyCharacterNumber">�G�l�~�[�̃L�����N�^�[�ԍ�</param>
	void Init(int playerCharacterNumber,int enemyCharacterNumber);

	/// <summary>
	/// �U���̃A�b�v�f�[�g���s��
	/// </summary>
	void Update();

	/// <summary>
	/// �U���̕`����s��
	/// </summary>
	void Draw();

	/// <summary>
	/// �Ǘ�����U�����쐬���A�ǉ�����(�U��������V�[���ɒǉ�����Ƃ��Ɏg�p����)
	/// </summary>
	/// <param name="attackInfo">�ǉ�����U���̏��</param>
	void AddAttack(AddAttackInfo attackInfo);

private:

	/// <summary>
	/// �K�E�Z�̃t�@�C�����J�����ۂ̏��̕��я�
	/// </summary>
	enum class SpecialAttackDataSort
	{
		kCharacterName,
		kFirstName,
		kFirstCost,
		kFirstDamageRate,
		kFirstKind,
		kFirstStartFrame,
		kFirstEndFrame,
		kSecondName,
		kSecondCost,
		kSecondDamageRate,
		kSecondKind,
		kSecondStartFrame,
		kSecondEndFrame
	};

	enum class NormalAttackDataSort
	{
		kAttackName,//�Q�[�����ł͎g�p���Ȃ��f�[�^
		kDamageRate,//�_���[�W�{��
		kTotalFrame,//���t���[��
		kAttackFrame,//�U�������t���[��
		kHitReaction,//�U�����󂯂��ۂ̔���
		kHitDirection,//�U�����󂯂����̐�����ԕ���
		kBurstPower,//������ԋ���
		kAnimationName//�A�j���[�V�����̖��O
	};

	//�v���C���[�̕K�E�Z���
	SpecialAttackInfo m_playerSpecialAttackInfo[kSpecialAttackNum];
	//�G�l�~�[�̕K�E�Z���
	SpecialAttackInfo m_enemySpecialAttackInfo[kSpecialAttackNum];
	//�e�L�������ʂ̒ʏ�U���̏��
	NormalAttackInfo m_normalAttackInfo;

	//��ɏo�Ă���U���̔z��
	std::vector<std::shared_ptr<Attack>> m_pAttacks;

};

