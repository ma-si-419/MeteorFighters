#pragma once
#include <map>
class SelectUi
{
public:
	enum class UiSituation
	{
		kSelect1P,
		kSelect2P
	};

private:
	enum class GraphName
	{
		kLastPlayer,
		kPlayer,
		kLastEnemy,
		kEnemy,
		kIcon0,
		kIcon1,
		kIcon2,
		kIconFrame1P,
		kIconFrame2P,
		kIconFrameBoth,
		kHeadSet,
		kVs
	};

	struct GraphData
	{
		int posX = 0;
		int posY = 0;
		double scale = 1.0;
		int alpha = 255;
		int handle = -1;
	};

public:

	SelectUi();
	virtual ~SelectUi();
	
	void Init();

	void Update();

	void Draw();

	/// <summary>
	/// どのキャラクターを選択しているかを設定する
	/// </summary>
	/// <param name="number">選択している番号</param>
	/// <param name="isPlayer">1P側ならtrue</param>
	void SetNumber(int number,bool isPlayer);

	/// <summary>
	/// どのアイコンを選択しているかを設定する
	/// </summary>
	/// <param name="number">選択しているアイコン</param>
	/// <param name="isPlayer">1P側ならtrue</param>
	void SetIconFrame(int number,bool isPlayer);

	/// <summary>
	/// 現在の状況を設定する
	/// </summary>
	/// <param name="situation">次の状況</param>
	void ChangeSituation(UiSituation situation);

private:

	void Update1P();

	void Update2P();

private:

	using UpdateFunc = void (SelectUi::*)();

	//更新関数
	UpdateFunc m_updateFunc;

	std::map<GraphName,GraphData> m_drawGraphs;

	//今表示している1P側のキャラクター
	int m_playerNumber;

	//ひとつ前に表示していた1P側のキャラクター
	int m_lastPlayerNumber;

	//今表示している2P側のキャラクター
	int m_enemyNumber;

	//ひとつ前に表示していた2P側のキャラクター
	int m_lastEnemyNumber;

	//スカイドームのハンドル
	int m_skyDomeHandle;

	//選択しているアイコンのフレームの拡縮のための変数
	float m_iconFrameScalling;

	//現在の状況
	UiSituation m_situation;
};

