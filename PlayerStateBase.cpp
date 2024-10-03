#include "PlayerStateBase.h"
#include "PlayerStateIdle.h"
#include "PlayerStateMove.h"
#include "PlayerStateGuard.h"
#include "PlayerStateAttack.h"
#include "PlayerStateRush.h"
#include "PlayerStateHitAttack.h"
#include "PlayerStateButtonBashing.h"

void PlayerStateBase::ChangeState(CharacterStateKind nextState)
{
	//終了処理
	Exit();

	//アイドル状態
	if (nextState == CharacterStateKind::kIdle)
	{
		//ポインタの作成
		m_pNextState = std::make_shared<PlayerStateIdle>();
		//次のStateの開始処理
		m_pNextState->Enter();
		return;
	}
	//移動状態
	else if (nextState == CharacterStateKind::kMove)
	{
		//ポインタの作成
		m_pNextState = std::make_shared<PlayerStateMove>();
		//次のStateの開始処理
		m_pNextState->Enter();
		return;
	}
	//ガード状態
	else if (nextState == CharacterStateKind::kGuard)
	{
		//ポインタの作成
		m_pNextState = std::make_shared<PlayerStateGuard>();
		//次のStateの開始処理
		m_pNextState->Enter();
		return;
	}
	//攻撃状態
	else if (nextState == CharacterStateKind::kAttack)
	{
		//ポインタの作成
		m_pNextState = std::make_shared<PlayerStateAttack>();
		//次のStateの開始処理
		m_pNextState->Enter();
		return;
	}
	//突撃状態
	else if (nextState == CharacterStateKind::kRush)
	{
		//ポインタの作成
		m_pNextState = std::make_shared<PlayerStateRush>();
		//次のStateの開始処理
		m_pNextState->Enter();
		return;
	}
	//攻撃被弾状態
	else if (nextState == CharacterStateKind::kHitAttack)
	{
		//ポインタの作成
		m_pNextState = std::make_shared<PlayerStateHitAttack>();
		//次のStateの開始処理
		m_pNextState->Enter();
		return;
	}
	//ボタン連打状態(攻撃の押し合いなど)
	else if (nextState == CharacterStateKind::kButtonBashing)
	{
		//ポインタの作成
		m_pNextState = std::make_shared<PlayerStateButtonBashing>();
		//次のStateの開始処理
		m_pNextState->Enter();
		return;
	}
}
