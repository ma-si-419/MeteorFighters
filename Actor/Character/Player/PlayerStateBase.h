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
	/// プレイヤーの移動ベクトルを設定する
	/// </summary>
	/// <param name="velo">移動ベクトル</param>
	void SetPlayerVelo(MyEngine::Vector3 velo);

	/// <summary>
	/// プレイヤーの座標を設定する
	/// </summary>
	/// <param name="pos">ワールド座標</param>
	void SetPlayerPos(MyEngine::Vector3 pos);

	void ChangeState(std::shared_ptr<CharacterStateBase> nextState) override;
	
	//プレイヤーのポインタ
	std::shared_ptr<Player> m_pPlayer;

};

