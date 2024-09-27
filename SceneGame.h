#pragma once
#include "SceneBase.h"
class SceneGame : public SceneBase
{
public:
	SceneGame(SceneManager& sceneManager);
	virtual ~SceneGame();
	//‰Šú‰»ˆ—
	virtual void Init();
	//XVˆ—
	virtual void Update(MyEngine::Input input);
	//•`‰æˆ—
	virtual void Draw();
	//I—¹ˆ—
	virtual void End();
};

