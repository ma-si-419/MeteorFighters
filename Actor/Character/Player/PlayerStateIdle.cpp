#include "PlayerStateIdle.h"
#include "PlayerStateNormalAttack.h"
#include "PlayerStateMove.h"
#include "PlayerStateJump.h"
#include "PlayerStateGuard.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"
#include "GameSceneConstant.h"
PlayerStateIdle::PlayerStateIdle(std::shared_ptr<Player> player) :
	PlayerStateBase(player),
	m_attackKey("empty"),
	m_attackButtonPushTime(0)
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

#endif // _DEBUG

	auto& input = MyEngine::Input::GetInstance();

	//攻撃ボタンが押されていないときに
	if (m_attackKey == "empty")
	{
		//格闘ボタンが押された時
		if (input.IsPress("X"))
		{
			m_attackKey = "X";
		}
		else if (input.IsPress("Y"))
		{
			m_attackKey = "Y";
		}
	}
	//攻撃ボタンが押されていたら
	else
	{
		//押しているフレーム数を数える
		m_attackButtonPushTime++;

		//押していたボタンが離されたら
		if (input.IsRelease(m_attackKey) ||
			m_attackButtonPushTime > GameSceneConstant::kChargeAttackTime)
		{
			//チャージされていたかどうか判定
			bool isCharge = m_attackButtonPushTime > GameSceneConstant::kChargeAttackTime;
			//次のStateのポインタ作成
			auto next = std::make_shared<PlayerStateNormalAttack>(m_pPlayer);
			//何の攻撃を行うかをAttackStateに渡す
			next->SetAttack(m_attackKey, isCharge);
			//StateをAttackに変更する
			ChangeState(next);
			return;
		}
	}

	//移動入力がされていたら
	if (input.GetStickInfo().leftStickX != 0 ||
		input.GetStickInfo().leftStickY != 0)
	{
		//次のStateのポインタ作成
		auto next = std::make_shared<PlayerStateMove>(m_pPlayer);
		//StateをMoveに変更する
		ChangeState(next);
		return;
	}

	//地上にいるときに
	if (m_pPlayer->IsGround())
	{
		//ジャンプボタンが押されたら
		if (input.IsPress("RB"))
		{
			//ジャンプState作成
			auto next = std::make_shared<PlayerStateJump>(m_pPlayer);

			next->StartJump();

			ChangeState(next);

			return;
		}


	}
	//空中にいるときに
	else
	{
		//上昇ボタンか下降ボタンが押されたら
		if (input.IsPress("RB")||
			input.IsPushTrigger(true))
		{
			//次のStateのポインタ作成
			auto next = std::make_shared<PlayerStateMove>(m_pPlayer);
			//StateをMoveに変更する
			ChangeState(next);
			return;
		}
	}

	//ガード入力がされていたら
	if (input.IsPress("B"))
	{
		//次のStateのポインタ作成
		auto next = std::make_shared<PlayerStateGuard>(m_pPlayer);
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
