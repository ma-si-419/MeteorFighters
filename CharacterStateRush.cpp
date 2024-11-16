#include "CharacterStateRush.h"
#include "CharacterStateIdle.h"
#include "CharacterStateDash.h"
#include "CharacterStateNormalAttack.h"
#include "DxLib.h"
#include "Input.h"
#include "CharacterBase.h"
#include "GameSceneConstant.h"
#include <cmath>

namespace
{
	//スティック操作がされていないとき上下移動
	constexpr float kMoveVerticalPower = 0.03f;

	//上下移動の角度の最大
	constexpr float kVerticalMoveMax = 0.5f;
	//上下移動の角度の最小
	constexpr float kVerticalMoveMin = -0.5f;

	//1フレームでどのくらい行く方向を回転できるか
	constexpr float kRotScaleMax = 10.0f;

	//ドラゴンダッシュ時にカーブする距離
	constexpr float kCurveDistance = 300.0f;

	//アイドルに戻るときのアニメーションブレンドの速さ
	constexpr float kEndAnimBlendSpeed = 0.08f;

	//アイドルに戻るときのアニメーションの再生速度
	constexpr float kEndAnimPlaySpeed = 0.5f;

	//アイドルに戻るときの硬直時間
	constexpr int kEndLandingTime = 20;

	//アイドルに戻るときにカメラを揺らす時間
	constexpr int kCameraShakeTime = 3;

	//最初のスタートモーションの時間
	constexpr float kStartAnimTime = 8.0f;
}

CharacterStateRush::CharacterStateRush(std::shared_ptr<CharacterBase> character) :
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
	float vX = GetTargetPos().x - m_pCharacter->GetPos().x;
	float vZ = GetTargetPos().z - m_pCharacter->GetPos().z;

	float yAngle = std::atan2f(vX, vZ);

	MyEngine::Vector3 rotation;

	rotation = MyEngine::Vector3(0.0f, yAngle, 0.0f);

	MATRIX mat = rotation.GetRotationMat();

	m_moveDir = dir.MatTransform(mat);

	//前入力は敵に向かっていくが後ろ入力はy座標を計算に入れないのでその計算
	if (dir.z > 0)
	{
		MyEngine::Vector3 toTargetDir = (GetTargetPos() - m_pCharacter->GetPos()).Normalize();

		m_moveDir = m_moveDir * (1.0 - dir.z) + toTargetDir * dir.z;
	}

	m_moveTarget.SetCenterPos(m_pCharacter->GetPos());
	m_moveTarget.SetLocalPos(dir.MatTransform(mat));
	m_moveTarget.SetFrontPos(GetTargetPos());
}

void CharacterStateRush::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kRush;
	m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kRushStart, true);
}

