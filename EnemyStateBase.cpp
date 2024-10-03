#include "EnemyStateBase.h"
#include "EnemyStateIdle.h"
#include "EnemyStateMove.h"
#include "EnemyStateGuard.h"
#include "EnemyStateAttack.h"
#include "EnemyStateRush.h"
#include "EnemyStateHitAttack.h"
#include "EnemyStateButtonBashing.h"

void EnemyStateBase::ChangeState(CharacterStateKind nextState)
{
	//終了処理
	Exit();

	//アイドル状態
	if (nextState == CharacterStateKind::kIdle)
	{
		//ポインタの作成
		m_pNextState = std::make_shared<EnemyStateIdle>();
		//次のStateの開始処理
		m_pNextState->Enter();
		return;
	}
	//移動状態
	else if (nextState == CharacterStateKind::kMove)
	{
		//ポインタの作成
		m_pNextState = std::make_shared<EnemyStateMove>();
		//次のStateの開始処理
		m_pNextState->Enter();
		return;
	}
	//ガード状態
	else if (nextState == CharacterStateKind::kGuard)
	{
		//ポインタの作成
		m_pNextState = std::make_shared<EnemyStateGuard>();
		//次のStateの開始処理
		m_pNextState->Enter();
		return;
	}
	//攻撃状態
	else if (nextState == CharacterStateKind::kAttack)
	{
		//ポインタの作成
		m_pNextState = std::make_shared<EnemyStateAttack>();
		//次のStateの開始処理
		m_pNextState->Enter();
		return;
	}
	//突撃状態
	else if (nextState == CharacterStateKind::kRush)
	{
		//ポインタの作成
		m_pNextState = std::make_shared<EnemyStateRush>();
		//次のStateの開始処理
		m_pNextState->Enter();
		return;
	}
	//攻撃被弾状態
	else if (nextState == CharacterStateKind::kHitAttack)
	{
		//ポインタの作成
		m_pNextState = std::make_shared<EnemyStateHitAttack>();
		//次のStateの開始処理
		m_pNextState->Enter();
		return;
	}
	//ボタン連打状態(攻撃の押し合いなど)
	else if (nextState == CharacterStateKind::kButtonBashing)
	{
		//ポインタの作成
		m_pNextState = std::make_shared<EnemyStateButtonBashing>();
		//次のStateの開始処理
		m_pNextState->Enter();
		return;
	}
}
