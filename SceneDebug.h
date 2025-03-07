#pragma once
#include "SceneBase.h"
#include <memory>
#include "Vector2.h"


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

	/// <summary>
	/// ダメージを描画する
	/// </summary>
	/// <param name="damage">ダメージ量</param>
	void DrawDamage(int damage);

private:

	////////////////////////////////
	//デバッグ

	//現在のコンボ数
	int m_combo;

	//ダメージを表示する座標
	int m_damagePosX;

	//現在のダメージ
	int m_damage;

	//表示しているダメージ
	int m_showDamage;

	//表示しているダメージを増やす時間
	int m_showDamageAddTime;

	//1フレームで増やすダメージ
	int m_showDamageAddNum;

	//コンボ数が切り替わってからの時間
	int m_comboTime;

	//コンボのUIのX座標
	MyEngine::Vector2 m_comboPos;

	//コンボの数字の拡大率
	double m_comboScale[2];

	//コンボのアルファ値
	int m_comboAlpha;

	////////////////////////////////

	int m_damageFontHandle;

	int m_numberGraphHandle[10];

	int m_comboBarGraphHandle;

	int m_hitGraphHandle;

	int m_selectScene;

	int m_selectSceneMoveTime;

};