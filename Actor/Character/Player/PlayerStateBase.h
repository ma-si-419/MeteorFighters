#pragma once
#include "CharacterStateBase.h"
#include "Vector3.h"

class Player;
class PlayerStateBase : public CharacterStateBase
{
public:
	PlayerStateBase(std::shared_ptr<Player> player);
protected:

	/// <summary>
	/// 敵の座標を取得する
	/// </summary>
	/// <returns></returns>
	MyEngine::Vector3 GetEnemyPos();
	
	/// <summary>
	/// 敵の移動ベクトルを取得する
	/// </summary>
	/// <returns></returns>
	MyEngine::Vector3 GetEnemyVelo();

	/// <summary>
	/// エネミーのやられ状態を取得する
	/// </summary>
	/// <returns>エネミーのやられ状態</returns>
	int GetEnemyHitReaction();

	/// <summary>
	/// プレイヤーの移動ベクトルを設定する
	/// </summary>
	/// <param name="velo">移動ベクトル</param>
	void SetPlayerVelo(MyEngine::Vector3 velo);

	/// <summary>
	/// プレイヤーの座標を設定する
	/// </summary>
	/// <param name="pos">ワールド座標</param>
	void SetPlayerPos(MyEngine::Vector3 pos);

	/// <summary>
	/// エネミーの背後座標を取得する
	/// </summary>
	/// <param name="distance">距離</param>
	/// <returns>エネミーの背後座標</returns>
	MyEngine::Vector3 GetEnemyBackPos(float distance);

	/// <summary>
	/// プレイヤーの残像を作成する
	/// </summary>
	void CreateAfterImage();

	/// <summary>
	/// アニメーション番号を取得する
	/// </summary>
	/// <param name="animName">アニメーションの名前</param>
	/// <returns>アニメーション番号</returns>
	int GetAttackAnimKind(std::string animName);

	/// <summary>
	/// キャラクターの移動速度を取得する
	/// </summary>
	/// <returns>使用しているキャラクターの移動速度</returns>
	float GetSpeed();

	/// <summary>
	/// カメラの移動を止める
	/// </summary>
	void StopMoveCamera();

	/// <summary>
	/// カメラの移動を始める
	/// </summary>
	void StartMoveCamera();

	//プレイヤーのポインタ
	std::shared_ptr<Player> m_pPlayer;

};

