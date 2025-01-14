#pragma once
#include <string>
#include <map>
class MenuUi
{
public:
	enum class SelectItem
	{
		kBattle,
		kTutorial,
		kTitle,
		kEndGame,
		kItemEnd = kEndGame,
		kItemNum
	};

private:

	struct StringUiStatus
	{
		std::string showString = "empty";
		int showPosX = 0;
		int showPosY = 0;
	};

	struct GraphUiStatus
	{
		int handle = -1;
		int showPosX = 0;
		int showPosY = 0;
		int alpha = 255;
	};

public:

	MenuUi();

	~MenuUi();

	void Init();

	int Update();

	void DrawItem();

	void DrawModel();
	
	void EntryGraph(std::string name,GraphUiStatus status);

	GraphUiStatus& GetGraphRef(std::string name);

private:

	SelectItem m_selectItem;

	StringUiStatus m_stringUi[static_cast<int>(SelectItem::kItemNum)];

	std::map<std::string, GraphUiStatus> m_graphMap;

	int m_selectItemFontHandle;

	int m_skyDomeHandle;

	//ƒƒjƒ…[‰æ–Ê‚Å‘O‰ñ€–Ú‚ğ“®‚©‚µ‚Ä‚©‚ç‚ÌŠÔ
	int m_selectItemMoveTime;
};