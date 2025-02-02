#include "GameCamera.h"
#include <cmath>

namespace
{
	const MyEngine::Vector3 kPlayerToCameraInitVec(17, 15, -30);

	constexpr float kCameraNear = 10.0f;
	constexpr float kCameraFar = 10000.0f;

	constexpr float kCameraToPlayerLange = 50.0f;

	constexpr float kOnMoveMaxLocalPosX = 42.0f;//左幅(大きいほど左に広がる)
	constexpr float kOnMoveMaxLocalPosY = 18.0f;//下幅(大きいほど下に広がる)
	constexpr float kOnMoveMaxLocalPosZ = -30.0f;//手前幅(大きいほどプレイヤーが手前に来る)

	constexpr float kOnMoveMinLocalPosX = -42.0f;//右幅(小さいほど右に広がる)
	constexpr float kOnMoveMinLocalPosY = -12.0f;//上幅(小さいほど上に広がる)
	constexpr float kOnMoveMinLocalPosZ = -35.0f;//奥幅(小さいほどプレイヤーが奥に行く)


	constexpr float kLocalInitPosX = 27.0f;
	constexpr float kLocalInitTopPosY = -2.0f;
	constexpr float kLocalInitUnderPosY = 8.0f;
	constexpr float kLocalInitPosZ = -30.0f;

	//プレイヤーとターゲットの間でどの当たりにカメラの焦点を合わせるか
	constexpr float kCameraTargetPosRate = 0.4f;

	//ローカル座標の補正にかける時間
	constexpr int kLocalPosMoveTime = 12;

	//ローカル座標の補正を始める時間
	constexpr int kLocalPosMoveStartTime = 5;

	//距離によって横移動のあそびを減らす割合
	constexpr float kLangeSubRate = 0.1f;

	//前のフレームのカメラ座標と次のフレームのカメラ座標の距離が遠いと判定する距離
	constexpr float kCameraFarLange = 100.0f;

	//カメラの次の座標が遠いときの移動速度の最大
	constexpr float kFarCameraMaxMoveSpeed = 50.0f;
	//カメラの次の座標が遠いときの移動速度の最小
	constexpr float kFarCameraMinMoveSpeed = 5.0f;

	//カメラが高速移動するときに目標座標まで行くのにかかる時間
	constexpr int kFastMoveTime = 5;

	//最小移動ベクトル
	constexpr float kSmallestMoveVecLength = 2.0f;

	//カメラを揺らす大きさ
	constexpr int kShakePower = 10;

	//ゆるやかにカメラを揺らす時の大きさ
	constexpr float kSwayPower = 0.6f;

	//緩やかにカメラを揺らす時の揺れの速さ
	constexpr float kSwaySpeed = 0.5f;

	//上下の差ができた際にY座標をどれだけずらすか
	constexpr float kCameraPosMaxYGap = 10.0f;

	//上下の差の最大と判断するY座標の差
	constexpr float kMaxYGap = 50.0f;

	//上下の差があるときのプレイヤーに近づくカメラの最近距離
	constexpr float kNearestLength = 10.0f;

	//カメラに最も近づく時のY座標の差
	constexpr float kNearestCameraPosYGap = 100.0f;

}

GameCamera::GameCamera() :
	m_updateFunc(nullptr),
	m_lightHandle(-1),
	m_stopCorrectTime(0),
	m_isFastMove(false),
	m_isStop(false),
	m_shakeTime(0),
	m_isSway(false),
	m_swayTime(0.0f),
	m_stopTime(0)
{
	//仮ライト
	m_lightHandle = CreateDirLightHandle(VGet(0, 0, 0));
	SetCameraNearFar(kCameraNear, kCameraFar);
}

GameCamera::~GameCamera()
{
}

void GameCamera::SetPoseCamera()
{
	m_updateFunc = &GameCamera::PoseUpdate;
}

void GameCamera::Update()
{
	(this->*m_updateFunc)();

	//ライトの座標をずっと更新し続ける
	SetLightPositionHandle(m_lightHandle, (m_localPos.GetWorldPos()).CastVECTOR());

}

void GameCamera::Final()
{
	DeleteLightHandle(m_lightHandle);
}

void GameCamera::SetBattleCamera()
{
	m_updateFunc = &GameCamera::BattleUpdate;
	m_localPos.SetLocalPos(kPlayerToCameraInitVec);
	m_nextCameraPos = m_localPos.GetWorldPos();
}

void GameCamera::SetCenterAndTarget(MyEngine::Vector3 player, MyEngine::Vector3 target)
{
	m_localPos.SetCenterPos(player);
	m_targetPos = target;
}

