#include "PlayerStateRush.h"
#include "PlayerStateDash.h"
#include "PlayerStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"
#include "GameSceneConstant.h"

PlayerStateRush::PlayerStateRush(std::shared_ptr<Player> player) :
	PlayerStateBase(player)
{
}

void PlayerStateRush::SetMoveDir(MyEngine::Vector3 dir)
{
}

void PlayerStateRush::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kRush;
}

void PlayerStateRush::Update()
{
	auto& input = MyEngine::Input::GetInstance();

	//スティックの傾き
	MyEngine::Vector3 stickDir(input.GetStickInfo().leftStickX, 0, -input.GetStickInfo().leftStickY);
	
	//Aボタンが押されたときに
	if (input.IsTrigger("A"))
	{
		//レフトショルダーも押されていたら
		if (input.IsPushTrigger(false))
		{
			//敵の近くまで向かう突撃状態になる
			m_isRushEnemy = true;
		}
		//押されていなければ
		else
		{
			//スティックの傾きを見る
			if (stickDir.SqLength() > 0.001f)
			{
				//ダッシュ状態に移行する
				auto next = std::make_shared<PlayerStateDash>(m_pPlayer);

				next->SetMoveDir(stickDir.Normalize());

				ChangeState(next);

				return;
			}
			//スティックが傾いていなければ
			else
			{
				//アイドル状態に戻る
				auto next = std::make_shared<PlayerStateIdle>(m_pPlayer);

				ChangeState(next);

				return;
			}
		}
	}


	float speed = m_pPlayer->GetSpeed() * GameSceneConstant::kRushMoveSpeedRate;


	if (stickDir.SqLength() > 0.001f)
	{
		//スティックの入力ベクトルが移動ベクトルの真反対であればステートをアイドルに戻す
		if (-stickDir.x == m_moveDir.x && -stickDir.z == m_moveDir.z)
		{
			auto next = std::make_shared<PlayerStateIdle>(m_pPlayer);

			ChangeState(next);

			return;
		}

		//移動の方向ベクトルの計算
		m_moveDir = m_moveDir * (1.0f - GameSceneConstant::kRushMoveDirChangeRate) + stickDir.Normalize() * GameSceneConstant::kRushMoveDirChangeRate;
	}

	MyEngine::Vector3 velo = m_moveDir * speed;

	SetPlayerVelo(velo);

#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Rush", GetColor(255, 255, 255));

#endif // _DEBUG

}
void PlayerStateRush::Exit()
{
}

void PlayerStateRush::OnCollide(std::shared_ptr<Collidable> collider)
{
}
