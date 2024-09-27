#pragma once
#include "SceneBase.h"
class SceneSelect : public SceneBase
{
public:
	SceneSelect(SceneManager& sceneManager);
	virtual ~SceneSelect();
	//‰Šú‰»ˆ—
	virtual void Init();
	//XVˆ—
	virtual void Update(MyEngine::Input input);
	//•`‰æˆ—
	virtual void Draw();
	//I—¹ˆ—
	virtual void End();
};

