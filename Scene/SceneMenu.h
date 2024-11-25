#pragma once
#include "SceneBase.h"
#include <string>
class SceneMenu : public SceneBase
{
public:
	enum class ItemKind
	{
		k1PvsCPU,
		k1Pvs2P,
		kCPUvsCPU,
		kOption,
		kExit,
		kSelectEnd = kExit,
		kItemKindNum
	};
	
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


	struct ShowUiStatus
	{
		std::string showString = "empty";
		int showPosX = 0;
		int showPosY = 0;
	};

	int m_selectKind;

	ShowUiStatus m_showUi[static_cast<int>(ItemKind::kItemKindNum)];
	
};

