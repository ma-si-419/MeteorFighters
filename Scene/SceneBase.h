#pragma once
#include "SceneManager.h"
class Input;
class SceneBase
{
public:
	SceneBase(SceneManager& sceneManager) : m_sceneManager(sceneManager) {}
	virtual ~SceneBase() {}
	//初期化処理
	virtual void Init() abstract;
	//更新処理
	virtual void Update() abstract;
	//非同期処理が行われているときのアップデート
	virtual void UpdateAsyncLoad() abstract;
	//描画処理
	virtual void Draw() abstract;
	//終了処理
	virtual void End() abstract;

protected:
	//シーン遷移などを管理するクラス
	SceneManager& m_sceneManager;
};