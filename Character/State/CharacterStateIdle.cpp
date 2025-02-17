#include "CharacterStateIdle.h"
#include "CharacterStateMove.h"
#include "CharacterStateNormalAttack.h"
#include "CharacterStateCharge.h"
#include "CharacterStateDash.h"
#include "CharacterStateRush.h"
#include "CharacterStateJump.h"
#include "CharacterStateGuard.h"
#include "Character.h"
#include "GameManagerBase.h"
#include "DxLib.h"
#include "Input.h"
#include "GameSceneConstant.h"

namespace
{
	//空中にいるときに揺らす速度
	constexpr float kSwaySpeed = 0.1f;

	//空中にいるときに揺らす幅
	constexpr float kSwayWidth = 0.5f;
}

CharacterStateIdle::CharacterStateIdle(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
	m_attackKey("empty"),
	m_attackButtonPushTime(0),
	m_isPlayEndAnim(false),
	m_endAnimTime(0),
	m_isReleaseX(true),
	m_isReleaseY(true),
	m_swayCount(0)
{
}

void CharacterStateIdle::SetEndAnim(int kind, int time)
{
	m_pCharacter->ChangeAnim(static_cast<Character::AnimKind>(kind), false);
	m_endAnimTime = time;
	m_isPlayEndAnim = true;
}

void CharacterStateIdle::SetEndAnim(int kind, int time, float blendSpeed)
{
	m_pCharacter->ChangeAnim(static_cast<Character::AnimKind>(kind), false, blendSpeed);
	m_endAnimTime = time;
	m_isPlayEndAnim = true;
}

void CharacterStateIdle::SetLastAttackState()
{
	m_attackKey = "empty";
	m_attackButtonPushTime = 0;
	m_isReleaseX = false;
	m_isReleaseY = false;
}

void CharacterStateIdle::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kIdle;
	m_pCharacter->SetHitReaction(Character::HitReactionKind::kNone);
}

void CharacterStateIdle::Update()
{
	auto input = GetCharacterInput();

#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Idle", GetColor(255, 255, 255));

#endif // _DEBUG

	//Stateにいる時間を計測する
	m_time++;

	MyEngine::Vector3 velo;

	//アニメーションが終わる時間になっていれば
	if (m_isPlayEndAnim)
	{

		//前のStateのアニメーションが終了していたら
		if (m_pCharacter->IsEndAnim())
		{
			m_isPlayEndAnim = false;
		}
	}
	else
	{
		//地上にいる場合
		if (m_pCharacter->IsGround())
		{
			//地上待機アニメーションでなければ
			if (m_pCharacter->GetPlayAnimKind() != Character::AnimKind::kIdle)
			{
				//地上待機アニメーションに変える
				m_pCharacter->ChangeAnim(Character::AnimKind::kIdle, true);
				//一応再生速度をリセットしておく
				m_pCharacter->SetAnimPlaySpeed();
			}
		}
		//空中にいる場合
		else
		{
			//アニメーションが空中待機でなければ
			if (m_pCharacter->GetPlayAnimKind() != Character::AnimKind::kSkyIdle)
			{
				//空中待機アニメーションに変更
				m_pCharacter->ChangeAnim(Character::AnimKind::kSkyIdle, true);
				//一応再生速度をリセットしておく
				m_pCharacter->SetAnimPlaySpeed();
			}

			//空中にいるときに揺らす
			m_swayCount += kSwaySpeed;

			//sin波で揺らす
			MyEngine::Vector3 shiftVec = MyEngine::Vector3(0.0f, sinf(m_swayCount) * kSwayWidth, 0.0f);

			//座標を設定
			m_pCharacter->SetDrawShiftVec(shiftVec);
		}
	}

	//前のStateの後隙が終わっていなければ
	if (m_time < m_endAnimTime) return;

	//一度ボタンが離されたかどうか
	if (!input->IsPress("X"))
	{
		m_isReleaseX = true;
	}
	if (!input->IsPress("Y"))
	{
		m_isReleaseY = true;
	}


	//攻撃ボタンが押されていないときに
	if (m_attackKey == "empty")
	{
		//格闘ボタンが押された時
		if (input->IsPress("X") && m_isReleaseX)
		{
			m_attackKey = "X";
		}
		else if (input->IsPress("Y") && m_isReleaseY)
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
		if (input->IsRelease(m_attackKey) ||
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
				if (m_attackKey == "X" && m_isReleaseX)
				{
					//スティックを上に傾けていたら
					if (input->GetStickInfo().leftStickY < -GameSceneConstant::kPhysicalAttackStickPower)
					{
						attackName = "UpCharge";
					}
					//スティックを下に傾けていたら
					else if (input->GetStickInfo().leftStickY > GameSceneConstant::kPhysicalAttackStickPower)
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
				else if (m_attackKey == "Y" && m_isReleaseY)
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

			//攻撃がセットされていたら
			if (attackName != "empty")
			{
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
	}

	//移動入力がされていたら
	if (input->GetStickInfo().leftStickX != 0 ||
		input->GetStickInfo().leftStickY != 0)
	{
		//次のStateのポインタ作成
		auto next = std::make_shared<CharacterStateMove>(m_pCharacter);
		//StateをMoveに変更する
		ChangeState(next);
		return;
	}

	//一定時間レフトショルダーボタンが押されたら
	if (input->GetPushTriggerTime(false) > GameSceneConstant::kChargeStateChangeTime)
	{
		//次のStateのポインタ作成
		auto next = std::make_shared<CharacterStateCharge>(m_pCharacter);
		//StateをChargeに変更する
		ChangeState(next);
		return;
	}

	//ダッシュボタンが押された時
	if (input->IsTrigger("A"))
	{
		//一緒にレフトショルダーも押されていたら
		if (input->IsPushTrigger(false))
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
		if ((m_pManager->GetTargetPos(m_pCharacter) - m_pCharacter->GetPos()).Length() > GameSceneConstant::kNearLange)
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
		if (input->IsPress("RB"))
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
		if (input->IsPress("RB") ||
			input->IsPushTrigger(true))
		{
			//次のStateのポインタ作成
			auto next = std::make_shared<CharacterStateMove>(m_pCharacter);
			//StateをMoveに変更する
			ChangeState(next);
			return;
		}
	}

	//ガード入力がされていたら
	if (input->IsPress("B"))
	{
		//次のStateのポインタ作成
		auto next = std::make_shared<CharacterStateGuard>(m_pCharacter);
		//StateをMoveに変更する
		ChangeState(next);
		return;
	}

	//アイドル状態の時は移動しない
	SetCharacterVelo(velo);
}
void CharacterStateIdle::Exit()
{

}