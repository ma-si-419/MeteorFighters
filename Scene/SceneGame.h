#pragma once
#include "SceneBase.h"
#include <vector>
class GameManager;
class Actor;
class SceneGame : public SceneBase
{
public:
	SceneGame(SceneManager& sceneManager);
	virtual ~SceneGame();
	//初期化処理
	virtual void Init();
	//更新処理
	virtual void Update();
	//描画処理
	virtual void Draw();
	//終了処理
	virtual void End();

	/// <summary>
	/// 使うキャラクターをセットする(SceneSelectからSceneGameに移行する前に必ず呼ぶ)
	/// </summary>
	/// <param name="player">プレイヤーが使うキャラクター番号</param>
	/// <param name="enemy">エネミーが使用するキャラクター番号</param>
	void SetCharacter(int player,int enemy);

private:
	std::vector<std::shared_ptr<Actor>> m_pActors;
	std::shared_ptr<GameManager> m_pGameManager;

};

