#include "PlayerStateMove.h"
#include "PlayerStateIdle.h"
#include "PlayerStateJump.h"
#include "PlayerStateDash.h"
#include "PlayerStateRush.h"
#include "PlayerStateNormalAttack.h"
#include "PlayerStateCharge.h"
#include "GameSceneConstant.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"
#include <cmath>

namespace
{
	//移動速度を遅くする割合
	constexpr float kSlowMoveSpeed = 0.6f;
	//さらに移動速度を遅くする距離
	constexpr float kNearestLange = 10.0f;
}

PlayerStateMove::PlayerStateMove(std::shared_ptr<Player> player) :
	PlayerStateBase(player),
	m_attackButtonPushTime(0.0f),
	m_attackKey("empty"),
	m_isFloat(false),
	m_gravityPower(0.0f),
	m_isLastGround(false)
{
	m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kSkyIdle, true);
}

void PlayerStateMove::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kMove;
	if (m_pPlayer->IsGround())
	{
		m_isFloat = false;
	}
	else
	{
		m_isFloat = true;
	}
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

		float yAngle = std::atan2f(vX, vZ);

		MyEngine::Vector3 rotation;

		rotation = MyEngine::Vector3(0.0f, yAngle, 0.0f);

		MATRIX mat = rotation.GetRotationMat();

		dir = dir.MatTransform(mat);

		MyEngine::Vector3 toTarget = (GetEnemyPos() - m_pPlayer->GetPos());
		MyEngine::Vector3 toTargetDir = toTarget.Normalize();

		//空中にいて前入力されていたら
		if (leftStickDir.Normalize().z > 0 && m_isFloat)
		{
			float frontRate = leftStickDir.Normalize().z;

			dir = (dir * (1.0 - frontRate)) + toTargetDir * frontRate;
		}

		//移動速度
		float speed = GetSpeed();

		//一定以上敵に近くなると移動速度を遅くする
		if (toTarget.Length() < GameSceneConstant::kNearLange)
		{
			speed *= kSlowMoveSpeed;
		}

		//移動方向にスピードをかける
		velo = dir * speed;


	}

	//宙に浮いていない場合
	if (!m_isFloat)
	{
		m_gravityPower += GameSceneConstant::kSkyGravityPower;

		if (m_gravityPower < GameSceneConstant::kMaxFallSpeed)
		{
			m_gravityPower = GameSceneConstant::kMaxFallSpeed;
		}

		velo.y += m_gravityPower;

		//地上にいなく
		if (!m_pPlayer->IsGround())
		{

			//プレイ中のアニメーションがジャンプ中でなければ
			if (!(m_pPlayer->GetPlayAnimKind() == CharacterBase::AnimKind::kJumping))
			{
				//アニメーションを変更する
				m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kJumping, true);
			}
		}
	}

	//ダッシュボタンが押されたら
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

				next->SetMoveDir(leftStickDir);

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

				next->SetMoveDir(leftStickDir.Normalize());

				ChangeState(next);
				return;
			}
		}
		//敵との距離が近い場合
		else
		{
			//MPを消費せずにステップをする
			auto next = std::make_shared<PlayerStateDash>(m_pPlayer);

			next->SetMoveDir(leftStickDir.Normalize());

			ChangeState(next);
			return;
		}
	}


	//地上で
	if (m_pPlayer->IsGround())
	{

		//ジャンプボタンが押されたら
		if (input.IsTrigger("RB"))
		{
			//ジャンプStateに移行する
			auto next = std::make_shared<PlayerStateJump>(m_pPlayer);

			//今の移動ベクトルを渡す
			next->StartJump(velo);

			//ステートを変更する
			ChangeState(next);

			return;
		}

		m_gravityPower = GameSceneConstant::kGroundGravityPower;

		m_isFloat = false;

		MyEngine::Vector3 groundVelo = velo;

		groundVelo.y = 0;

		//移動していれば
		if (groundVelo.SqLength() > 0.001f)
		{
			//プレイ中のアニメーションが地上移動でなければ
			if (!(m_pPlayer->GetPlayAnimKind() == CharacterBase::AnimKind::kRun))
			{
				//アニメーションを変更する
				m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kRun, true);
			}

			auto frontPos = m_pPlayer->GetPos() + groundVelo;

			frontPos.y = m_pPlayer->GetPos().y;

			m_pPlayer->SetFrontPos(frontPos);
		}
		//移動していなければ
		else
		{
			//プレイ中のアニメーションが地上待機でなければ
			if (!(m_pPlayer->GetPlayAnimKind() == CharacterBase::AnimKind::kIdle))
			{
				//アニメーションを変更する
				m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kIdle, true);
			}
		}

		//今のフレーム地上にいたかどうかを保存する
		m_isLastGround = true;

	}
	//空中で
	else
	{

		//上昇ボタンが押されたら
		if (input.IsPress("RB"))
		{
			velo.y = GetSpeed();
			m_isFloat = true;
		}
		//下降ボタンが押されたら
		else if (input.IsPushTrigger(true))
		{
			velo.y = -GetSpeed();

			//プレイ中のアニメーションがジャンプ中でなければ
			if (!(m_pPlayer->GetPlayAnimKind() == CharacterBase::AnimKind::kJumping))
			{
				//アニメーションを変更する
				m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kJumping, true);
			}

		}

		//前のフレームから空中にいれば
		if (!m_isLastGround)
		{
			//敵の方向を向く(Y座標はプレイヤーと同じ座標にする)
			auto frontPos = GetEnemyPos();

			frontPos.y = m_pPlayer->GetPos().y;

			m_pPlayer->SetFrontPos(frontPos);

			//上昇しているか上下移動をしていない場合
			if (velo.y >= 0)
			{
				//プレイ中のアニメーションが空中待機でなければ
				if (!(m_pPlayer->GetPlayAnimKind() == CharacterBase::AnimKind::kSkyIdle))
				{
					//アニメーションを変更する
					m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kSkyIdle, true);
				}
			}
		}

		//今のフレーム空中にいたことを保存する
		m_isLastGround = false;

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
			m_attackButtonPushTime > GameSceneConstant::kChargeAttackTime)
		{
			//チャージされていたかどうか判定
			bool isCharge = m_attackButtonPushTime > GameSceneConstant::kChargeAttackTime;
			//次のStateのポインタ作成
			std::shared_ptr<PlayerStateNormalAttack> next = std::make_shared<PlayerStateNormalAttack>(m_pPlayer);
			//何の攻撃を行うかをAttackStateに渡す
			next->SetAttack(m_attackKey, isCharge);
			//StateをAttackに変更する
			ChangeState(next);
			return;
		}
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
