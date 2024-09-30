#pragma once
#include "SceneBase.h"
class SceneMenu : public SceneBase
{
public:
	SceneMenu(SceneManager& sceneManager);
	virtual ~SceneMenu();
	//‰Šú‰»ˆ—
	virtual void Init();
	//XVˆ—
	virtual void Update();
	//•`‰æˆ—
	virtual void Draw();
	//I—¹ˆ—
	virtual void End();

};

