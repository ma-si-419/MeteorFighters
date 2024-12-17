#pragma once
#include "SceneBase.h"
class SceneSelect : public SceneBase
{
public:
	SceneSelect(SceneManager& sceneManager);
	virtual ~SceneSelect();
	//‰Šú‰»ˆ—
	virtual void Init() override;
	//XVˆ—
	virtual void Update() override;
	//•`‰æˆ—
	virtual void Draw() override;
	//I—¹ˆ—
	virtual void End() override;

private:

	void SelectOnePlayer();

	void SelectTwoPlayer();

	void ConfirmCharacter();

private:



	using UpdateSelectFunc = void (SceneSelect::*)();

	UpdateSelectFunc m_updateSelectFunc;

	int m_playerNumber;

	int m_enemyNumber;

};

