#include "CharacterStateIdle.h"
#include "CharacterStateMove.h"
#include "CharacterStateNormalAttack.h"
#include "CharacterStateCharge.h"
#include "CharacterStateDash.h"
#include "CharacterStateRush.h"
#include "CharacterStateJump.h"
#include "CharacterStateGuard.h"
#include "CharacterBase.h"
#include "DxLib.h"
#include "Input.h"
#include "GameSceneConstant.h"

CharacterStateIdle::CharacterStateIdle(std::shared_ptr<CharacterBase> character) :
	CharacterStateBase(character),
	m_attackKey("empty"),
	m_attackButtonPushTime(0),
	m_isPlayEndAnim(false),
	m_endAnimTime(0)
{
}

void CharacterStateIdle::SetEndAnim(int kind, int time)
{
	m_pCharacter->ChangeAnim(static_cast<CharacterBase::AnimKind>(kind), false);
	m_endAnimTime = time;
	m_isPlayEndAnim = true;
}

void CharacterStateIdle::SetEndAnim(int kind, int time, float blendSpeed)
{
	m_pCharacter->ChangeAnim(static_cast<CharacterBase::AnimKind>(kind), false, blendSpeed);
	m_endAnimTime = time;
	m_isPlayEndAnim = true;
}

void CharacterStateIdle::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kIdle;
}

