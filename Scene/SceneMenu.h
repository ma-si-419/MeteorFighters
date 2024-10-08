#pragma once
#include "SceneBase.h"
#include <string>
class SceneMenu : public SceneBase
{
public:
	enum class ItemKind
	{
		k1PVsCPU,
		k1PVs2P,
		kCPUVsCPU,
		kOption,
		kExit,
		kSelectEnd = kExit,
		kItemKindNum
	};
	
	SceneMenu(SceneManager& sceneManager);
	virtual ~SceneMenu();
	//初期化処理
	virtual void Init();
	//更新処理
	virtual void Update();
	//描画処理
	virtual void Draw();
	//終了処理
	virtual void End();
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

