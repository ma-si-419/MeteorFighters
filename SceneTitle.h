#pragma once
#include "SceneBase.h"
class SceneTitle : public SceneBase
{
public:
	SceneTitle(SceneManager& sceneManager);
	virtual ~SceneTitle();
	//‰Šú‰»ˆ—
	virtual void Init();
	//XVˆ—
	virtual void Update(MyEngine::Input input);
	//•`‰æˆ—
	virtual void Draw();
	//I—¹ˆ—
	virtual void End();

};

