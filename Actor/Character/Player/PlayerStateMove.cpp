#include "PlayerStateMove.h"
#include "PlayerStateIdle.h"
#include "PlayerStateNormalAttack.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"
#include <cmath>

namespace
{
	constexpr int kChargeAttackTime = 15;
}

PlayerStateMove::PlayerStateMove(std::shared_ptr<Player> player) :
	PlayerStateBase(player),
	m_attackButtonPushTime(0.0f),
	m_attackKey("empty")
{
	m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kSkyIdle,true);
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
	MyEngine::Vector3 velo;
	//移動方向ベクトル
	MyEngine::Vector3 dir;

	//スティックの情報取得
	MyEngine::Input::StickInfo stick = input.GetStickInfo();

	//左スティックの傾き取得
	MyEngine::Vector3 leftStickDir(stick.leftStickX, 0, -stick.leftStickY);
	//移動ベクトルが0じゃなければ
	if (leftStickDir.SqLength() > 0.001)
	{
		//移動方向
		dir = leftStickDir.Normalize();

		//エネミーの方向に移動方向を回転させる
		float vX = GetEnemyPos().x - m_pPlayer->GetPos().x;
		float vZ = GetEnemyPos().z - m_pPlayer->GetPos().z;

		float angle = std::atan2f(vX, vZ);

		MyEngine::Vector3 rotation(0.0f, angle, 0.0f);

		MATRIX mat = rotation.GetRotationMat();

		dir = dir.MatTransform(mat);

		//移動方向にスピードをかける
		velo = dir * GetSpeed();
	}
	//ジャンプボタンが押されたら
	if (input.IsPress("RB"))
	{
		velo.y = GetSpeed();
	}
	//下降ボタンが押されたら
	else if (input.IsPushTrigger(true))
	{
		velo.y = -GetSpeed();
	}
	
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
			m_attackButtonPushTime > kChargeAttackTime)
		{
			//チャージされていたかどうか判定
			bool isCharge = m_attackButtonPushTime > kChargeAttackTime;
			//次のStateのポインタ作成
			std::shared_ptr<PlayerStateNormalAttack> next = std::make_shared<PlayerStateNormalAttack>(m_pPlayer);
			//何の攻撃を行うかをAttackStateに渡す
			next->SetAttack(m_attackKey, isCharge);
			//StateをAttackに変更する
			ChangeState(next);
			return;
		}
	}

	//移動していなかったら
	if (velo.SqLength() == 0)
	{
		//アイドル状態に戻る
		std::shared_ptr<PlayerStateIdle> next = std::make_shared<PlayerStateIdle>(m_pPlayer);

		ChangeState(next);
	}
	//移動ベクトルを設定する
	SetPlayerVelo(velo);


#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Move", GetColor(255, 255, 255));

#endif // _DEBUG

}
void PlayerStateMove::Exit()
{
}

void PlayerStateMove::OnCollide(std::shared_ptr<Collidable> collider)
{
}
