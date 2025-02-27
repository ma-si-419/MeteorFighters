#pragma once
#include "SceneBase.h"
#include <memory>

class SceneDebug : public SceneBase
{
public:
	SceneDebug(SceneManager& sceneManager);
	virtual ~SceneDebug();
	//初期化処理
	virtual void Init() override;
	//更新処理
	virtual void Update() override;
	//非同期処理が行われているときのアップデート
	virtual void UpdateAsyncLoad() override {};
	//描画処理
	virtual void Draw() override;
	//終了処理
	virtual void End() override;


private:


	/// <summary>
	/// 指定したシーンを作成する
	/// </summary>
	/// <param name="sceneNum">Game::Sceneの番号を指定</param>
	/// <returns>指定したシーンのポインタ</returns>
	std::shared_ptr<SceneBase> CreateScene(int sceneNum);

private:

	////////////////////////////////
	//デバッグ

	//現在のコンボ数
	int m_combo;

	//コンボ数が切り替わってからの時間
	int m_comboTime;

	//コンボのUIのX座標
	int m_comboPosX[2];

	//コンボの数字の拡大率
	double m_comboScale[2];

	//コンボのアルファ値
	int m_comboAlpha;

	////////////////////////////////

	int m_numberGraphHandle[10];

	int m_selectScene;

	int m_selectSceneMoveTime;

};