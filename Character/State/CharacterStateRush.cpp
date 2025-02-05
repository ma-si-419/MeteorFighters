#include "CharacterStateRush.h"
#include "CharacterStateIdle.h"
#include "CharacterStateDash.h"
#include "CharacterStateNormalAttack.h"
#include "CharacterStateButtonBashing.h"
#include "DxLib.h"
#include "Input.h"
#include "Character.h"
#include "GameSceneConstant.h"
#include "Effect.h"
#include "GameManagerBase.h"
#include "TutorialManager.h"
#include <cmath>
#include "SoundManager.h"

namespace
{
	//スティック操作がされていないとき上下移動
	constexpr float kMoveVerticalPower = 0.05f;

	//上下移動の角度の最大
	constexpr float kVerticalMoveMax = 0.5f;
	//上下移動の角度の最小
	constexpr float kVerticalMoveMin = -0.5f;

	//1フレームでどのくらい行く方向を回転できるか
	constexpr float kRotScaleMax = 10.0f;

	//ロケットダッシュ時にカーブする距離
	constexpr float kCurveDistance = 300.0f;

	//エネミーに向かっていくダッシュで使用する気力量
	constexpr float kEnemyRushCost = 10000.0f;

	//移動中にどのくらい気力を使用するか
	constexpr float kRushCost = 50.0f;

	//アイドルに戻るときのアニメーションブレンドの速さ
	constexpr float kEndAnimBlendSpeed = 0.08f;

	//アイドルに戻るときのアニメーションの再生速度
	constexpr float kEndAnimPlaySpeed = 0.6f;

	//アイドルに戻るときの硬直時間
	constexpr int kEndLandingTime = 20;

	//アイドルに戻るときにカメラを揺らす時間
	constexpr int kCameraShakeTime = 3;

	//最初のスタートモーションの時間
	constexpr float kStartAnimTime = 8.0f;

	//線形補間の速さ
	constexpr float kEasingSpeed = 0.08f;

	//突撃を行う最低時間
	constexpr int kRushTime = 20;

	//移動中に真反対を入力されたと判断するベクトルの差の大きさ
	constexpr float kReverseVecScale = 1.999f; 

	//音声を再生する間隔
	constexpr int kSoundInterval = 60;
}

CharacterStateRush::CharacterStateRush(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
	m_isRushEnemy(false),
	m_isEndRush(false),
	m_rushEndTime(0),
	m_isAttackInput(false)
{
}

void CharacterStateRush::SetMoveDir(MyEngine::Vector3 dir)
{
	//エネミーの方向に移動方向を回転させる
	float vX = m_pManager->GetTargetPos(m_pCharacter).x - m_pCharacter->GetPos().x;
	float vZ = m_pManager->GetTargetPos(m_pCharacter).z - m_pCharacter->GetPos().z;

	float yAngle = std::atan2f(vX, vZ);

	MyEngine::Vector3 rotation;

	rotation = MyEngine::Vector3(0.0f, yAngle, 0.0f);

	MATRIX mat = rotation.GetRotationMat();

	m_moveDir = dir.MatTransform(mat);

	//前入力は敵に向かっていくが後ろ入力はy座標を計算に入れないのでその計算
	if (dir.z > 0)
	{
		MyEngine::Vector3 toTargetDir = (m_pManager->GetTargetPos(m_pCharacter) - m_pCharacter->GetPos()).Normalize();

		m_moveDir = m_moveDir * (1.0f - dir.z) + toTargetDir * dir.z;
	}

}

void CharacterStateRush::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kRush;
	m_pCharacter->ChangeAnim(Character::AnimKind::kRushStart, true);

	//スーパーダッシュのチュートリアルをクリアする
	SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kSuperDash));

	//スーパーダッシュ開始のサウンドを再生する
	SoundManager::GetInstance().PlayOnceSound("StartSuperDash");
}

