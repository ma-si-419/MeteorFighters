#pragma once
class SelectUi
{
public:

	SelectUi();
	virtual ~SelectUi();

	void Draw();

	void ChangeNumber(int number,bool isPlayer);

private:

	int m_playerGraph;

	int m_lastPlayerGraph;

	int m_enemyGraph;

	int m_lastEnemyGraph;
};

