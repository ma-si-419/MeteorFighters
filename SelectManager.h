#pragma once
class SelectManager
{
public:
	SelectManager();

	virtual ~SelectManager();

	void Update();

	/// <summary>
	/// 1プレイヤー側のキャラクターの番号を返す
	/// </summary>
	/// <returns>1プレイヤーのキャラクターの番号を返す</returns>
	int GetPlayerNumber() { return m_playerNumber; }

	/// <summary>
	/// 2プレイヤー側のキャラクターの番号を返す
	/// </summary>
	/// <returns>2プレイヤーのキャラクターの番号を返す</returns>
	int GetEnemyNumber() { return m_enemyNumber; }

	/// <summary>
	/// 次のフレームで移行するシーンを取得する
	/// </summary>
	/// <returns>次のシーン(Game::Sceneキャストして使用)</returns>
	int GetNextScene() {return m_nextScene};
private:

	void SelectOnePlayer();

	void SelectTwoPlayer();

	void ConfirmCharacter();

private:

	using UpdateSelectFunc = void (SelectManager::*)();

	UpdateSelectFunc m_updateSelectFunc;

	int m_playerNumber;

	int m_enemyNumber;

	int m_nextScene;
};

