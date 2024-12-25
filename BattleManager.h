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
class BattleManager : public std::enable_shared_from_this<BattleManager>
{
public:

	enum class Situation
	{
		kRetry,
		kStart1P,
		kStart2P,
		kBattle,
		kKnockOut,
		kResult
	};

public:

	BattleManager(std::shared_ptr<GameCamera> camera);
	~BattleManager();

	/// <summary>
	/// ゲームマネージャーの初期化
	/// </summary>
	void Init();

	/// <summary>
	/// ゲームをもう一度行う際の初期化を行う
	/// </summary>
	void RetryInit();

	/// <summary>
	/// ゲームマネージャーの更新
	/// </summary>
	void Update();

	/// <summary>
	/// 攻撃の描画などを行う
	/// </summary>
	void Draw();

	/// <summary>
	/// シーンから抜けるときなどに使用
	/// </summary>
	void Final();

	/// <summary>
	/// プレイヤーが操作するキャラクターを設定する
	/// </summary>
	/// <param name="number">プレイヤーが使うキャラクターの番号</param>
	void SetOnePlayerStatus(int number, std::vector<std::string> statusData);

	/// <summary>
	/// エネミーが操作するキャラクターを設定する
	/// </summary>
	/// <param name="number">エネミーが使うキャラクターの番号</param>
	void SetTwoPlayerStatus(int number, std::vector<std::string> statusData);

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
	/// カメラを揺れを設定する
	/// </summary>
	/// <param name="time">揺らす時間</param>
	void ShakeCamera(int time);

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
	/// 今の状況を取得する
	/// </summary>
	/// <returns>現在ゲームシーンで何が行われているかを返す</returns>
	Situation GetNowSituation() { return m_situation; }

	/// <summary>
	/// セレクトシーンに戻るときにtrueを返す
	/// </summary>
	/// <returns>セレクトシーンに戻る時にtrueになる</returns>
	Game::Scene GetNextScene() { return m_nextScene; }

private:

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
	/// リトライ時のアップデート
	/// </summary>
	void UpdateRetry();
	
	/// <summary>
	/// 状況を変化させる
	/// </summary>
	/// <param name="situation">変更後の状況</param>
	void ChangeSituation(Situation situation);

	/// <summary>
	/// 文字列のステータスをCharacterStatusに変換する
	/// </summary>
	/// <param name="statusData">ステータスの情報を持った文字列</param>
	/// <returns>CharacterStatus型のステータス情報</returns>
	Character::CharacterStatus GetCharacterStatus(std::vector<std::string> statusData);

private:

	using UpdateSituationFunc = void (BattleManager::*)();

	UpdateSituationFunc m_updateSituationFunc;

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
	//UI管理クラス
	std::shared_ptr<GameUi> m_pGameUi;
	//ゲームの状況
	Situation m_situation;
	//時間を計る際に使用する変数
	int m_time;
	//演出時のカメラ座標
	MyEngine::Vector3 m_poseCameraPos;
	//フェードを行うときの変数
	int m_alpha;
	//シーンをセレクトシーンに戻す時にtrueにする
	Game::Scene m_nextScene;
};