void GameCamera::SetFrontPos(MyEngine::Vector3 pos)
{
	m_localPos.SetFrontPos(pos);
}

void GameCamera::SetPlayerVelo(MyEngine::Vector3 velo)
{
	m_playerVelo = velo;
}

void GameCamera::SetLocalPos()
{
	m_localPos.SetLocalPos(kPlayerToCameraInitVec);
}

void GameCamera::ShakeCamera(int time)
{
	ShakeCamera(time,kShakePower);
}

void GameCamera::ShakeCamera(int time, int power)
{
	m_shakeTime = time;
	m_shakePower = power;
}

void GameCamera::PoseUpdate()
{
	//揺らす大きさ(基本はゼロ)
	MyEngine::Vector3 shiftVec;

	//カメラを揺らす設定がされていたら
	if (m_shakeTime > 0)
	{
		m_shakeTime--;

		//マイナスにもなるようにランダムからランダムの大きさの半分減らす
		int shakePowerHalf = static_cast<int>(m_shakePower * 0.5f);

		//カメラのターゲット座標を揺らす
		shiftVec.x = static_cast<float>(GetRand(m_shakePower) - shakePowerHalf);
		shiftVec.y = static_cast<float>(GetRand(m_shakePower) - shakePowerHalf);
		shiftVec.z = static_cast<float>(GetRand(m_shakePower) - shakePowerHalf);
	}

	//カメラを緩やかに揺らす設定がされていたら
	if (m_isSway)
	{
		m_swayTime += kSwaySpeed;

		//ターゲット座標を揺らす
		shiftVec.y += sinf(m_swayTime) * kSwayPower;

		//緩やかに揺らすのをやめる設定を毎フレームする
		m_isSway = false;
	}

	SetCameraPositionAndTarget_UpVecY(m_localPos.GetWorldPos().CastVECTOR(), (m_targetPos + shiftVec).CastVECTOR());

	SetLightDirectionHandle(m_lightHandle, (m_localPos.GetWorldPos() - m_targetPos).Normalize().CastVECTOR());
}

