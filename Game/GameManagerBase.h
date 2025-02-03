#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Vector3.h"
#include "Character.h"
#include "Game.h"

class Player;
class Enemy;
class GameCamera;
class Attack;
class Stage;
class EffectManager;
class Effect;
class GameUi;
class ObjectBase;
class GameManagerBase : public std::enable_shared_from_this<GameManagerBase>
{
public:

	enum class BattleSituation
	{
		kRetry,
		kStart1P,
		kStart2P,
		kBattle,
		kButtonBashing,
		kKnockOut,
		kResult,
		kMenu,
		kIdle
	};

	enum class GameKind
	{
		kBattle,
		kTutorial
	};

	enum class ButtonBashingSituation
	{
		kFirstHit,
		kSecondHit,
		kFighting,
		kSituationNum
	};

public:

	GameManagerBase(std::shared_ptr<GameCamera> camera, GameKind kind);
	~GameManagerBase();

	/// <summary>
	/// ゲームマネージャーの初期化
	/// </summary>
	virtual void Init() abstract;

	/// <summary>
	/// ゲームをもう一度行う際の初期化を行う
	/// </summary>
	void RetryInit();

	/// <summary>
	/// ゲームマネージャーの更新
	/// </summary>
	virtual void Update()abstract;

	/// <summary>
	/// 非同期ロード中のアップデート
	/// </summary>
	virtual void UpdateAsyncLoad();

	/// <summary>
	/// 攻撃の描画などを行う
	/// </summary>
	virtual void Draw() abstract;

	/// <summary>
	/// シーンから抜けるときなどに使用
	/// </summary>
	virtual void Final() abstract;

	/// <summary>
	/// プレイヤーが操作するキャラクターを設定する
	/// </summary>
	/// <param name="number">プレイヤーが使うキャラクターの番号</param>
	void SetOnePlayerStatus(int number, std::vector<std::string> statusData);

	/// <summary>
	/// エネミーが操作するキャラクターを設定する
	/// </summary>
	/// <param name="number">エネミーが使うキャラクターの番号</param>
	/// <param name="enemyLevel">Aiの強さ</param>
	void SetTwoPlayerStatus(int number, std::vector<std::string> statusData,int enemyLevel);

