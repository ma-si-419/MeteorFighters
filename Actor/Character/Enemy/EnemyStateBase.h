#pragma once
#include "CharacterStateBase.h"
#include "Vector3.h"
class Enemy;
class Attack;
class EnemyStateBase : public CharacterStateBase
{

public:
	EnemyStateBase(std::shared_ptr<Enemy> enemy);

protected:

	/// <summary>
	/// エネミーの移動ベクトルを変更する
	/// </summary>
	/// <param name="moveVec">移動ベクトル</param>
	void SetEnemyVelo(MyEngine::Vector3 moveVec);

	/// <summary>
	/// 攻撃を受けた時に呼ぶ関数
	/// </summary>
	/// <param name="attack">受けた攻撃のクラス</param>
	/// <param name="stateKind">現在のState</param>
	void HitAttack(std::shared_ptr<Attack> attack,CharacterStateBase::CharacterStateKind stateKind);

	//そのStateに入って何フレーム立ったか
	int m_time = 0;
	
	/// <summary>
	/// プレイヤーの座標を取得する
	/// </summary>
	/// <returns>プレイヤーの座標</returns>
	MyEngine::Vector3 GetPlayerPos();

	//エネミーのポインタ
	std::shared_ptr<Enemy> m_pEnemy;
	
};

