#pragma once
#include "Actor.h"
class SceneGame;
class GameManager;
class CharacterStateBase;
class CharacterBase : public Actor
{
public: 

	enum class AnimKind
	{
		kIdle,
		kGuard,
		kStep,
		kMove
	};

	enum class CharacterKind
	{
		kMouse,
		kBigBoy,
		kBlueHead
	};

	enum class CharacterStatusDataSort
	{
		kHp,
		kAtk,
		kDef,
		kSpd,
		kChargeSpd
	};

	struct CharacterStatus
	{
		float hp = 0;
		float atk = 0;
		float def = 0;
		float spd = 0;
		float chargeSpd = 0;
	};

	CharacterBase(ObjectTag tag,CharacterKind kind);
	~CharacterBase();

	/// <summary>
	/// 敵の座標などを取得するためにゲームマネージャーのポインタをセットする
	/// </summary>
	/// <param name="manager">ゲームマネージャーのポインタ</param>
	void SetGameManager(std::shared_ptr<GameManager> manager);

	/// <summary>
	/// 現在のワールド座標を返す
	/// </summary>
	/// <returns>自身のワールド座標</returns>
	MyEngine::Vector3 GetPos();

	/// <summary>
	/// ステータスのセットする
	/// </summary>
	/// <param name="status">キャラクターのステータス</param>
	void SetStatus(CharacterStatus status) { m_status = status; }

	/// <summary>
	/// キャラクターの攻撃力を取得する
	/// </summary>
	/// <returns>キャラクターの攻撃力</returns>
	float GetPower() { return m_status.atk; }

	/// <summary>
	/// 再生するアニメーションを変える
	/// </summary>
	/// <param name="animNumber">アニメーション番号</param>
	/// <param name="loop">繰り返すかどうか</param>
	void ChangeAnim(int animNumber,bool loop);

	/// <summary>
	/// アニメーションを再生する
	/// </summary>
	void PlayAnim();

	/// <summary>
	/// アニメーションが終了したかを取得する
	/// </summary>
	/// <returns>true:アニメーション終了時　false:アニメーション再生時</returns>
	bool GetAnimEnd();

protected:

	

	//モデルハンドル
	int m_modelHandle;
	//シーンのポインタ
	std::shared_ptr<SceneGame> m_pScene;
	//マネージャーのポインタ
	std::shared_ptr<GameManager> m_pGameManager;
	//State
	std::shared_ptr<CharacterStateBase> m_pState;
	//使うキャラクターの種類
	CharacterKind m_characterKind;
	//自身のステータス
	CharacterStatus m_status;


	//アニメーションの情報
	int m_attachAnim;
	float m_totalAnimTime;
	float m_playAnimTime;
	float m_animPlaySpeed;
	bool m_isLoop;

};