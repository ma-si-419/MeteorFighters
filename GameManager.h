#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Vector3.h"

class Player;
class Enemy;
class AttackManager;
class GameManager : public std::enable_shared_from_this<GameManager>
{
public:

	GameManager();
	~GameManager();


	/// <summary>
	/// ゲームマネージャーの更新
	/// </summary>
	void Update();

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
	/// プレイヤーのポインタを返す
	/// </summary>
	/// <returns>プレイヤーのポインタ</returns>
	std::shared_ptr<Player> GetPlayerPointer() { return m_pPlayer; }

	/// <summary>
	/// エネミーのポインタを返す
	/// </summary>
	/// <returns>エネミーのポインタ</returns>
	std::shared_ptr<Enemy> GetEnemyPointer() { return m_pEnemy; }

	/// <summary>
	/// プレイヤーの座標を取得する
	/// </summary>
	/// <returns>プレイヤーの座標</returns>
	MyEngine::Vector3 GetPlayerPos();

	/// <summary>
	/// エネミーの座標を取得する
	/// </summary>
	/// <returns>エネミーの座標</returns>
	MyEngine::Vector3 GetEnemyPos();

	/// <summary>
	/// 攻撃を作成し、追加する
	/// </summary>
	/// <param name="attackNumber">攻撃の番号</param>
	/// <param name="isSpecial">必殺技かどうか</param>
	/// <param name="isPlayer">プレイヤーかどうか</param>
	void AddAttack(int attackNumber, bool isSpecial, bool isPlayer);

private:
	//プレイヤーのポインタ
	std::shared_ptr<Player> m_pPlayer;
	//エネミーのポインタ
	std::shared_ptr<Enemy> m_pEnemy;
	//攻撃の情報を持っているマネージャー
	std::shared_ptr<AttackManager> m_pAttackManager;
};