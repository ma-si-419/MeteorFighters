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
	/// 次のフレームで移行するシーンを取得する
	/// </summary>
	/// <returns>次のシーン(Game::Sceneキャストして使用)</returns>
	int GetNextScene() { return m_nextScene; };
private:

	void SelectOnePlayer();

	void SelectTwoPlayer();

	void ConfirmCharacter();

private:

	using UpdateSelectFunc = void (SelectManager::*)();

	UpdateSelectFunc m_updateSelectFunc;

	std::shared_ptr<SelectUi> m_pUi;

	int m_selectNumber;

	int m_playerNumber;

	int m_enemyNumber;

	int m_nextScene;
};

