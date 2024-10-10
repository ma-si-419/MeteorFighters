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
	/// プレイヤーとエネミーが使用する必殺技の情報を設定する
	/// </summary>
	/// <param name="playerCharacterNumber">プレイヤーのキャラクター番号</param>
	/// <param name="enemyCharacterNumber">エネミーのキャラクター番号</param>
	void Init(int playerCharacterNumber,int enemyCharacterNumber);

	/// <summary>
	/// 攻撃のアップデートを行う
	/// </summary>
	void Update();

	/// <summary>
	/// 攻撃の描画を行う
	/// </summary>
	void Draw();

	/// <summary>
	/// 管理する攻撃を作成する
	/// </summary>
	/// <param name="attackNumber">攻撃の番号</param>
	/// <param name="isSpecial">必殺技かどうか</param>
	/// <param name="isPlayer">プレイヤーが出したかどうか</param>
	/// <param name="power">出したキャラクターの攻撃力</param>
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

	//プレイヤーの必殺技情報
	SpecialAttackInfo m_playerSpecialAttackInfo[kSpecialAttackNum];
	//エネミーの必殺技情報
	SpecialAttackInfo m_enemySpecialAttackInfo[kSpecialAttackNum];
	//場に出ている攻撃の配列
	std::vector<std::shared_ptr<Attack>> m_pAttacks;

};

