#pragma once
#include "SceneBase.h"
#include <memory>

class MenuUi;
class SceneMenu : public SceneBase
{
public:

	SceneMenu(SceneManager& sceneManager);
	virtual ~SceneMenu();
	//初期化処理
	virtual void Init() override;
	//更新処理
	virtual void Update() override;
	//描画処理
	virtual void Draw() override;
	//終了処理
	virtual void End() override;

private:

	std::shared_ptr<MenuUi> m_pMenuUi;

};

