#pragma once
#include <string>
#include <map>
class MenuUi
{
public:
	enum class SelectItem
	{
		kSelect,
		kTutorial,
		kOption,
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
		int srcPosX = 0;
		int graphSizeX = 0;
		int graphSizeY = 0;
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

	void UpdateCommon();

	int UpdateSelect();

	int UpdateOption();

	int UpdateEndGame();

	void DrawOption();

	void DrawEndGame();

	void DrawNone() {};//何もしない

private:

	using UpdateFunc = int (MenuUi::*)();

	using DrawFunc = void (MenuUi::*)();

	//更新関数
	UpdateFunc m_updateFunc;

	//描画関数
	DrawFunc m_drawFunc;

	SelectItem m_lastSelectItem;

	SelectItem m_selectItem;

	StringUiStatus m_selectStringUi[static_cast<int>(SelectItem::kItemNum)];

	std::map<std::string, GraphUiStatus> m_selectGraphMap;

	int m_selectItemFontHandle;

	int m_skyDomeHandle;

	int m_optionFontHandle;

	int m_optionButtonFontHandle;

	//メニュー画面で前回項目を動かしてからの時間
	int m_selectItemMoveTime;

	//右側に表示する画像の切り取る座標
	int m_rightGraphSrcNum;

	//右側に表示する画像を切り取る座標を切り替える時間
	int m_rightGraphChangeTime;

	//オプション画面で選択している項目
	int m_optionSelectItem;

	//オプション画面のアルファ値
	int m_optionAlpha;

	//オプション画面を終わるときにtrueにする
	bool m_isEndOption;

	//シークバーを動かすかどうか
	bool m_isMoveSeekBar;

	//シークバーのつまみを拡縮させる際に使用する変数
	float m_thumbSize;

	//音量の設定
	float m_masterVolume;
	float m_seVolume;
	float m_bgmVolume;
	float m_voiceVolume;

};