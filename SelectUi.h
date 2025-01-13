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

	void SetNumber(int number,bool isPlayer);

private:

	std::map<GraphName,GraphData> m_drawGraphs;

	int m_playerNumber;

	int m_lastPlayerNumber;

	int m_enemyNumber;

	int m_lastEnemyNumber;
};