void GameCamera::BattleUpdate()
{
	//動かない時間が設定されていたら
	if (m_stopTime > 0)
	{
		m_stopTime--;
		if (m_stopTime <= 0)
		{
			m_isStop = false;
		}
	}


	//動かないと設定されていたらリターン
	if (m_isStop)return;

	//一定値以下の移動ベクトルは0に戻しておく
	if (m_moveVec.Length() < kSmallestMoveVecLength)
	{
		m_moveVec = MyEngine::Vector3(0.0f, 0.0f, 0.0f);
	}

	//今のカメラのワールド座標
	//プレイヤーの次の座標
	MyEngine::Vector3 playerPos = m_localPos.GetCenterPos() + m_playerVelo;
	//中心座標の更新
	MyEngine::Vector3 cameraWorldPos = m_localPos.GetWorldPos();
	m_localPos.SetCenterPos(playerPos);
	//プレイヤーからカメラへの次の座標に対するベクトル
	MyEngine::Vector3 playerToCamera = cameraWorldPos - playerPos;
	//次のカメラのローカル座標
	m_nextCameraPos = m_localPos.ChangeWorldToLocal(playerPos + playerToCamera);

	//X,Y,Zそれぞれを一定の値の中にとどめるようにする

	//最大値の調整
	m_nextCameraPos.x = std::fmin(m_nextCameraPos.x, kOnMoveMaxLocalPosX);
	m_nextCameraPos.y = std::fmin(m_nextCameraPos.y, kOnMoveMaxLocalPosY);
	m_nextCameraPos.z = std::fmin(m_nextCameraPos.z, kOnMoveMaxLocalPosZ);

	//最小値の調整
	m_nextCameraPos.x = std::fmax(m_nextCameraPos.x, kOnMoveMinLocalPosX);
	m_nextCameraPos.y = std::fmax(m_nextCameraPos.y, kOnMoveMinLocalPosY);
	m_nextCameraPos.z = std::fmax(m_nextCameraPos.z, kOnMoveMinLocalPosZ);

	//上下の座が大きければカメラの座標を補正する
	MyEngine::Vector3 addVec;

	//上下の差に合わせてカメラの座標を上下に動かす
	float yGap = m_targetPos.y - m_localPos.GetCenterPos().y;	

	float yShiftLength = yGap / kMaxYGap;

	addVec.y = -yShiftLength;
	
	m_nextCameraPos += addVec;

	//プレイヤーが移動していないとき
	if (m_playerVelo.SqLength() < 0.001f)
	{
		//移動していない時間を計る
		m_stopCorrectTime++;

		//移動していない時間が一定時間たったら
		if (m_stopCorrectTime > kLocalPosMoveStartTime)
		{
			//一定時間かけてローカル座標を既定の座標に近づけていく
			MyEngine::Vector3 initPos(m_nextCameraPos.x, m_nextCameraPos.y, kLocalInitPosZ);

			//X座標は左右を超えていたら一定座標まで戻す
			if (m_nextCameraPos.x > kLocalInitPosX)
			{
				initPos.x = kLocalInitPosX;
			}
			else if (m_nextCameraPos.x < -kLocalInitPosX)
			{
				initPos.x = -kLocalInitPosX;
			}
			//Y座標は上下で戻す座標を変える
			if (m_nextCameraPos.y > kLocalInitUnderPosY)
			{
				initPos.y = kLocalInitUnderPosY;
			}
			else if (m_nextCameraPos.y < kLocalInitTopPosY)
			{
				initPos.y = kLocalInitTopPosY;
			}

			//移動ベクトルが指定されていなければ
			if (m_moveVec.SqLength() < 0.001f)
			{
				MyEngine::Vector3 moveDir = (initPos - m_nextCameraPos).Normalize();
				float moveSpeed = (initPos - m_nextCameraPos).Length() / kLocalPosMoveTime;

				m_moveVec = moveDir * moveSpeed;
			}

			if ((initPos - m_nextCameraPos).Length() < m_moveVec.Length())
			{
				m_moveVec = m_moveVec.Normalize();

				m_moveVec *= (initPos - m_nextCameraPos).Length();
			}

			m_nextCameraPos += m_moveVec;
		}
	}
	else
	{
		//動くたびにリセット
		m_moveVec = MyEngine::Vector3(0.0f, 0.0f, 0.0f);
		m_stopCorrectTime = 0;
	}

	//カメラが速く移動すると設定されていたら
	if (m_isFastMove)
	{
		//向かいたいローカル座標を設定
		m_localPos.SetLocalPos(m_nextCameraPos);
		//そのローカル座標に向かうベクトルを作成
		MyEngine::Vector3 toNextPos = m_localPos.GetWorldPos() - m_lastCameraPos;
		//移動速度
		float speed = toNextPos.Length() / kFastMoveTime;

		//最大速度を超えないようにする
		speed = std::fmin(speed, kFarCameraMaxMoveSpeed);
		//最小速度も設定しておく
		speed = std::fmax(speed, kFarCameraMinMoveSpeed);

		//カメラを瞬間移動させずに速く移動させる
		m_moveVec = toNextPos.Normalize() * speed;

		if (m_moveVec.Length() > toNextPos.Length())
		{
			m_moveVec = toNextPos;
			m_isFastMove = false;
		}

		m_nextCameraPos = m_localPos.ChangeWorldToLocal(m_lastCameraPos + m_moveVec);
	}

	//揺らす大きさ(基本はゼロ)
	MyEngine::Vector3 shakeVec;

	//カメラを揺らす設定がされていたら
	if (m_shakeTime > 0)
	{
		m_shakeTime--;

		//マイナスにもなるようにランダムからランダムの大きさの半分減らす
		int shakePowerHalf = static_cast<int>(m_shakePower * 0.5f);

		//カメラのターゲット座標を揺らす
		shakeVec.x = static_cast<float>(GetRand(m_shakePower) - shakePowerHalf);
		shakeVec.y = static_cast<float>(GetRand(m_shakePower) - shakePowerHalf);
		shakeVec.z = static_cast<float>(GetRand(m_shakePower) - shakePowerHalf);
	}

	//ターゲット座標の設定
	MyEngine::Vector3 targetPos = (m_targetPos - m_localPos.GetCenterPos()) * kCameraTargetPosRate + m_localPos.GetCenterPos();

	//カメラを緩やかに揺らす設定がされていたら
	if (m_isSway)
	{
		m_swayTime += kSwaySpeed;

		//ターゲット座標を揺らす
		targetPos.y += sinf(m_swayTime) * kSwayPower;
	}

	//ローカル座標の設定
	m_localPos.SetLocalPos(m_nextCameraPos);
	//緩やかに揺らすのをやめる設定を毎フレームする
	m_isSway = false;

	SetCameraPositionAndTarget_UpVecY(m_localPos.GetWorldPos().CastVECTOR(), (targetPos + shakeVec).CastVECTOR());

	SetLightDirectionHandle(m_lightHandle, (m_localPos.GetWorldPos() - targetPos).Normalize().CastVECTOR());

	m_lastCameraPos = m_localPos.GetWorldPos();
}
