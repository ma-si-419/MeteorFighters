#include "PlayerStateIdle.h"
#include "PlayerStateNormalAttack.h"
#include "PlayerStateMove.h"
#include "PlayerStateGuard.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"

PlayerStateIdle::PlayerStateIdle(std::shared_ptr<Player> player) :
	PlayerStateBase(player)
{
}

void PlayerStateIdle::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kIdle;
	m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kIdle, true);
}

void PlayerStateIdle::Update()
{

#ifdef _DEBUG

		DrawString(0, 16, "PlayerState:Idle", GetColor(255, 255, 255));

	//if (MyEngine::Input::GetInstance().IsTrigger("A"))
	//{
	//	ChangeState(CharacterStateKind::kAttack);
	//}
	//else if (MyEngine::Input::GetInstance().IsTrigger("B"))
	//{
	//	ChangeState(CharacterStateKind::kGuard);
	//}
	//else if (MyEngine::Input::GetInstance().IsTrigger("X"))
	//{
	//	ChangeState(CharacterStateKind::kHitAttack);
	//}
	//else if (MyEngine::Input::GetInstance().IsTrigger("Y"))
	//{
	//	ChangeState(CharacterStateKind::kMove);
	//}
	//else if (MyEngine::Input::GetInstance().IsTrigger("Lb"))
	//{
	//	ChangeState(CharacterStateKind::kButtonBashing);
	//}
	//else if (MyEngine::Input::GetInstance().IsTrigger("Rb"))
	//{
	//	ChangeState(CharacterStateKind::kRush);
	//}
#endif // _DEBUG

	auto& input = MyEngine::Input::GetInstance();

	//攻撃入力がされたら
	if (input.IsTrigger("X"))
	{
		//次のStateのポインタ作成
		std::shared_ptr<PlayerStateNormalAttack> next = std::make_shared<PlayerStateNormalAttack>(m_pPlayer);
		//何の攻撃を行うかをAttackStateに渡す
		next->SetAttack("Low1");
		//StateをAttackに変更する
		ChangeState(next);
		return;
	}

	//移動入力がされていたら
	if (input.GetStickInfo().leftStickX != 0 ||
		input.GetStickInfo().leftStickY != 0)
	{
		//次のStateのポインタ作成
		std::shared_ptr<PlayerStateMove> next = std::make_shared<PlayerStateMove>(m_pPlayer);
		//StateをMoveに変更する
		ChangeState(next);
		return;
	}

	//ガード入力がされていたら
	if (input.IsPress("B"))
	{
		//次のStateのポインタ作成
		std::shared_ptr<PlayerStateGuard> next = std::make_shared<PlayerStateGuard>(m_pPlayer);
		//StateをMoveに変更する
		ChangeState(next);
		return;
	}
		
	SetPlayerVelo(MyEngine::Vector3(0, 0, 0));

}
void PlayerStateIdle::Exit()
{

}

void PlayerStateIdle::OnCollide(std::shared_ptr<Collidable> collider)
{
}
