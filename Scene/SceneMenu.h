#pragma once
#include "SceneBase.h"
#include <memory>

class MenuUi;
class SceneMenu : public SceneBase
{
public:

	SceneMenu(SceneManager& sceneManager);
	virtual ~SceneMenu();
	//‰Šú‰»ˆ—
	virtual void Init() override;
	//XVˆ—
	virtual void Update() override;
	//•`‰æˆ—
	virtual void Draw() override;
	//I—¹ˆ—
	virtual void End() override;

private:

	std::shared_ptr<MenuUi> m_pMenuUi;

};

