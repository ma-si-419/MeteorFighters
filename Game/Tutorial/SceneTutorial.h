#pragma once
#include "SceneBase.h"
#include <vector>

class GameManagerBase;
class Character;
class SceneTutorial : public SceneBase
{
public:

	SceneTutorial(SceneManager& sceneManager);

	virtual ~SceneTutorial();

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

private:

	std::vector<std::shared_ptr<Character>> m_pCharacters;

	std::shared_ptr<GameManagerBase> m_pGameManager;

	bool m_isChangeScene;

};

