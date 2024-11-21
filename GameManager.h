#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Vector3.h"
#include "CharacterBase.h"

class Player;
class Enemy;
class GameCamera;
class Attack;
class Stage;
class EffectManager;
class Effect;
class GameUi;
class GameManager : public std::enable_shared_from_this<GameManager>
{
public:

	GameManager(std::shared_ptr<GameCamera> camera);
	~GameManager();

	/// <summary>
	/// ゲームマネージャーの初期化
	/// </summary>
	void Init();

	/// <summary>
	/// ゲームマネージャーの更新
	/// </summary>
	void Update();

	/// <summary>
	/// 攻撃の描画などを行う
	/// </summary>
	void Draw();

	/// <summary>
	/// プレイヤーが操作するキャラクターを設定する
	/// </summary>
	/// <param name="number">プレイヤーが使うキャラクターの番号</param>
	void SetPlayerStatus(int number, std::vector<std::string> statusData);

	/// <summary>
	/// エネミーが操作するキャラクターを設定する
	/// </summary>
	/// <param name="number">エネミーが使うキャラクターの番号</param>
	void SetEnemyStatus(int number, std::vector<std::string> statusData);

	/// <summary>
	/// 1Pのポインタを返す
	/// </summary>
	/// <returns>1Pのポインタ</returns>
	std::shared_ptr<CharacterBase> GetOnePlayerPointer() { return m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]; }

	/// <summary>
	/// 2Pのポインタを返す
	/// </summary>
	/// <returns>2Pのポインタ</returns>
	std::shared_ptr<CharacterBase> GetTwoPlayerPointer() { return m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]; }

	/// <summary>
	/// 対戦相手の座標を取得する
	/// </summary>
	/// <param name="character">この関数を呼んだキャラクターのポインタ</param>
	/// <returns>対戦相手の座標</returns>
	MyEngine::Vector3 GetTargetPos(std::shared_ptr<CharacterBase> character);

	/// <summary>
	/// 対戦相手の移動ベクトルを取得する
	/// </summary>
	/// <param name="character">この関数を呼んだキャラクターのポインタ</param>
	/// <returns>対戦相手の座標</returns>
	MyEngine::Vector3 GetTargetVelo(std::shared_ptr<CharacterBase> character);

	/// <summary>
	/// 対戦相手の今のやられ状態を取得する
	/// </summary>
	/// <param name="character">この関数を呼んだキャラクターのポインタ</param>
	/// <returns>対戦相手のやられ状態</returns>
	CharacterBase::HitReactionKind GetTargetHitReaction(std::shared_ptr<CharacterBase> character);

	/// <summary>
	/// 対戦相手の背後座標を取得する
	/// </summary>
	/// <param name="distance">背後の距離</param>
	/// <param name="character">自身のポインタ</param>
	/// <returns>対戦相手の背後座標</returns>
	MyEngine::Vector3 GetTargetBackPos(float distance, std::shared_ptr<CharacterBase> character);

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
	/// カメラの移動を止める
	/// </summary>
	void StopMoveCamera();

	/// <summary>
	/// カメラの移動を始める
	/// </summary>
	void StartMoveCamera();


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
	/// 画像のハンドルを返す
	/// </summary>
	/// <param name="graphName">画像の名前</param>
	/// <returns>画像のハンドル</returns>
	int GetGraphHandle(std::string graphName);
private:

	//キャラクターのポインタ
	std::shared_ptr<CharacterBase> m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kPlayerNum)];
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
};