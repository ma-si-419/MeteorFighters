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
	/// 必殺技の種類
	/// </summary>
	enum class SpecialAttackKind
	{
		kBeam,
		kPhysical,
		kEnergy,
		kEnergyRush
	};
	/// <summary>
	/// 攻撃を受けた時の反応
	/// </summary>
	enum class HitReaction
	{
		kRow,//弱
		kMid,//中
		kHigh,//強
		kBurst,//吹っ飛び
		kStun//怯み
	};

	/// <summary>
	/// 吹き飛ばす力
	/// </summary>
	enum class BurstPower
	{
		kNone,
		kRow,
		kMid,
		kHigh
	};

	/// <summary>
	/// 攻撃を受けた際にどちらの方向に動かすのか
	/// </summary>
	enum class HitDirection
	{
		kUp,
		kDown,
		kFar
	};



	/// <summary>
	/// 必殺技の情報
	/// </summary>
	struct SpecialAttackInfo
	{
		std::string name;
		SpecialAttackKind kind;//必殺技の種類によって吹き飛ばし方などを判別する
		int cost = -1;
		float damageRate = -1;
		int startFrame = -1;//発生フレーム
		//攻撃を継続する時間は必殺技の種類から取得する
		int endFrame = -1;//攻撃終了後の硬直フレーム
	};

	/// <summary>
	/// 通常攻撃の情報
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
	/// 攻撃をシーンに追加する際に使用する構造体
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
	/// 管理する攻撃を作成し、追加する(攻撃判定をシーンに追加するときに使用する)
	/// </summary>
	/// <param name="attackInfo">追加する攻撃の情報</param>
	void AddAttack(AddAttackInfo attackInfo);

private:

	/// <summary>
	/// 必殺技のファイルを開いた際の情報の並び順
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
		kAttackName,//ゲーム内では使用しないデータ
		kDamageRate,//ダメージ倍率
		kTotalFrame,//総フレーム
		kAttackFrame,//攻撃発生フレーム
		kHitReaction,//攻撃を受けた際の反応
		kHitDirection,//攻撃を受けた時の吹き飛ぶ方向
		kBurstPower,//吹き飛ぶ強さ
		kAnimationName//アニメーションの名前
	};

	//プレイヤーの必殺技情報
	SpecialAttackInfo m_playerSpecialAttackInfo[kSpecialAttackNum];
	//エネミーの必殺技情報
	SpecialAttackInfo m_enemySpecialAttackInfo[kSpecialAttackNum];
	//各キャラ共通の通常攻撃の情報
	NormalAttackInfo m_normalAttackInfo;

	//場に出ている攻撃の配列
	std::vector<std::shared_ptr<Attack>> m_pAttacks;

};