	/// <summary>
	/// 1Pのポインタを返す
	/// </summary>
	/// <returns>1Pのポインタ</returns>
	std::shared_ptr<Character> GetOnePlayerPointer() { return m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]; }

	/// <summary>
	/// 2Pのポインタを返す
	/// </summary>
	/// <returns>2Pのポインタ</returns>
	std::shared_ptr<Character> GetTwoPlayerPointer() { return m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]; }

	/// <summary>
	/// エフェクトマネージャーのポインタを返す
	/// </summary>
	/// <returns>エフェクト管理クラスのポインタ</returns>
	std::shared_ptr<EffectManager> GetEffectManagerPointer() { return m_pEffectManager; }

	/// <summary>
	/// 対戦相手の座標を取得する
	/// </summary>
	/// <param name="character">この関数を呼んだキャラクターのポインタ</param>
	/// <returns>対戦相手の座標</returns>
	MyEngine::Vector3 GetTargetPos(std::shared_ptr<Character> character);

	/// <summary>
	/// 対戦相手の移動ベクトルを取得する
	/// </summary>
	/// <param name="character">この関数を呼んだキャラクターのポインタ</param>
	/// <returns>対戦相手の座標</returns>
	MyEngine::Vector3 GetTargetVelo(std::shared_ptr<Character> character);

	/// <summary>
	/// 対戦相手の今のやられ状態を取得する
	/// </summary>
	/// <param name="character">この関数を呼んだキャラクターのポインタ</param>
	/// <returns>対戦相手のやられ状態</returns>
	Character::HitReactionKind GetTargetHitReaction(std::shared_ptr<Character> character);

	/// <summary>
	/// 対戦相手の今のStateを取得する
	/// </summary>
	/// <param name="character">この関数を呼んだキャラクターのポインタ</param>
	/// <returns>対戦相手のState</returns>
	int GetTargetState(std::shared_ptr<Character> character);

	/// <summary>
	/// 対戦相手の背後座標を取得する
	/// </summary>
	/// <param name="distance">背後の距離</param>
	/// <param name="character">自身のポインタ</param>
	/// <returns>対戦相手の背後座標</returns>
	MyEngine::Vector3 GetTargetBackPos(float distance, std::shared_ptr<Character> character);

	/// <summary>
	/// 管理する攻撃を追加する
	/// </summary>
	/// <param name="attack">追加する攻撃クラスのポインタ</param>
	void AddAttack(std::shared_ptr<Attack> attack);

	/// <summary>
	/// カメラを高速移動させる
	/// </summary>
	void StartFastCameraMove();

	/// <summary>
	/// カメラの補正を止める
	/// </summary>
	void StopCameraCorrection();

	/// <summary>
	/// カメラの動きを止める
	/// </summary>
	/// <param name="time">止める時間</param>
	void StopCamera(int time);

	/// <summary>
	/// カメラの補正を始める
	/// </summary>
	void StartCameraCorrection();

	/// <summary>
	/// カメラに揺れを設定する(揺れの大きさ固定)
	/// </summary>
	/// <param name="time">揺らす時間</param>
	void ShakeCamera(int time);

	/// <summary>
	/// カメラに揺れを設定する(揺れの大きさ可変)
	/// </summary>
	/// <param name="time">揺らす時間</param>
	/// <param name="power">揺らす大きさ</param>
	void ShakeCamera(int time, int power);

	/// <summary>
	/// カメラを緩やかに揺らす
	/// </summary>
	void SwayCamera();

	/// <summary>
	/// エフェクトを登録する
	/// </summary>
	/// <param name="effect">登録したいエフェクトのポインタ</param>
	void EntryEffect(std::shared_ptr<Effect> effect);

	/// <summary>
	/// エフェクトを削除する
	/// </summary>
	/// <param name="effect">削除したいエフェクト</param>
	void ExitEffect(std::shared_ptr<Effect> effect);

	/// <summary>
	/// セレクトシーンに戻るときにtrueを返す
	/// </summary>
	/// <returns>セレクトシーンに戻る時にtrueになる</returns>
	Game::Scene GetNextScene() { return m_nextScene; }

	/// <summary>
	/// 今の状況を取得する
	/// </summary>
	/// <returns>現在ゲームシーンで何が行われているかを返す</returns>
	BattleSituation GetNowSituation() { return m_situation; }

	/// <summary>
	/// ステージのモデルのパスを返す
	/// </summary>
	/// <returns>ステージのパス</returns>
	std::string GetStagePath();

	/// <summary>
	/// スカイドームのパスを返す
	/// </summary>
	/// <returns>スカイドームのパス</returns>
	std::string GetSkyDomePath();

	/// <summary>
	/// ボタン連打状態かどうかを返す
	/// </summary>
	/// <returns>ボタン連打状態ならtrue</returns>
	bool IsButtonBashing() { return m_isButtonBashing; }

	/// <summary>
	/// ボタン連打で勝った方を返す
	/// </summary>
	/// <returns>ボタン連打で買った方を返す</returns>
	Character::PlayerNumber GetButtonBashWinner();

	/// <summary>
	/// ボタンを連打した回数を増やす
	/// </summary>
	/// <param name="player">どちらのプレイヤーか</param>
	void BashButton(Character::PlayerNumber player) { m_buttonBashNum[static_cast<int>(player)]++; };

	/// <summary>
	/// ボタン連打を始めるタイミングで呼ぶ
	/// </summary>
	void StartButtonBashing();

	/// <summary>
	/// ボタン連打の状況を次に進める
	/// </summary>
	void SetBashingSituation(ButtonBashingSituation situation) { m_buttonBashingSituation = situation; }

	/// <summary>
	/// ボタン連打の状況を取得する
	/// </summary>
	ButtonBashingSituation GetBashingSituation() { return m_buttonBashingSituation; }

	/// <summary>
	/// 何のボタンを連打するのかを取得する
	/// </summary>
	/// <returns>連打するボタン</returns>
	std::string GetBashingButton() { return m_bashingButton; }

	/// <summary>
	/// 非同期ロードを行うモデルを追加する
	/// </summary>
	/// <param name="name">非同期ロードを行うモデルの名前を入れる</param>
	void AddLoadModel(std::string name) { m_modelHandles[name] = -1; }

	/// <summary>
	/// 現在のゲームモードを返す
	/// </summary>
	GameKind GetGameKind() { return m_gameKind; }

	/// <summary>
	/// オブジェクトを登録する
	/// </summary>
	/// <param name="">オブジェクトのポインタ</param>
	void EntryObject(std::shared_ptr<ObjectBase> object);
	
	/// <summary>
	/// 石のモデルハンドルを取得する
	/// </summary>
	///	<param name="kind">石の種類</param>
	/// <returns>石のモデルハンドル</returns>
	int GetRockModelHandle(int kind) { return m_rockModelHandle[kind]; }

