#include "PlayerStateMove.h"
#include "PlayerStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"
#include <cmath>

namespace
{
#ifdef _DEBUG
	constexpr float kSpeed = 0.7f;

#endif // _DEBUG
}

PlayerStateMove::PlayerStateMove(std::shared_ptr<Player> player) :
	PlayerStateBase(player)
{
}

void PlayerStateMove::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kMove;
}

void PlayerStateMove::Update()
{
	//プレイヤーからエネミーへのベクトル
	MyEngine::Vector3 playerToTarget = GetEnemyPos() - m_pPlayer->GetPos();

	//インプットを管理しているクラスの参照
	MyEngine::Input& input = MyEngine::Input::GetInstance();
	//移動ベクトル
	MyEngine::Vector3 moveVec;

	//スティックの情報取得
	MyEngine::Input::StickInfo stick = input.GetStickInfo();

	//左スティックの傾き取得
	MyEngine::Vector3 leftStickDir(stick.leftStickX, 0, -stick.leftStickY);
	//移動ベクトルが0じゃなければ
	if (leftStickDir.sqLength() > 0.001)
	{
		//移動方向
		MyEngine::Vector3 moveDir = leftStickDir.Normalize();

		//エネミーの方向に移動方向を回転させる
		float vX = GetEnemyPos().x - m_pPlayer->GetPos().x;
		float vZ = GetEnemyPos().z - m_pPlayer->GetPos().z;

		float angle = std::atan2f(vX, vZ);

		MyEngine::Vector3 rotation(0.0f, angle, 0.0f);

		MATRIX mat = rotation.GetRotationMat();

		moveDir = moveDir.MatTransform(mat);

		//移動方向にスピードをかける
		moveVec = moveDir * kSpeed;
	}
	
	//移動していなかったら
	if (moveVec.sqLength() == 0)
	{
		//アイドル状態に戻る
		std::shared_ptr<PlayerStateIdle> next = std::make_shared<PlayerStateIdle>(m_pPlayer);

		ChangeState(next);
	}
	//移動ベクトルを設定する
	SetPlayerVelo(moveVec);


#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Move", GetColor(255, 255, 255));

	if (MyEngine::Input::GetInstance().IsTrigger("A"))
	{
		std::shared_ptr<PlayerStateIdle> next = std::make_shared<PlayerStateIdle>(m_pPlayer);

		ChangeState(next);
	}
#endif // _DEBUG

}
void PlayerStateMove::Exit()
{
}
