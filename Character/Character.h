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
	/// 攻撃の種類
	/// </summary>
	enum class AttackHitKind
	{
		kLow,//弱
		kMiddle,//中
		kWeakUpBurst,//上に弱吹っ飛び
		kUpBurst,//上吹っ飛び
		kDownBurst,//下吹っ飛び
		kFarBurst,//奥吹っ飛び
		kBottomStan,//下段スタン
		kMiddleStan,//中段スタン
		kKindNum
	};

	/// <summary>
	/// 自分の状態
	/// </summary>
	enum class HitReactionKind
	{
		kNone,//なにもされていない状態
		kLow,//弱
		kMiddle,//中
		kWeakUpBurst,//上に弱吹っ飛び
		kUpBurst,//上吹っ飛び
		kDownBurst,//下吹っ飛び
		kFarBurst,//奥吹っ飛び
		kWeakBurst,//弱吹っ飛び
		kBottomStan,//下段スタン
		kMiddleStan,//中段スタン
		kGuard,//ガード状態
		kGuardBreak,//ガードブレイク状態
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
	/// 攻撃の種類
	/// </summary>
	enum class AttackKind
	{
		kEnergy,//気弾攻撃
		kPhysical,//格闘攻撃
		kLaser,//レーザー上の攻撃
		kRush,//ぶつかると演出に切り替わる攻撃
		kThrow,//ガードできなくぶつかると演出に切り替わる攻撃
		kAssault//ぶつかると演出せずにそのままダメージを受ける攻撃
	};

	/// <summary>
	/// どのキャラクターか
	/// </summary>
	enum class PlayerNumber
	{
		kOnePlayer,
		kTwoPlayer,
		kPlayerNum
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
	/// 必殺技の情報
	/// </summary>
	struct SpecialAttackData
	{
		std::string name = "empty";
		std::string path = "empty";
		AttackKind kind = AttackKind::kLaser;//必殺技の種類によって吹き飛ばし方などを判別する
		int cost = -1;
		float damageRate = -1;
		float radius = -1;
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
		int chaseAttackNum = 0;
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
		std::string attackName = "empty";
		std::string effectName = "empty";
		std::string hitSoundName = "empty";
		AttackKind attackKind = AttackKind::kPhysical;
		AttackHitKind attackHitKind = AttackHitKind::kLow;
	};

	/// <summary>
	/// 残像の情報
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
	/// enemyInputをセットする
	/// </summary>
	/// <param name="level">敵の強さ</param>
	void SetEnemyInput(int level);

	void Update();

	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	void Final();

	/// <summary>
	/// もう一度バトルを行う際に使用する関数
	/// </summary>
	void RetryInit();

	/// <summary>
	/// 残像の更新を行う
	/// </summary>
	void UpdateAfterImage();

	/// <summary>
	/// 敵の座標などを取得するためにゲームマネージャーのポインタをセットする
	/// </summary>
	/// <param name="manager">ゲームマネージャーのポインタ</param>
	void SetGameManager(std::shared_ptr<GameManagerBase> manager);

	/// <summary>
	/// 現在のワールド座標を返す
	/// </summary>
	/// <returns>自身のワールド座標</returns>
	MyEngine::Vector3 GetPos();

	/// <summary>
	/// 現在の移動ベクトルを返す
	/// </summary>
	/// <returns>自身の移動ベクトル</returns>
	MyEngine::Vector3 GetVelo();

	/// <summary>
	/// ステータスのセットする
	/// </summary>
	/// <param name="status">キャラクターのステータス</param>
	void SetStatus(CharacterStatus status) { m_status = status; }

	/// <summary>
	/// 現在の体力を取得する
	/// </summary>
	/// <returns>現在の体力</returns>
	float GetHp() { return m_nowHp; }

	/// <summary>
	/// 現在の気力を取得する
	/// </summary>
	/// <returns>現在の気力</returns>
	float GetMp() { return m_nowMp; }

	/// <summary>
	/// 体力を減らす
	/// </summary>
	/// <param name="subHp">減少量</param>
	void SubHp(int subHp) { m_nowHp -= subHp; }

	/// <summary>
	/// 気力を減らす
	/// </summary>
	/// <param name="subMp">減少量</param>
	/// <returns>足りなければfalse</returns>
	bool SubMp(float subMp);

	/// <summary>
	/// 気力を貯める
	/// </summary>
	void ChargeMp();

	/// <summary>
	/// キャラクターの攻撃力を取得する
	/// </summary>
	/// <returns>キャラクターの攻撃力</returns>
	float GetPower() { return m_status.atk; }

	/// <summary>
	/// キャラクターの移動速度を取得する
	/// </summary>
	/// <returns>キャラクターの移動速度</returns>
	float GetSpeed() { return m_status.spd; }

	/// <summary>
	/// キャラクターの瞬間移動攻撃できる回数を取得
	/// </summary>
	/// <returns>瞬間移動攻撃できる回数</returns>
	int GetChaseNum() { return m_status.chaseAttackNum; }

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
	void ChangeAnim(AnimKind animKind, bool loop, float blendSpeed);

	/// <summary>
	/// アニメーションを再生する
	/// </summary>
	void PlayAnim();

	/// <summary>
	/// アニメーションの再生速度を設定する
	/// </summary>
	/// <param name="speed">再生速度</param>
	void SetAnimPlaySpeed(float speed);

	/// <summary>
	/// アニメーションの再生速度を初期値に戻す
	/// </summary>
	void SetAnimPlaySpeed();

	/// <summary>
	/// アニメーションが終了したかを取得する
	/// </summary>
	/// <returns>true:アニメーション終了時　false:アニメーション再生時</returns>
	bool IsEndAnim() { return m_isEndAnim; }

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
	/// <returns>作成した攻撃のポインタ</returns>
	std::shared_ptr<Attack> CreateAttack(AttackData attackData);

	/// <summary>
	/// 通常攻撃の情報を取得する
	/// </summary>
	/// <param name="attackName">取得したい通常攻撃の名前</param>
	/// <returns>通常攻撃を生成する際の情報</returns>
	NormalAttackData GetNormalAttackData(std::string attackName);

	/// <summary>
	/// 必殺技の情報を取得する
	/// </summary>
	/// <param name="specialAttackName">取得したい必殺技の番号(1か2)</param>
	/// <returns>設定されている必殺技の情報</returns>
	SpecialAttackData GetSpecialAttackData(int specialNumber);

	/// <summary>
	/// アニメーションブレンドが終了したかどうかを取得する
	/// </summary>
	/// <returns>終了していたらtrue</returns>
	bool IsEndAnimationBlend() { return m_isEndAnimationBlend; }

	/// <summary>
	/// アニメーションが終了しているかどうかを取得する
	/// </summary>
	/// <returns>アニメーションが終了していたらtrue</returns>
	bool IsAnimEnd();

	/// <summary>
	/// 現在再生しているアニメーションを取得する
	/// </summary>
	/// <returns>アニメーションの種類</returns>
	AnimKind GetPlayAnimKind() { return m_playAnimKind; }

	/// <summary>
	/// 自身がどちらを向いているかを設定する
	/// </summary>
	/// <param name="frontPos">前方の座標</param>
	void SetFrontPos(MyEngine::Vector3 frontPos);

	/// <summary>
	/// 自身の向いている方向を取得する
	/// </summary>
	/// <returns>自身の前方の座標</returns>
	MyEngine::Vector3 GetFrontPos();

	/// <summary>
	/// 前方にターゲットがいるかどうかを取得
	/// </summary>
	/// <returns>前方にいるならtrue</returns>
	bool IsFrontTarget();

	/// <summary>
	/// 描画する座標を設定する(MV1SetPositionの代わり)
	/// </summary>
	/// <param name="pos">描画したい座標</param>
	void SetDrawPos(MyEngine::Vector3 pos);

	/// <summary>
	/// 攻撃のアニメーションを返す
	/// </summary>
	/// <param name="animName">アニメーションの名前</param>
	/// <returns>アニメーション番号</returns>
	AnimKind GetAttackAnimKind(std::string animName);

	/// <summary>
	/// 現在のやられ状態を取得する
	/// </summary>
	/// <returns>現在のやられ状態</returns>
	HitReactionKind GetHitReaction() { return m_nowHitReaction; }

	/// <summary>
	/// 現在のStateの種類を返す
	/// </summary>
	int GetStateKind();

	/// <summary>
	/// やられ状態を設定する
	/// </summary>
	/// <param name="kind">次のやられ状態</param>
	void SetHitReaction(HitReactionKind kind) {m_nowHitReaction = kind; }

	/// <summary>
	/// キャラクターが地上にいるかどうかを返す
	/// </summary>
	/// <returns>地上にいるのならtrue</returns>
	bool IsGround() { return m_pColData->IsGround(); }

	/// <summary>
	/// キャラクターが壁にいるかどうかを返す
	/// </summary>
	bool IsWall() { return m_pColData->IsWall(); }

	/// <summary>
	/// ターゲットの方向を向くようにする
	/// </summary>
	void LookTarget();

	/// <summary>
	/// 残像を作成する
	/// </summary>
	void CreateAfterImage();

	/// <summary>
	/// 情報を設定して残像を作成する
	/// </summary>
	/// <param name="afterImageInfo">ハンドル以外の残像の情報</param>
	void CreateAfterImage(AfterImage afterImageInfo);

	/// <summary>
	/// カメラを速く動かす時に呼ぶ関数(ダッシュするときなど)
	/// </summary>
	void StartFastCameraMove();

	/// <summary>
	/// 背後の座標を取得する
	/// </summary>
	/// <returns>キャラクターが向いている方向と逆方向の座標</returns>
	/// <param name="distance">ハンドル以外の残像の情報</param>
	MyEngine::Vector3 GetBackPos(float distance);

	/// <summary>
	/// なにかと衝突した時に物理挙動を行うかを設定する
	/// </summary>
	/// <param name="flag">ぶつかるかどうか</param>
	void SetIsTrigger(bool flag);

	/// <summary>
	/// キャラクターの番号を設定
	/// </summary>
	/// <param name="num">1Pか2Pかを設定する</param>
	void SetCharacterNumber(PlayerNumber num) { m_playerNumber = num; }

	/// <summary>
	/// キャラクターの番号を取得する
	/// </summary>
	/// <returns>1Pか2Pかを返す</returns>
	PlayerNumber GetPlayerNumber() { return m_playerNumber; }

	/// <summary>
	/// 状況によって変化させるアップデートを変更する
	/// </summary>
	/// <param name="situation">GameManagerBase::Situationをint型にキャストして引数にする</param>
	void ChangeSituationUpdate(int situation);

	/// <summary>
	/// 入力を取得する
	/// </summary>
	/// <returns>入力情報</returns>
	std::shared_ptr<MyEngine::InputData> GetInputData() { return m_input; }

	/// <summary>
	/// モデルのパスを取得する
	/// </summary>
	/// <returns>モデルのパス</returns>
	std::string GetModelPath() { return m_modelPath; }

	/// <summary>
	/// モデルのハンドルを設定する
	/// </summary>
	/// <param name="handle">モデルのハンドル</param>
	void SetModelHandle(int handle);

	/// <summary>
	/// 一番近いステージのポリゴンの座標を取得する
	/// </summary>
	/// <returns>ステージのポリゴンの座標</returns>
	MyEngine::Vector3 GetNearStagePos();	

	/// <summary>
	/// エネミーのインプットクラスを取得する
	/// </summary>
	/// <returns>エネミーのインプットクラス</returns>
	std::shared_ptr<EnemyInput> GetEnemyInput();


	/// <summary>
	/// 声を出す際に呼ぶ関数
	/// </summary>
	/// <param name="kind">話す声の種類</param>
	void PlayVoice(VoiceKind kind);

	/// <summary>
	/// どのくらい描画座標をずらすかを設定する
	/// </summary>
	/// <param name="vec">ずらすベクトル</param>
	void SetDrawShiftVec(MyEngine::Vector3 vec) { m_drawShiftVec = vec; }

private:

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
		kAttackMoveSpeed,//攻撃の移動速度
		kIsTeleportation,
		kAttackHitKind,//この攻撃を受けた時のやられ状態
		kAttackKind,//攻撃の種類
		kNextComboName,//次に出る攻撃の名前
		kTargetHitReaction,//敵の状態が何だったら攻撃を出せるか
		kAnimationName,//アニメーションの名前
		kAnimationSpeed,//アニメーションの再生速度
		kEffectName,//攻撃のエフェクトの名前
		kSoundName//攻撃の音の名前
	};

private:

	/// <summary>
	/// 開始時の初期化
	/// </summary>
	void InitStart();

	/// <summary>
	/// 開始時の演出中のアップデート
	/// </summary>
	void UpdateStart();

	/// <summary>
	/// バトル時アップデート
	/// </summary>
	void UpdateBattle();

	/// <summary>
	/// ノックアウト時のアップデート
	/// </summary>
	void UpdateKnockOut();

	/// <summary>
	/// リザルト時のアップデート
	/// </summary>
	void UpdateResult();

	/// <summary>
	/// アイドル時のアップデート
	/// </summary>
	void UpdateIdle();

	/// <summary>
	/// 何もしない時のアップデート
	/// </summary>
	void UpdateNone();

private:
	using UpdateSituationFunc = void (Character::*)();

	UpdateSituationFunc m_updateSituationFunc;

	//モデルのパス
	std::string m_modelPath;
	//エネミーの入力を受け付けるクラス(1P側は使用しない)
	std::shared_ptr<EnemyInput> m_pEnemyInput;
	//入力情報
	std::shared_ptr<MyEngine::InputData> m_input;
	//自身がどちら側のキャラクターか
	PlayerNumber m_playerNumber;
	//モデルハンドル
	int m_modelHandle;
	//シーンのポインタ
	std::shared_ptr<SceneGame> m_pScene;
	//マネージャーのポインタ
	std::shared_ptr<GameManagerBase> m_pBattleManager;
	//State
	std::shared_ptr<CharacterStateBase> m_pState;
	//作成した残像
	std::list<AfterImage> m_afterImageList;
	//使うキャラクターの種類
	CharacterKind m_characterKind;
	//自身のステータス
	CharacterStatus m_status;
	//描画座標をどのくらいずらすか(空中で揺れる際などに使用)
	MyEngine::Vector3 m_drawShiftVec;
	//現在の体力
	float m_nowHp;
	//現在の気力
	float m_nowMp;
	//今向いている方向座標
	MyEngine::LocalPos m_lookPos;
	//現在のやられ状態
	HitReactionKind m_nowHitReaction;
	//すべてのキャラで共通で使う通常攻撃の情報
	std::map<std::string, NormalAttackData> m_normalAttackData;
	//自身の向いている方向などを保存するためにローカル座標を持っておく
	MyEngine::LocalPos m_targetLocalPos;
	//今再生しているアニメ
	int m_attachAnim;
	//今再生しているアニメの種類
	AnimKind m_playAnimKind;
	//アニメーションが終わったかどうか
	bool m_isEndAnim;
	//アニメーションの合計時間
	float m_totalAnimTime;
	//アニメーションの現在の再生時間
	float m_nowPlayAnimTime;
	//前のアニメーション
	float m_lastPlayAnimTime;
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
	//ノックアウト時のベロシティ
	MyEngine::Vector3 m_knockOutVelo;
	//キャラクターを描画するかどうか
	bool m_isDrawCharacter;
	//話している声のプレイハンドル
	int m_voiceHandle;

	friend CharacterStateBase;
};