#pragma once
#include <memory>

class SelectUi;
class SelectManager
{
public:

	enum class CharacterNumber
	{
		kMouse,
		kRandom,
		kBlueHead,
		kCharacterNum
	};

public:
	SelectManager();

	virtual ~SelectManager();

	void Init();

	void Update();

	void Draw();

	/// <summary>
	/// 1プレイヤー側のキャラクターの番号を返す
	/// </summary>
	/// <returns>1プレイヤーのキャラクターの番号を返す</returns>
	int GetPlayerNumber();

	/// <summary>
	/// 2プレイヤー側のキャラクターの番号を返す
	/// </summary>
	/// <returns>2プレイヤーのキャラクターの番号を返す</returns>
	int GetEnemyNumber();

	/// <summary>
	/// エネミーのレベルを返す
	/// </summary>
	/// <returns>エネミーのレベル</returns>
	int GetEnemyLevel() { return m_enemyLevel; }

	/// <summary>
	/// 次のフレームで移行するシーンを取得する
	/// </summary>
	/// <returns>次のシーン(Game::Sceneキャストして使用)</returns>
	int GetNextScene() { return m_nextScene; };

	/// <summary>
	/// BGMの番号を取得する
	/// </summary>
	/// <returns>BGMの番号</returns>
	int GetBgmNumber() { return m_bgmNumber; }

private:

	void SelectOnePlayer();

	void SelectTwoPlayer();

private:

	using UpdateSelectFunc = void (SelectManager::*)();

	//更新関数
	UpdateSelectFunc m_updateSelectFunc;

	//UIのポインタ
	std::shared_ptr<SelectUi> m_pUi;

	//選択中の番号
	int m_selectNumber;

	//プレイヤーの番号
	int m_playerNumber;

	//エネミーの番号
	int m_enemyNumber;

	//次のシーン
	int m_nextScene;

	//エネミーのAIレベル
	int m_enemyLevel;

	//今再生しているBGMの番号
	int m_bgmNumber;
};

