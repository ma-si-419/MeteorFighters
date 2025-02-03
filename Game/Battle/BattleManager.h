#pragma once
#include "GameManagerBase.h"
#include <memory>
#include <vector>
#include <string>
#include "Vector3.h"
#include "Character.h"
#include "Game.h"

class BattleUi;
class BattleManager : public GameManagerBase
{
public:

	BattleManager(std::shared_ptr<GameCamera> camera);
	~BattleManager();

	/// <summary>
	/// ゲームマネージャーの初期化
	/// </summary>
	void Init() override;

	/// <summary>
	/// ゲームマネージャーの更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 攻撃の描画などを行う
	/// </summary>
	void Draw() override;

	/// <summary>
	/// シーンから抜けるときなどに使用
	/// </summary>
	void Final();


	/// <summary>
	/// BGMの名前をセットする
	/// </summary>
	/// <param name="name">BGMの名前</param>
	void SetMusicName(std::string name) { m_bgmName = name; }

private:

	/// <summary>
	/// 開始時の演出中のアップデート
	/// </summary>
	void UpdateStart();

	/// <summary>
	/// バトル時アップデート
	/// </summary>
	void UpdateBattle();

	/// <summary>
	/// ノックアウト時のアップデート
	/// </summary>
	void UpdateKnockOut();

	/// <summary>
	/// リザルト時のアップデート
	/// </summary>
	void UpdateResult();

	/// <summary>
	/// リトライ時のアップデート
	/// </summary>
	void UpdateRetry();

	/// <summary>
	/// メニューを開いている時のアップデート
	/// </summary>
	void UpdateMenu();

	/// <summary>
	/// 状況を変化させる
	/// </summary>
	/// <param name="situation">変更後の状況</param>
	void ChangeSituation(BattleSituation situation);

private:

	using UpdateSituationFunc = void (BattleManager::*)();

	UpdateSituationFunc m_updateSituationFunc;

	//バトル時のUI
	std::shared_ptr<BattleUi> m_pBattleUi;

	int m_menuSelectNumber;

	//BGMの名前
	std::string m_bgmName;
};