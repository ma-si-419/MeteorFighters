#pragma once
#include "StateBase.h"
#include "Vector3.h"
#include <string>
#include "InputData.h"
class Character;
class Collidable;
class Attack;
class Input;
class Effect;
class EffectManager;
class GameManagerBase;
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

	CharacterStateBase(std::shared_ptr<Character> character);

	virtual void OnCollide(std::shared_ptr<Collidable> collider);

	/// <summary>
	/// 常に更新を行うものだけ更新する
	/// </summary>
	void UpdateCommon();

	/// <summary>
	///	Stateの種類を取得する
	/// </summary>
	/// <returns>Stateの種類</returns>
	CharacterStateKind GetKind() { return m_kind; }

	//次のState
	std::shared_ptr<CharacterStateBase> m_pNextState;

protected:

	virtual void ChangeState(std::shared_ptr<CharacterStateBase> nextState);

	std::shared_ptr<MyEngine::InputData> GetCharacterInput();

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
	/// 攻撃を受けた時に呼ぶ関数
	/// </summary>
	/// <param name="attack">受けた攻撃のクラス</param>
	/// <param name="stateKind">現在のState</param>
	void HitAttack(std::shared_ptr<Attack> attack, CharacterStateBase::CharacterStateKind stateKind);

	/// <summary>
	/// チュートリアルクリア条件を達成したとする
	/// </summary>
	/// <param name="tutorialNumber">チュートリアルの番号(TutorialManager::TutorialSuccessKindをintにキャストする)</param>
	void SuccessTutorial(int tutorialNumber);

protected:

	//現在の状態
	CharacterStateKind m_kind = CharacterStateKind::kIdle;

	//Stateに入って何フレーム立ったか
	float m_time = 0.0f;

	//自分のキャラクターのポインタ
	std::shared_ptr<Character> m_pCharacter;

	//バトルマネージャー
	std::shared_ptr<GameManagerBase> m_pManager;

	//自分についているエフェクト(基本的に1つだけ再生するようにする)
	std::shared_ptr<Effect> m_pEffect;
};