void CharacterStateIdle::Update()
{

#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Idle", GetColor(255, 255, 255));

#endif // _DEBUG


	if (m_isPlayer)
	{
		if (MyEngine::Input::GetInstance().IsPress("A"))
		{
			m_pCharacter->SubHp(100);
		}
	}

	//Stateにいる時間を計測する
	m_time++;

	MyEngine::Vector3 velo;

	//アニメーションが終わる時間になっていれば
	if (m_time > m_endAnimTime)
	{
		m_isPlayEndAnim = false;
		//アイドルアニメーションでなければ
		if (m_pCharacter->GetPlayAnimKind() != CharacterBase::AnimKind::kIdle)
		{
			//アイドルアニメーションに変える
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kIdle, true);
			//一応再生速度をリセットしておく
			m_pCharacter->SetAnimPlaySpeed();
		}
	}

	//前のフレームの終了アニメーションが再生されていたら下の条件分岐を通らない
	if (m_isPlayEndAnim) return;

	auto& input = MyEngine::Input::GetInstance();

	//攻撃ボタンが押されていないときに
	if (m_attackKey == "empty")
	{
		//格闘ボタンが押された時
		if (m_isPlayer && input.IsPress("X"))
		{
			m_attackKey = "X";
		}
		else if (m_isPlayer && input.IsPress("Y"))
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
		if (m_isPlayer && input.IsRelease(m_attackKey) ||
			m_attackButtonPushTime > GameSceneConstant::kChargeAttackTime)
		{
			//チャージされていたかどうか判定
			bool isCharge = m_attackButtonPushTime > GameSceneConstant::kChargeAttackTime;
			//次のStateのポインタ作成
			auto next = std::make_shared<CharacterStateNormalAttack>(m_pCharacter);

			//何の攻撃を行うかをAttackStateに渡す
			std::string attackName = "empty";

			//チャージされていて
			if (isCharge)
			{
				//Xボタンが押されていて
				if (m_attackKey == "X")
				{
					//スティックを上に傾けていたら
					if (input.GetStickInfo().leftStickY < -GameSceneConstant::kPhysicalAttackStickPower)
					{
						attackName = "UpCharge";
					}
					//スティックを下に傾けていたら
					else if (input.GetStickInfo().leftStickY > GameSceneConstant::kPhysicalAttackStickPower)
					{
						attackName = "DownCharge";
					}
					//スティックを傾けていなければ
					else
					{
						attackName = "MiddleCharge";
					}
				}
				//Yボタンが押されていたら
				else if (m_attackKey == "Y")
				{
					attackName = "EnergyCharge";
				}
			}
			//チャージされていなければ
			else
			{
				if (m_attackKey == "X")
				{
					attackName = "Low1";
				}
				else if (m_attackKey == "Y")
				{
					attackName = "Energy1";
				}
			}

			//気弾攻撃ならば気力を減らす
			if (attackName == "Energy1" || attackName == "EnergyCharge")
			{
				//減らせなければ攻撃をセットしない
				if (m_pCharacter->SubMp(GameSceneConstant::kEnergyAttackCost))
				{
					next->SetAttack(m_attackKey, attackName);

					//StateをAttackに変更する
					ChangeState(next);
					return;
				}
			}
			else
			{
				next->SetAttack(m_attackKey, attackName);


				//StateをAttackに変更する
				ChangeState(next);
				return;
			}
		}
	}

	//移動入力がされていたら
	if (m_isPlayer && input.GetStickInfo().leftStickX != 0 ||
		m_isPlayer && input.GetStickInfo().leftStickY != 0)
	{
		//次のStateのポインタ作成
		auto next = std::make_shared<CharacterStateMove>(m_pCharacter);
		//StateをMoveに変更する
		ChangeState(next);
		return;
	}

	//一定時間レフトショルダーボタンが押されたら
	if (m_isPlayer && input.GetPushTriggerTime(false) > GameSceneConstant::kChargeStateChangeTime)
	{
		//次のStateのポインタ作成
		auto next = std::make_shared<CharacterStateCharge>(m_pCharacter);
		//StateをChargeに変更する
		ChangeState(next);
		return;
	}

	//ダッシュボタンが押された時
	if (m_isPlayer && input.IsTrigger("A"))
	{
		//一緒にレフトショルダーも押されていたら
		if (m_isPlayer && input.IsPushTrigger(false))
		{
			//ダッシュのコストがあれば
			if (m_pCharacter->SubMp(GameSceneConstant::kDashCost))
			{
				//突撃状態に移行する
				auto next = std::make_shared<CharacterStateRush>(m_pCharacter);

				next->SetMoveDir(MyEngine::Vector3(0.0f, 0.0f, 1.0f));

				ChangeState(next);
				return;
			}
		}

		//敵との距離からダッシュかステップか判断する
		//(ステップかダッシュかの判定はDashStateの中でも行う)
		//(ここではMPを消費するかしないか、DashStateにはいるかどうかを判断する)
		if ((GetTargetPos() - m_pCharacter->GetPos()).Length() > GameSceneConstant::kNearLange)
		{
			//遠かった場合Mpを消費してダッシュする
			if (m_pCharacter->SubMp(GameSceneConstant::kDashCost))
			{
				auto next = std::make_shared<CharacterStateDash>(m_pCharacter);

				next->SetMoveDir(MyEngine::Vector3(0.0f, 0.0f, 1.0f));

				ChangeState(next);
				return;
			}
		}
		//敵との距離が近い場合
		else
		{
			//MPを消費せずにステップをする
			auto next = std::make_shared<CharacterStateDash>(m_pCharacter);

			next->SetMoveDir(MyEngine::Vector3(0.0f, 0.0f, 1.0f));

			ChangeState(next);
			return;
		}
	}

	//地上にいるときに
	if (m_pCharacter->IsGround())
	{
		//ジャンプボタンが押されたら
		if (m_isPlayer && input.IsPress("RB"))
		{
			//ジャンプState作成
			auto next = std::make_shared<CharacterStateJump>(m_pCharacter);

			next->StartJump();

			ChangeState(next);

			return;
		}

		//重力をかけておく
		velo.y += GameSceneConstant::kGroundGravityPower;

	}
	//空中にいるときに
	else
	{
		//上昇ボタンか下降ボタンが押されたら
		if (m_isPlayer && input.IsPress("RB") ||
			m_isPlayer && input.IsPushTrigger(true))
		{
			//次のStateのポインタ作成
			auto next = std::make_shared<CharacterStateMove>(m_pCharacter);
			//StateをMoveに変更する
			ChangeState(next);
			return;
		}
	}

	//ガード入力がされていたら
	if (m_isPlayer && input.IsPress("B"))
	{
		//次のStateのポインタ作成
		auto next = std::make_shared<CharacterStateGuard>(m_pCharacter);
		//StateをMoveに変更する
		ChangeState(next);
		return;
	}

	if (!m_isPlayer)
	{
		a += 0.1f;

		//velo.y = sin(a) * m_pCharacter->GetSpeed() * 2;
	}

	//アイドル状態の時は移動しない
	SetCharacterVelo(velo);
}
void CharacterStateIdle::Exit()
{

}