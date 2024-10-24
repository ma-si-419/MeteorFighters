#pragma once
#include "StateBase.h"
class Collidable;
class CharacterStateBase : public StateBase
{
public:
	enum class CharacterStateKind
	{
		kIdle,//何もしていない状態
		kMove,//移動状態
		kGuard,//ガード状態
		kAttack,//攻撃状態
		kRush,//突撃状態
		kHitAttack,//攻撃を受けた時
		kButtonBashing,//敵との押し合い
		kStateKindNum
	};

public:

	virtual void OnCollide(std::shared_ptr<Collidable> collider) abstract;

	CharacterStateKind GetKind() { return m_kind; }

	//次のState
	std::shared_ptr<CharacterStateBase> m_pNextState;

protected:
	
	virtual void ChangeState(std::shared_ptr<CharacterStateBase> nextState) abstract;

	//現在の状態
	CharacterStateKind m_kind = CharacterStateKind::kIdle;

	//Stateに入って何フレーム立ったか
	int m_time = 0;
};

