#pragma once
#include "SceneBase.h"
#include <vector>

class GameManagerBase;
class Character;
class SceneGame : public SceneBase
{
public:
	SceneGame(SceneManager& sceneManager);
	virtual ~SceneGame();
	//初期化処理
	virtual void Init() override;
	//更新処理
	virtual void Update() override;
	//非同期処理が行われているときのアップデート
	virtual void UpdateAsyncLoad() override;
	//描画処理
	virtual void Draw() override;
	//終了処理
	virtual void End() override;

	/// <summary>
	/// 使うキャラクターをセットする(SceneSelectからSceneGameに移行する前に必ず呼ぶ)
	/// </summary>
	/// <param name="player">プレイヤーが使うキャラクター番号</param>
	/// <param name="enemy">エネミーが使用するキャラクター番号</param>
	void SetCharacter(int player,int enemy);

private:
	std::vector<std::shared_ptr<Character>> m_pCharacters;

	std::shared_ptr<GameManagerBase> m_pGameManager;
};