void CharacterStateRush::Update()
{
	m_time++;

	auto& input = MyEngine::Input::GetInstance();

	//スティックの傾き
	MyEngine::Vector3 stickDir;

	if (m_isPlayer)
	{
		stickDir = MyEngine::Vector3(input.GetStickInfo().leftStickX, 0, -input.GetStickInfo().leftStickY);
	}

	////////

	//ラッシュ終了時の処理
	if (m_isEndRush)
	{
		m_rushEndTime++;

		if (m_pCharacter->GetPlayAnimKind() != CharacterBase::AnimKind::kRushEnd)
		{
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kRushEnd, false, kEndAnimBlendSpeed);

			m_pCharacter->SetAnimPlaySpeed(kEndAnimPlaySpeed);
		}
		//攻撃入力がされていたら攻撃状態に移行する
		if (m_isAttackInput)
		{
			auto next = std::make_shared<CharacterStateNormalAttack>(m_pCharacter);

			next->SetAttack("X", "DashAttack");

			//敵の方向を向く
			m_pCharacter->SetFrontPos(GetTargetPos());

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
	if (m_isPlayer && input.IsTrigger("A"))
	{
		//レフトショルダーも押されていたら
		if (m_isPlayer && input.IsPushTrigger(false))
		{
			//敵の近くまで向かう突撃状態になる
			m_isRushEnemy = true;
			m_rushTargetPos = GetTargetBackPos(GameSceneConstant::kEnemyBackPosDistance);
		}
		//押されていなければ
		else
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

				ShakeCamera(kCameraShakeTime);
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
	if (m_isPlayer && input.IsPress("RB"))
	{
		dir.y += kMoveVerticalPower;
		m_moveDir.y += kMoveVerticalPower * speed;
	}
	if (m_isPlayer && input.IsPushTrigger(true))
	{
		dir.y -= kMoveVerticalPower;
		m_moveDir.y -= kMoveVerticalPower * speed;
	}

	//移動ベクトルが上にも下にも傾きすぎないようにクランプ
	m_moveDir.y = std::fmax(m_moveDir.y, kVerticalMoveMin);
	m_moveDir.y = std::fmin(m_moveDir.y, kVerticalMoveMax);

	//スティックを傾けた時か上昇、下降ボタンを押したときのみ行きたい方向を変更する
	if (stickDir.SqLength() > 0.001f)
	{
		stickDir = stickDir.Normalize();

		//エネミーの方向に移動方向を回転させる
		float vX = GetTargetPos().x - m_pCharacter->GetPos().x;
		float vZ = GetTargetPos().z - m_pCharacter->GetPos().z;

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
			MyEngine::Vector3 toTargetDir = (GetTargetPos() - m_pCharacter->GetPos()).Normalize();

			dir = dir * (1.0 - stickDir.z) + toTargetDir * stickDir.z;
		}

		m_moveDir = dir.Normalize();
	}


	//開始アニメーションを再生して一定時間たって
	if (m_time > kStartAnimTime)
	{
		//まだ開始アニメーションを再生していたら
		if (m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kRushStart)
		{
			ShakeCamera(kCameraShakeTime);
			//アニメーションを変更する
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kSkyDash, true);
		}
	}
	//まだ開始アニメーションを再生していたら
	else
	{
		//移動しようとしている方向に体を向ける
		m_pCharacter->SetFrontPos(m_moveDir + m_pCharacter->GetPos());

		SetCharacterVelo(MyEngine::Vector3(0,0,0));

		//移動処理を行わない
		return;
	}


	////行きたい方向を設定する
	//m_moveTarget.SetCenterPos(m_pCharacter->GetPos());
	//m_moveTarget.SetLocalPos(dir);
	//m_moveTarget.SetFrontPos(GetTargetPos());

	////行きたい座標の回転度を調べる
	//float toMoveTargetRotX = m_moveTarget.GetWorldPos().x - m_pCharacter->GetPos().x;
	//float toMoveTargetRotY = m_moveTarget.GetWorldPos().y - m_pCharacter->GetPos().y;
	//float toMoveTargetRotZ = m_moveTarget.GetWorldPos().z - m_pCharacter->GetPos().z;

	////y回転
	//float targetYAngle = std::atan2f(toMoveTargetRotX, toMoveTargetRotX);

	////今向かっている方向の回転度を調べる
	//float moveVecRotX = m_moveDir.x;
	//float moveVecRotY = m_moveDir.y;
	//float moveVecRotZ = m_moveDir.z;

	//float moveYAngle = std::atan2f(moveVecRotX, moveVecRotZ);

	////回転の差を求める
	//float difY = targetYAngle - moveYAngle;

	////回転する大きさを設定
	//float rotY = std::fmin(difY, kRotScaleMax);

	//MyEngine::Vector3 rotation(0.0f, rotY, 0.0f);

	////回転
	//MATRIX mat = rotation.GetRotationMat();

	////移動ベクトルを回転させる
	//m_moveDir = m_moveDir.MatTransform(mat);

	//printfDx("moveY : %0.3f,tarY : % 0.3f\n", moveVecRotX, toMoveTargetRotX);
	//printfDx("X:%.2f,Y:%.2f,Z:%.2f\n", dir.x, dir.y, dir.z);

	MyEngine::Vector3 velo = m_moveDir * speed;


	//敵の背後に向かうフラグが立っていれば
	if (m_isRushEnemy)
	{
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
		if ((GetTargetBackPos(GameSceneConstant::kEnemyBackPosDistance) - m_pCharacter->GetPos()).Length() > GameSceneConstant::kCameraMoveDistance)
		{
			m_rushTargetPos = GetTargetBackPos(GameSceneConstant::kEnemyBackPosDistance);
			StopMoveCamera();
		}
		//一定距離まで近づいたら
		else
		{
			StartMoveCamera();

			//カメラを高速移動させる
			m_pCharacter->StartFastCameraMove();

			//さらに近くまで近づいたら
			if (toTarget.Length() < GameSceneConstant::kEndRushDistance)
			{
				//障害物に当たるようにする
				m_pCharacter->SetIsTrigger(false);

				//敵の方向を向く
				MyEngine::Vector3 nextPos = m_pCharacter->GetPos() + velo;

				MyEngine::Vector3 nextToTarget = GetTargetPos() - nextPos;

				m_pCharacter->SetFrontPos(nextToTarget + m_pCharacter->GetPos());

				ShakeCamera(kCameraShakeTime);

				m_isEndRush = true;
			}
		}
	}
	//通常のラッシュ時は
	else
	{
		//次の移動座標がエネミーとぶつかるのならばラッシュをやめる
		MyEngine::Vector3 nextPos = m_pCharacter->GetPos() + velo;

		float lange = (GetTargetPos() - nextPos).Length();

		//キャラクターの半径二体分よりも距離が近ければ
		if (lange < GameSceneConstant::kCharacterRadius * 2)
		{
			m_isEndRush = true;
		}

		//攻撃入力がされたらすぐに攻撃に移る
		if (m_isPlayer && input.IsTrigger("X"))
		{
			auto next = std::make_shared<CharacterStateNormalAttack>(m_pCharacter);

			next->SetAttack("X", "MiddleCharge");
			next->SetAttackVelo(m_moveDir * 3.0f);

			//移動方向を見る
			m_pCharacter->SetFrontPos(m_moveDir + m_pCharacter->GetPos());

			ChangeState(next);
		}
	}

	SetCharacterVelo(velo);
	//ラッシュを終わる処理をしていなければ移動方向を見る
	if (!m_isEndRush)
	{
		m_pCharacter->SetFrontPos(m_pCharacter->GetPos() + velo);
	}

#ifdef _DEBUG

	DrawSphere3D(m_rushTargetPos.CastVECTOR(), 3, 5, GetColor(255, 0, 255), GetColor(255, 0, 255), true);

	DrawString(0, 16, "PlayerState:Rush", GetColor(255, 255, 255));

#endif // _DEBUG

}
void CharacterStateRush::Exit()
{
}
