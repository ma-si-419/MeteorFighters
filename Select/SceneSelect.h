#pragma once
#include "SceneBase.h"
class SceneSelect : public SceneBase
{
public:
	SceneSelect(SceneManager& sceneManager);
	virtual ~SceneSelect();
	//初期化処理
	virtual void Init() override;
	//更新処理
	virtual void Update() override;
	//非同期処理が行われているときのアップデート
	virtual void UpdateAsyncLoad() override;
	//描画処理
	virtual void Draw() override;
	//終了処理
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

