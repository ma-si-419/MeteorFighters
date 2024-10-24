#pragma once
#include "Actor.h"
#include "LocalPos.h"
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
	/// 攻撃の種類
	/// </summary>
	enum class AttackHitKind
	{
		kLow,//弱
		kMiddle,//中
		kUpBurst,//上吹っ飛び
		kDownBurst,//下吹っ飛び
		kFarBurst,//奥吹っ飛び
		kBottomStan,//下段スタン
		kMiddleStan,//中段スタン
		kKindNum
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
		kBackMiddleHit
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
	/// 攻撃の種類
	/// </summary>
	enum class AttackKind
	{
		kEnergy,//気弾攻撃
		kPhysical,//格闘攻撃
		kBeam,//レーザー上の攻撃
		kRush,//ぶつかると演出に切り替わる攻撃
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
		float moveSpeed = -1;
		AttackHitKind attackHitKind = AttackHitKind::kLow;
		AttackKind attackKind = AttackKind::kPhysical;
		std::string nextLowComboName = "empty";
		std::string nextHighComboName = "empty";
		std::string animationName = "empty";
	};

	/// <summary>
	/// 必殺技の情報
	/// </summary>
	struct SpecialAttackData
	{
		std::string name = "empty";
		AttackKind kind = AttackKind::kBeam;//必殺技の種類によって吹き飛ばし方などを判別する
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
		float startMp = 0;
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
		AttackKind attackKind = AttackKind::kPhysical;
		AttackHitKind attackHitKind = AttackHitKind::kLow;
	};

public:

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
	/// 体力を減らす
	/// </summary>
	/// <param name="subHp">減少量</param>
	void SubHp(int subHp) { m_nowHp -= subHp; }

	/// <summary>
	/// キャラクターの攻撃力を取得する
	/// </summary>
	/// <returns>キャラクターの攻撃力</returns>
	float GetPower() { return m_status.atk; }

	/// <summary>
	/// 再生するアニメーションを変える(ブレンドスピード固定)
	/// </summary>
	/// <param name="animKind">アニメーションを指定</param>
	/// <param name="loop">繰り返すかどうか</param>
	void ChangeAnim(AnimKind animKind, bool loop);

	/// <summary>
	/// 再生するアニメーションを変える(ブレンドスピード可変)
	/// </summary>
	/// <param name="animKind">アニメーションを指定</param>
	/// <param name="loop">繰り返すかどうか</param>
	/// <param name="blendSpeed">繰り返すかどうか</param>
	void ChangeAnim(AnimKind animKind, bool loop ,float blendSpeed);

	/// <summary>
	/// アニメーションを再生する
	/// </summary>
	void PlayAnim();

	/// <summary>
	/// アニメーションが終了したかを取得する
	/// </summary>
	/// <returns>true:アニメーション終了時　false:アニメーション再生時</returns>
	bool IsAnimEnd();

	/// <summary>
	/// csvから持ってきたstringデータの必殺技の種類をSpecialAttackKindに変換する
	/// </summary>
	/// <param name="kind">必殺技の種類を表すstringのデータ</param>
	/// <returns>SpecialAttackKind型の必殺技の種類</returns>
	AttackKind GetSpecialAttackKind(std::string kind);

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

	/// <summary>
	/// アニメーションブレンドが終了したかどうかを取得する
	/// </summary>
	/// <returns>終了していたらtrue</returns>
	bool IsEndAnimationBlend() { return m_isEndAnimationBlend; }
	
	/// <summary>
	/// 現在再生しているアニメーションを返す
	/// </summary>
	/// <returns>アニメーションの種類</returns>
	AnimKind GetPlayAnimKind() { return m_playAnimKind; }

	/// <summary>
	/// 自身がどちらを向いているかを設定する
	/// </summary>
	/// <param name="frontPos">前方の座標</param>
	void SetFrontPos(MyEngine::Vector3 frontPos);

	/// <summary>
	/// 前方にターゲットがいるかどうかを返す
	/// </summary>
	/// <param name="isPlayer">プレイヤーならtrueを入れる</param>
	/// <returns>前方にいるならtrue</returns>
	bool IsFrontTarget(bool isPlayer);

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
		kMoveSpeed,//移動速度
		kAttackHitKind,//この攻撃を受けた時のやられ状態
		kAttackKind,//攻撃の種類
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
	//現在の体力
	float m_nowHp;
	//現在の気力
	float m_nowMp;
	//すべてのキャラで共通で使う通常攻撃の情報
	std::map<std::string, NormalAttackData> m_normalAttackData;
	//自身の向いている方向などを保存するためにローカル座標を持っておく
	LocalPos m_targetLocalPos;
	//今再生しているアニメ
	int m_attachAnim;
	//今再生しているアニメの種類
	AnimKind m_playAnimKind;
	//アニメーションの合計時間
	float m_totalAnimTime;
	//アニメーションの現在の再生時間
	float m_playAnimTime;
	//アニメーションの再生速度
	float m_animPlaySpeed;
	//アニメーションをループするかどうか
	bool m_isLoop;
	//アニメーションのブレンド率
	float m_animBlendRate;
	//アニメーションのブレンド速度
	float m_animBlendSpeed;
	//1つ前のアニメーション
	int m_lastAnim;
	//アニメーションブレンドが終わったかどうか
	bool m_isEndAnimationBlend;

};