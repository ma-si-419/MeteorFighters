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

	enum class SpecialAttackKind
	{
		kBeam,
		kPhysical,
		kEnergy,
		kEnergyRush
	};


	struct SpecialAttackInfo
	{
		std::string name;
		SpecialAttackKind kind;
		int cost = -1;
		float damageRate = -1;
		int startFrame = -1;
		int endFrame = -1;
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
	/// �Ǘ�����U�����쐬����
	/// </summary>
	/// <param name="attackNumber">�U���̔ԍ�</param>
	/// <param name="isSpecial">�K�E�Z���ǂ���</param>
	/// <param name="isPlayer">�v���C���[���o�������ǂ���</param>
	/// <param name="power">�o�����L�����N�^�[�̍U����</param>
	void AddAttack(int attackNumber,bool isSpecial,bool isPlayer,float power);

private:

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

	//�v���C���[�̕K�E�Z���
	SpecialAttackInfo m_playerSpecialAttackInfo[kSpecialAttackNum];
	//�G�l�~�[�̕K�E�Z���
	SpecialAttackInfo m_enemySpecialAttackInfo[kSpecialAttackNum];
	//��ɏo�Ă���U���̔z��
	std::vector<std::shared_ptr<Attack>> m_pAttacks;

};

