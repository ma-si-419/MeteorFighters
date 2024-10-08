#pragma once
#include "SceneBase.h"
class SceneTitle : public SceneBase
{
public:
	SceneTitle(SceneManager& sceneManager);
	virtual ~SceneTitle();
	//初期化処理
	virtual void Init();
	//更新処理
	virtual void Update();
	//描画処理
	virtual void Draw();
	//終了処理
	virtual void End();

};