protected:

	/// <summary>
	/// ゲームシーンの共通の更新
	/// </summary>
	void UpdateCommon();

	/// <summary>
	/// ゲームシーン共通の描画処理
	/// </summary>
	void DrawCommon();

	/// <summary>
	/// ボタン連打時の更新を行う
	/// </summary>
	void UpdateButtonBashing();

	/// <summary>
	/// ボタン連打時の描画を行う
	/// </summary>
	void DrawButtonBashing();

	/// <summary>
	/// 文字列のステータスをCharacterStatusに変換する
	/// </summary>
	/// <param name="statusData">ステータスの情報を持った文字列</param>
	/// <returns>CharacterStatus型のステータス情報</returns>
	Character::CharacterStatus GetCharacterStatus(std::vector<std::string> statusData);

protected:

	//キャラクターのポインタ
	std::shared_ptr<Character> m_pCharacters[static_cast<int>(Character::PlayerNumber::kPlayerNum)];
	//攻撃の情報を持っているマネージャー
	std::vector<std::shared_ptr<Attack>> m_pAttacks;
	//ステージクラス
	std::shared_ptr<Stage> m_pStage;
	//カメラクラス
	std::shared_ptr<GameCamera> m_pCamera;
	//エフェクト管理クラス
	std::shared_ptr<EffectManager> m_pEffectManager;
	//ゲームシーンのUI管理クラス
	std::shared_ptr<GameUi> m_pGameUi;
	//表示している触れないオブジェクトのポインタ
	std::vector<std::shared_ptr<ObjectBase>> m_pObjects;
	//ゲームの状況
	BattleSituation m_situation;
	//何を行っているのか
	GameKind m_gameKind;
	//石のモデルハンドル(マジックナンバーどうにかする)
	int m_rockModelHandle[3];
	//時間を計る際に使用する変数
	int m_time;
	//演出時のカメラ座標
	MyEngine::Vector3 m_poseCameraPos;
	//フェードを行うときの変数
	int m_alpha;
	//連打を行うボタン
	std::string m_bashingButton;
	//ボタン連打を行っているかどうか
	bool m_isButtonBashing;
	//ボタン連打を行った回数
	int m_buttonBashNum[2];
	//ボタン連打を行って何フレーム立ったかを保存する
	int m_buttonBashingTime;
	//ボタン連打を行っているときのカメラ座標
	MyEngine::Vector3 m_buttonBashingCameraPos;
	//ボタン連打を行っているときのカメラの回転度
	float m_buttonBashingCameraRota;
	//ボタン連打の今の状態
	ButtonBashingSituation m_buttonBashingSituation;
	//シーンをセレクトシーンに戻す時にtrueにする
	Game::Scene m_nextScene;
	//非同期処理を行うモデルの配列
	std::map<std::string, int> m_modelHandles;
	//HPバーを表示するかどうか
	bool m_isDrawHpBar;
	//ボタン連打のエフェクト
	std::shared_ptr<Effect> m_pButtonBashingEffect;
};