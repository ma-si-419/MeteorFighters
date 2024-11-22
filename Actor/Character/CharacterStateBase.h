#pragma once
#include "StateBase.h"
#include "Vector3.h"
#include <string>
class CharacterBase;
class Collidable;
class Attack;
class Input;
class Effect;
class CharacterStateBase : public StateBase
{
public:
	enum class CharacterStateKind
	{
		kIdle,//何もしていない状態
		kMove,//移動状態
		kJump,//ジャンプ状態
		kGuard,//ガード状態
		kNormalAttack,//通常攻撃状態
		kSpecialAttack,//必殺技状態
		kRush,//突撃状態
		kHitAttack,//攻撃を受けた時
		kButtonBashing,//敵との押し合い
		kCharge,//チャージ状態
		kStateKindNum//Stateの数
	};

public:

	CharacterStateBase(std::shared_ptr<CharacterBase> character);

	virtual void OnCollide(std::shared_ptr<Collidable> collider);

	CharacterStateKind GetKind() { return m_kind; }

	//次のState
	std::shared_ptr<CharacterStateBase> m_pNextState;

protected:

	virtual void ChangeState(std::shared_ptr<CharacterStateBase> nextState);

	/// <summary>
	/// 対戦相手の座標を取得する
	/// </summary>
	/// <returns>対戦相手の座標</returns>
	MyEngine::Vector3 GetTargetPos();

	/// <summary>
	/// 対戦相手の移動ベクトルを取得する
	/// </summary>
	/// <returns>対戦相手の移動ベクトル</returns>
	MyEngine::Vector3 GetTargetVelo();

	/// <summary>
	/// 対戦相手のやられ状態を取得する
	/// </summary>
	/// <returns>対戦相手のやられ状態</returns>
	int GetTargetHitReaction();

	/// <summary>
	/// プレイヤーの移動ベクトルを設定する
	/// </summary>
	/// <param name="velo">移動ベクトル</param>
	void SetCharacterVelo(MyEngine::Vector3 velo);

	/// <summary>
	/// プレイヤーの座標を設定する
	/// </summary>
	/// <param name="pos">ワールド座標</param>
	void SetCharacterPos(MyEngine::Vector3 pos);

	/// <summary>
	/// 対戦相手の背後座標を取得する
	/// </summary>
	/// <param name="distance">距離</param>
	/// <returns>対戦相手の背後座標</returns>
	MyEngine::Vector3 GetTargetBackPos(float distance);

	/// <summary>
	/// プレイヤーの残像を作成する
	/// </summary>
	void CreateAfterImage();

	/// <summary>
	/// アニメーション番号を取得する
	/// </summary>
	/// <param name="animName">アニメーションの名前</param>
	/// <returns>アニメーション番号</returns>
	int GetAttackAnimKind(std::string animName);

	/// <summary>
	/// キャラクターの移動速度を取得する
	/// </summary>
	/// <returns>使用しているキャラクターの移動速度</returns>
	float GetSpeed();

	/// <summary>
	/// カメラの補正を止める
	/// </summary>
	void StopCameraCorrection();

	/// <summary>
	/// カメラの移動を止める(時間指定)
	/// </summary>
	/// <param name="time">カメラを止める時間</param>
	void StopCamera(int time);

	/// <summary>
	/// カメラの補正を始める
	/// </summary>
	void StartCameraCorrection();

	/// <summary>
	/// カメラを揺れを設定する
	/// </summary>
	/// <param name="time">揺らす時間</param>
	void ShakeCamera(int time);

	/// <summary>
	/// カメラを緩やかに揺らす
	/// </summary>
	void SwayCamera();

	/// <summary>
	/// 攻撃を受けた時に呼ぶ関数
	/// </summary>
	/// <param name="attack">受けた攻撃のクラス</param>
	/// <param name="stateKind">現在のState</param>
	void HitAttack(std::shared_ptr<Attack> attack, CharacterStateBase::CharacterStateKind stateKind);

	/// <summary>
	/// エフェクトを登録する
	/// </summary>
	/// <param name="effect">登録したいエフェクトのポインタ</param>
	void EntryEffect(std::shared_ptr<Effect> effect);

	/// <summary>
	/// エフェクトを削除する
	/// </summary>
	/// <param name="effect">削除したいエフェクトのポインタ</param>
	void ExitEffect(std::shared_ptr<Effect> effect);

	//現在の状態
	CharacterStateKind m_kind = CharacterStateKind::kIdle;

	//Stateに入って何フレーム立ったか
	float m_time = 0.0f;

	//自分のキャラクターのポインタ
	std::shared_ptr<CharacterBase> m_pCharacter;

	//自分が人によって操作されているか
	bool m_isPlayer;

	//自分についているエフェクト(基本的に1つだけ再生するようにする)
	std::shared_ptr<Effect> m_pEffect;
};

