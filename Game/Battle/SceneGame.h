#pragma once
#include "SceneBase.h"
#include <vector>
#include <string>

class GameManagerBase;
class Character;
class BattleUi;
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
	/// <param name="enemyLevel">エネミーのレベル</param>
	void SetCharacter(int player,int enemy,int enemyLevel);

	/// <summary>
	/// バトル中に使用するBGMを設定する
	/// </summary>
	/// <param name="musicName">BGMの名前</param>
	void SetMusicName(std::string musicName);

private:
	std::vector<std::shared_ptr<Character>> m_pCharacters;

	std::shared_ptr<GameManagerBase> m_pGameManager;
};

