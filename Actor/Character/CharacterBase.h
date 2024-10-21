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
	/// 攻撃を受けた時の反応
	/// </summary>
	enum class HitReaction
	{
		kLow,//弱
		kMid,//中
		kHigh,//強
		kBurst,//吹っ飛び
		kBottomStan,//下段スタン
		kMiddleStan,//中段スタン
		kTopStan//上段スタン
	};

	/// <summary>
	/// 吹き飛ばす力
	/// </summary>
	enum class BurstPower
	{
		kNone,
		kLow,
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
	/// 必殺技の種類
	/// </summary>
	enum class SpecialAttackKind
	{
		kBeam,//レーザー上の攻撃
		kRush,//ぶつかると演出に切り替わる攻撃
		kEnergy,//気弾攻撃
		kThrow,//ガードできなくぶつかると演出に切り替わる攻撃
		kAssault//ぶつかると演出せずにそのままダメージを受ける攻撃
	};

	/// <summary>
	/// 通常攻撃の情報
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
	/// 必殺技の情報
	/// </summary>
	struct SpecialAttackData
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
	/// キャラクターのステータス
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
	/// 攻撃を作成するときに必要な変数をまとめた構造体
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
	/// 敵の座標などを取得するためにゲームマネージャーのポインタをセットする
	/// </summary>
	/// <param name="manager">ゲームマネージャーのポインタ</param>
	void SetGameManager(std::shared_ptr<GameManager> manager);

	/// <summary>
	/// 現在のワールド座標を返す
	/// </summary>
	/// <returns>自身のワールド座標</returns>
	MyEngine::Vector3 GetPos();

	/// <summary>
	/// ステータスのセットする
	/// </summary>
	/// <param name="status">キャラクターのステータス</param>
	void SetStatus(CharacterStatus status) { m_status = status; }

	/// <summary>
	/// キャラクターの攻撃力を取得する
	/// </summary>
	/// <returns>キャラクターの攻撃力</returns>
	float GetPower() { return m_status.atk; }

	/// <summary>
	/// 再生するアニメーションを変える
	/// </summary>
	/// <param name="animKind">アニメーションを指定</param>
	/// <param name="loop">繰り返すかどうか</param>
	void ChangeAnim(AnimKind animKind, bool loop);

	/// <summary>
	/// アニメーションを再生する
	/// </summary>
	void PlayAnim();

	/// <summary>
	/// アニメーションが終了したかを取得する
	/// </summary>
	/// <returns>true:アニメーション終了時　false:アニメーション再生時</returns>
	bool IsGetAnimEnd();

	/// <summary>
	/// csvから持ってきたstringデータの必殺技の種類をSpecialAttackKindに変換する
	/// </summary>
	/// <param name="kind">必殺技の種類を表すstringのデータ</param>
	/// <returns>SpecialAttackKind型の必殺技の種類</returns>
	SpecialAttackKind GetSpecialAttackKind(std::string kind);

	/// <summary>
	/// 各キャラ共通で使用する通常攻撃を設定する
	/// </summary>
	/// <param name="normalAttackData">csvファイルから持ってきた通常攻撃の情報</param>
	void SetNormalAttackData(std::vector<std::vector<std::string>> normalAttackData);

	/// <summary>
	/// 攻撃を作成してシーン上に出す
	/// </summary>
	/// <param name="attackData">攻撃の情報</param>
	void CreateAttack(AttackData attackData);

	/// <summary>
	/// 通常攻撃の情報を取得する
	/// </summary>
	/// <param name="attackName">取得したい通常攻撃の名前</param>
	/// <returns>通常攻撃を生成する際の情報</returns>
	NormalAttackData GetNormalAttackData(std::string attackName);

protected:

	/// <summary>
	/// 各キャラ共通の攻撃の情報のcsvファイルに入っている順番
	/// </summary>
	enum class NormalAttackDataSort
	{
		kAttackName,//攻撃の名前
		kDamageRate,//ダメージ倍率
		kTotalFrame,//総フレーム
		kAttackFrame,//攻撃発生フレーム
		kCancelFrame,//次の攻撃に移行できるようになるフレーム
		kHitReaction,//攻撃を受けた際の反応
		kHitDirection,//攻撃を受けた時の吹き飛ぶ方向
		kBurstPower,//吹き飛ぶ強さ
		kLowComboName,//Xボタンを押したときに次に出る攻撃の名前
		kHighComboName,//Yボタンを押したときに次に出る攻撃の名前
		kAnimationName//アニメーションの名前
	};


	//モデルハンドル
	int m_modelHandle;
	//シーンのポインタ
	std::shared_ptr<SceneGame> m_pScene;
	//マネージャーのポインタ
	std::shared_ptr<GameManager> m_pGameManager;
	//State
	std::shared_ptr<CharacterStateBase> m_pState;
	//使うキャラクターの種類
	CharacterKind m_characterKind;
	//自身のステータス
	CharacterStatus m_status;
	//すべてのキャラで共通で使う通常攻撃の情報
	std::map<std::string,NormalAttackData> m_normalAttackData;

	//アニメーションの情報
	int m_attachAnim;
	float m_totalAnimTime;
	float m_playAnimTime;
	float m_animPlaySpeed;
	bool m_isLoop;
	float m_animBlendRate;
	int m_lastAnim;

};