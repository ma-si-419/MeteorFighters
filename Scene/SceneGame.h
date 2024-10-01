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
	//‰Šú‰»ˆ—
	virtual void Init();
	//XVˆ—
	virtual void Update();
	//•`‰æˆ—
	virtual void Draw();
	//I—¹ˆ—
	virtual void End();
private:
	std::vector<std::shared_ptr<Actor>> m_pActors;
	std::shared_ptr<GameManager> m_pGameManager;
};

