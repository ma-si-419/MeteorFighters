#pragma once
#include <map>
class SelectUi
{
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
		kIconFrame,
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
	void SetIconFrame(int number);

	/// <summary>
	/// 選択しているアイコンを示すフレームを描画するかどうかを設定する
	/// </summary>
	/// <param name="flag">描画するならtrue</param>
	void SetDrawIconFrame(bool flag);

private:

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
};

