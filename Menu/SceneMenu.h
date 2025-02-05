#pragma once
#include "SceneBase.h"
#include <memory>

class MenuManager;
class SceneMenu : public SceneBase
{
public:

	SceneMenu(SceneManager& sceneManager);
	virtual ~SceneMenu();
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

	std::shared_ptr<MenuManager> m_pMenuManager;
};