#include "PlayerStateIdle.h"
#include "PlayerStateNormalAttack.h"
#include "PlayerStateMove.h"
#include "PlayerStateJump.h"
#include "PlayerStateDash.h"
#include "PlayerStateRush.h"
#include "PlayerStateGuard.h"
#include "PlayerStateCharge.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"
#include "GameSceneConstant.h"
PlayerStateIdle::PlayerStateIdle(std::shared_ptr<Player> player) :
	PlayerStateBase(player),
	m_attackKey("empty"),
	m_attackButtonPushTime(0),
	m_isPlayEndAnim(false),
	m_endAnimTime(0)
{
}

void PlayerStateIdle::SetEndAnim(int kind, int time)
{
	m_pPlayer->ChangeAnim(static_cast<CharacterBase::AnimKind>(kind), false);
	m_endAnimTime = time;
	m_isPlayEndAnim = true;
}

void PlayerStateIdle::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kIdle;
}

void PlayerStateIdle::Update()
{

#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Idle", GetColor(255, 255, 255));

#endif // _DEBUG


	//Stateにいる時間を計測する
	m_time++;

	//アイドル状態の時は移動しない
	SetPlayerVelo(MyEngine::Vector3(0, 0, 0));
	
	//アニメーションが終わる時間になっていれば
	if (m_time > m_endAnimTime)
	{
		m_isPlayEndAnim = false;
		//アイドルアニメーションでなければ
		if (m_pPlayer->GetPlayAnimKind() != CharacterBase::AnimKind::kIdle)
		{
			m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kIdle, true);
		}
	}

	//前のフレームの終了アニメーションが再生されていたら下の条件分岐を通らない
	if (m_isPlayEndAnim) return;

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

	//一定時間レフトショルダーボタンが押されたら
	if (input.GetPushTriggerTime(false) > GameSceneConstant::kChargeStateChangeTime)
	{
		//次のStateのポインタ作成
		auto next = std::make_shared<PlayerStateCharge>(m_pPlayer);
		//StateをChargeに変更する
		ChangeState(next);
		return;
	}

	//ダッシュボタンが押された時
	if (input.IsTrigger("A"))
	{
		//一緒にレフトショルダーも押されていたら
		if (input.IsPushTrigger(false))
		{
			//ダッシュのコストがあれば
			if (m_pPlayer->SubMp(GameSceneConstant::kDashCost))
			{
				//突撃状態に移行する
				auto next = std::make_shared<PlayerStateRush>(m_pPlayer);

				next->SetMoveDir(MyEngine::Vector3(0,0,1));

				ChangeState(next);
				return;
			}
		}

		//敵との距離からダッシュかステップか判断する
		//(ステップかダッシュかの判定はDashStateの中でも行う)
		//(ここではMPを消費するかしないか、DashStateにはいるかどうかを判断する)
		if ((GetEnemyPos() - m_pPlayer->GetPos()).Length() > GameSceneConstant::kNearLange)
		{
			//遠かった場合Mpを消費してダッシュする
			if (m_pPlayer->SubMp(GameSceneConstant::kDashCost))
			{
				auto next = std::make_shared<PlayerStateDash>(m_pPlayer);

				next->SetMoveDir(MyEngine::Vector3(0.0f, 0.0f, 1.0f));

				ChangeState(next);
				return;
			}
		}
		//敵との距離が近い場合
		else
		{
			//MPを消費せずにステップをする
			auto next = std::make_shared<PlayerStateDash>(m_pPlayer);

			next->SetMoveDir(MyEngine::Vector3(0.0f, 0.0f, 1.0f));

			ChangeState(next);
			return;
		}
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
		if (input.IsPress("RB") ||
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


}
void PlayerStateIdle::Exit()
{

}

void PlayerStateIdle::OnCollide(std::shared_ptr<Collidable> collider)
{
}
