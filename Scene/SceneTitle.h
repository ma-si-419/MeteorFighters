#pragma once
#include "SceneBase.h"
#include <memory>

class TitleUi;
class SceneTitle : public SceneBase
{
public:
	SceneTitle(SceneManager& sceneManager);
	virtual ~SceneTitle();
	//‰Šú‰»ˆ—
	virtual void Init() override;
	//XVˆ—
	virtual void Update() override;
	//•`‰æˆ—
	virtual void Draw() override;
	//I—¹ˆ—
	virtual void End() override;

private:
	std::shared_ptr<TitleUi> m_pTitleUi;
};