void CharacterStateRush::Update()
{
	m_time++;

	//サウンドを一定間隔で再生する
	if (static_cast<int>(m_time) % kSoundInterval == 0)
	{
		SoundManager::GetInstance().PlayOnceSound("OnSuperDash");
	}

	//通常時で気力が足りなければ
	if (!m_pCharacter->SubMp(kRushCost) && !m_isRushEnemy)
	{
		//ラッシュを終了する
		m_isEndRush = true;
	}

	auto input = GetCharacterInput();

	//スティックの傾き
	MyEngine::Vector3 stickDir;

	stickDir = MyEngine::Vector3(input->GetStickInfo().leftStickX, 0, -input->GetStickInfo().leftStickY);

	////////

	//ラッシュ終了時の処理
	if (m_isEndRush)
	{
		m_rushEndTime++;

		//ラッシュ終了アニメーションになっていなければ
		if (m_pCharacter->GetPlayAnimKind() != Character::AnimKind::kRushEnd)
		{
			//ラッシュ終了アニメーションになる
			m_pCharacter->ChangeAnim(Character::AnimKind::kRushEnd, false, kEndAnimBlendSpeed);

			//アニメーションの再生速度を変更する
			m_pCharacter->SetAnimPlaySpeed(kEndAnimPlaySpeed);

			//エフェクトを変更する
			m_pManager->ExitEffect(m_pEffect);

			m_pEffect = std::make_shared<Effect>(Effect::EffectKind::kDashEnd);

			m_pEffect->SetPos(m_pCharacter->GetFrontPos());

			//回転を設定する
			MyEngine::Vector3 rotation;

			//敵に向かう入力がされていた時
			if (m_isRushEnemy)
			{
				MyEngine::Vector3 targetPos = m_pManager->GetTargetPos(m_pCharacter);
				MyEngine::Vector3 pos = m_pCharacter->GetPos();

				float vX = targetPos.x - pos.x;
				float vZ = targetPos.z - pos.z;

				rotation.y = std::atan2f(vX, vZ);
			}
			//敵に向かう入力がされていなかったとき
			else
			{
				MyEngine::Vector3 nextPos = m_pCharacter->GetPos() + m_moveDir;
				MyEngine::Vector3 pos = m_pCharacter->GetPos();

				float vX = nextPos.x - pos.x;
				float vZ = nextPos.z - pos.z;

				rotation.y = std::atan2f(vX, vZ);
			}

			m_pEffect->SetRotation(rotation);

			m_pManager->EntryEffect(m_pEffect);

		}
		//攻撃入力がされていたら攻撃状態に移行する
		if (m_isAttackInput)
		{
			auto next = std::make_shared<CharacterStateNormalAttack>(m_pCharacter);

			next->SetAttack("X", "DashAttack");

			//敵の方向を向く
			m_pCharacter->SetFrontPos(m_pManager->GetTargetPos(m_pCharacter));

			ChangeState(next);
		}

		//ラッシュが終了してから一定時間たったら
		if (m_rushEndTime > kEndLandingTime)
		{
			//アイドル状態に戻る
			auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

			ChangeState(next);

			return;
		}


		SetCharacterVelo(MyEngine::Vector3(0.0f, 0.0f, 0.0f));
		return;//下の処理をしないために
	}

	////////



	////////

	//ラッシュを行うときの処理

	//Aボタンが押されたときに
	if (input->IsTrigger("A"))
	{
		//レフトショルダーも押されていたら
		if (input->IsPushTrigger(false))
		{
			//敵の近くまで向かう突撃状態じゃなければ
			if (!m_isRushEnemy)
			{
				//気力が足りた場合のみ
				if (m_pCharacter->SubMp(kEnemyRushCost))
				{
					//サウンドを再生する
					SoundManager::GetInstance().PlayOnceSound("StartRocketDash");

					//敵の近くまで向かう突撃状態になる
					m_isRushEnemy = true;
					m_rushTargetPos = m_pManager->GetTargetBackPos(GameSceneConstant::kEnemyBackPosDistance, m_pCharacter);
				}
			}
		}
		//押されていないとき
		else
		{
			//敵の近くまで向かう突撃状態でなければ
			if (!m_isRushEnemy)
			{
				//スティックの傾きを見る
				if (stickDir.SqLength() > 0.001f)
				{
					//ダッシュ状態に移行する
					auto next = std::make_shared<CharacterStateDash>(m_pCharacter);

					next->SetMoveDir(stickDir.Normalize());

					ChangeState(next);

					return;
				}
				//スティックが傾いていなければ
				else
				{
					m_isEndRush = true;

					m_pManager->ShakeCamera(kCameraShakeTime);
				}
			}
		}
	}

	////////


	////////

	//移動処理

	float speed = m_pCharacter->GetSpeed() * GameSceneConstant::kRushMoveSpeedRate;

	//行きたい方向
	MyEngine::Vector3 dir;

	//上下移動処理
	if (input->IsPress("RB"))
	{
		dir.y += kMoveVerticalPower * speed;
	}
	if (input->IsPushTrigger(true))
	{
		dir.y -= kMoveVerticalPower * speed;
	}

	//スティックを傾けた時か上昇、下降ボタンを押したときのみ行きたい方向を変更する
	if (stickDir.SqLength() > 0.001f)
	{
		stickDir = stickDir.Normalize();
		//エネミーの方向に移動方向を回転させる
		float vX = m_pManager->GetTargetPos(m_pCharacter).x - m_pCharacter->GetPos().x;
		float vZ = m_pManager->GetTargetPos(m_pCharacter).z - m_pCharacter->GetPos().z;

		float yAngle = std::atan2f(vX, vZ);

		MyEngine::Vector3 rotation;

		rotation = MyEngine::Vector3(0.0f, yAngle, 0.0f);

		//回転行列(y成分のみ)
		MATRIX mat = rotation.GetRotationMat();

		//移動方向を設定
		dir += stickDir.MatTransform(mat);

		//前入力は敵に向かっていくが後ろ入力はy座標を計算に入れないのでその計算
		if (stickDir.z > 0)
		{
			MyEngine::Vector3 toTargetDir = (m_pManager->GetTargetPos(m_pCharacter) - m_pCharacter->GetPos()).Normalize();

			dir = dir * (1.0f - stickDir.z) + toTargetDir * stickDir.z;
		}
	}
	if (dir.SqLength() > 0.1f)
	{
		m_moveTarget = dir.Normalize();
	}

	//開始アニメーションを再生して一定時間たって
	if (m_time > kStartAnimTime)
	{
		//まだ開始アニメーションを再生していたら
		if (m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kRushStart)
		{
			m_pManager->ShakeCamera(kCameraShakeTime);
			//アニメーションを変更する
			m_pCharacter->ChangeAnim(Character::AnimKind::kSkyDash, true);
			//エフェクトを再生する
			m_pEffect = std::make_shared<Effect>(Effect::EffectKind::kDash);
			m_pEffect->SetPos(m_pCharacter->GetPos());
			m_pEffect->SetLoop(30, 32);
			m_pManager->EntryEffect(m_pEffect);
		}
	}
	//まだ開始アニメーションを再生していたら
	else
	{
		//移動しようとしている方向に体を向ける
		m_pCharacter->SetFrontPos(m_moveDir + m_pCharacter->GetPos());

		SetCharacterVelo(MyEngine::Vector3(0, 0, 0));

		//移動処理を行わない
		return;
	}

	//少しずつスティックを傾けている方向に移動方向を変更する
	if (m_moveDir.SqLength() > 0.1f && m_moveTarget.SqLength() > 0.1f)
	{
		m_moveDir = m_moveDir * (1.0f - kEasingSpeed) + m_moveTarget * kEasingSpeed;

		MyEngine::Vector3 checkVecA = m_moveDir;
		checkVecA.y = 0;
		checkVecA = checkVecA.Normalize();

		MyEngine::Vector3 checkVecB = m_moveTarget;
		checkVecB.y = 0;
		checkVecB = checkVecB.Normalize();

		if (std::fabs((checkVecA - checkVecB).Length()) > kReverseVecScale && m_time > kRushTime)
		{
			//敵に自動で向かっていく処理をしていなければ
			if (!m_isRushEnemy)
			{
				m_isEndRush = true;
			}
		}
		m_moveDir = m_moveDir.Normalize();
	}
	else if (m_moveTarget.SqLength() > 0.1f)
	{
		m_moveDir = m_moveTarget.Normalize();
	}



	MyEngine::Vector3 velo = m_moveDir * speed;


	//敵の背後に向かうフラグが立っていれば
	if (m_isRushEnemy)
	{
		//ロケットダッシュのチュートリアルをクリアする
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kRocketDash));

		//もし敵がRushStateであれば
		if (static_cast<CharacterStateKind>(m_pManager->GetTargetState(m_pCharacter)) == CharacterStateKind::kRush)
		{
			//ボタン連打対決に移行する
			auto next = std::make_shared<CharacterStateButtonBashing>(m_pCharacter);

			ChangeState(next);

			return;
		}

		//障害物に当たらないようにする
		m_pCharacter->SetIsTrigger(true);

		MyEngine::Vector3 toTarget = m_rushTargetPos - m_pCharacter->GetPos();

		m_moveDir = toTarget.Normalize();

		//一定距離から移動方向を変えるため、距離の割合をとる
		float langeRate = toTarget.Length() / kCurveDistance;
		//カーブの割合
		float curveRate = 0.0f;

		//一定距離まで近づいたら
		if (langeRate < 1.0f)
		{
			//0から1の間で-1から+1にするために*2-1をする
			curveRate = std::sinf(langeRate * 2.0f - 1.0f);

			MyEngine::Vector3 rotation(0.0f, curveRate, 0.0f);

			MATRIX mat = rotation.GetRotationMat();

			m_moveDir = m_moveDir.MatTransform(mat);
		}

		if (toTarget.Length() > GameSceneConstant::kRushSpeed)
		{
			velo = m_moveDir * GameSceneConstant::kRushSpeed;
		}
		else
		{
			velo = toTarget;
		}

		//一定距離までは目的座標を更新し続ける
		if ((m_pManager->GetTargetBackPos(GameSceneConstant::kEnemyBackPosDistance, m_pCharacter) - m_pCharacter->GetPos()).Length() > GameSceneConstant::kCameraMoveDistance)
		{
			m_rushTargetPos = m_pManager->GetTargetBackPos(GameSceneConstant::kEnemyBackPosDistance, m_pCharacter);

			//1Pのみカメラを動かす
			if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
			{
				m_pManager->StopCameraCorrection();
			}
		}
		//一定距離まで近づいたら
		else
		{
			m_pManager->StartCameraCorrection();

			//1Pのみカメラを動かす
			if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
			{
				//カメラを高速移動させる
				m_pCharacter->StartFastCameraMove();
			}
			//さらに近くまで近づいたら
			if (toTarget.Length() < GameSceneConstant::kEndRushDistance)
			{
				//障害物に当たるようにする
				m_pCharacter->SetIsTrigger(false);

				//敵の方向を向く
				MyEngine::Vector3 nextPos = m_pCharacter->GetPos() + velo;

				MyEngine::Vector3 nextToTarget = m_pManager->GetTargetPos(m_pCharacter) - nextPos;

				m_pCharacter->SetFrontPos(nextToTarget + m_pCharacter->GetPos());

				m_pManager->ShakeCamera(kCameraShakeTime);

				m_isEndRush = true;
			}
		}
	}
	//通常のラッシュ時は
	else
	{
		//次の移動座標がエネミーとぶつかるのならばラッシュをやめる
		MyEngine::Vector3 nextPos = m_pCharacter->GetPos() + velo;

		float lange = (m_pManager->GetTargetPos(m_pCharacter) - nextPos).Length();

		//キャラクターの半径二体分よりも距離が近ければ
		if (lange < GameSceneConstant::kCharacterRadius * 2)
		{
			m_isEndRush = true;
		}

		//攻撃入力がされたらすぐに攻撃に移る
		if (input->IsTrigger("X"))
		{
			auto next = std::make_shared<CharacterStateNormalAttack>(m_pCharacter);

			next->SetAttack("X", "MiddleCharge");
			next->SetAttackVelo(m_moveDir * 3.0f);

			//移動方向を見る
			m_pCharacter->SetFrontPos(m_moveDir + m_pCharacter->GetPos());

			ChangeState(next);
		}
	}

	//エフェクトに回転と座標を設定する
	MyEngine::Vector3 rotation;

	MyEngine::Vector3 nextPos = m_pCharacter->GetPos() + velo;
	MyEngine::Vector3 pos = m_pCharacter->GetPos();

	float vX = nextPos.x - pos.x;
	float vZ = nextPos.z - pos.z;

	rotation.y = std::atan2f(vX, vZ);

	m_pEffect->SetPos(pos);
	m_pEffect->SetRotation(rotation);

	SetCharacterVelo(velo);
	//ラッシュを終わる処理をしていなければ
	if (!m_isEndRush)
	{
		//1Pのみカメラを動かす
		if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
		{
			//カメラを少し揺らす
			m_pManager->SwayCamera();
		}
		//移動方向を見る
		m_pCharacter->SetFrontPos(m_pCharacter->GetPos() + velo);
	}
}
void CharacterStateRush::Exit()
{
	m_pManager->ExitEffect(m_pEffect);

	//サウンドを止める
	SoundManager::GetInstance().StopSound("OnSuperDash");
}
