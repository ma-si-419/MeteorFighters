#include "GameCamera.h"
#include <cmath>

namespace
{
	const MyEngine::Vector3 kPlayerToCameraInitVec(-17, 15, -30);

	constexpr float kCameraNear = 1.0f;
	constexpr float kCameraFar = 500.0f;

	constexpr float kCameraToPlayerLange = 50.0f;

	constexpr float kOnMoveMaxLocalPosX = 42.0f;//左幅(大きいほど左に広がる)
	constexpr float kOnMoveMaxLocalPosY = 18.0f;//下幅(大きいほど下に広がる)
	constexpr float kOnMoveMaxLocalPosZ = -30.0f;//手前幅(大きいほどプレイヤーが手前に来る)

	constexpr float kOnMoveMinLocalPosX = -42.0f;//右幅(小さいほど右に広がる)
	constexpr float kOnMoveMinLocalPosY = -12.0f;//上幅(小さいほど上に広がる)
	constexpr float kOnMoveMinLocalPosZ = -40.0f;//奥幅(小さいほどプレイヤーが奥に行く)


	constexpr float kLocalInitPosX = 27.0f;
	constexpr float kLocalInitTopPosY = -2.0f;
	constexpr float kLocalInitUnderPosY = 8.0f;
	constexpr float kLocalInitPosZ = -30.0f;

	//プレイヤーとターゲットの間でどの当たりにカメラの焦点を合わせるか
	constexpr float kCameraTargetPosRate = 0.4;

	//ローカル座標の補正にかける時間
	constexpr int kLocalPosMoveTime = 12;

	//ローカル座標の補正を始める時間
	constexpr int kLocalPosMoveStartTime = 5;

	//距離によって横移動のあそびを減らす割合
	constexpr float kLangeSubRate = 0.1f;

}

GameCamera::GameCamera() :
	m_lightHandle(-1),
	m_stopTime(0)
{
	m_lightHandle = CreateDirLightHandle(VGet(0, 0, 0));
}

GameCamera::~GameCamera()
{
	DeleteLightHandle(m_lightHandle);
}

void GameCamera::Init(MyEngine::Vector3 centerPos)
{
	m_localPos.SetCenterPos(centerPos);
	m_localPos.SetLocalPos(kPlayerToCameraInitVec);
	SetCameraNearFar(kCameraNear, kCameraFar);
	m_nextCameraPos = m_localPos.GetWorldPos();
}

void GameCamera::Update()
{
	//ローカル座標の設定
	m_localPos.SetLocalPos(m_nextCameraPos);

	//今のカメラのワールド座標
	MyEngine::Vector3 cameraWorldPos = m_localPos.GetWorldPos();
	//プレイヤーの次の座標
	MyEngine::Vector3 playerPos = m_localPos.GetCenterPos() + m_playerVelo;
	//中心座標の更新
	m_localPos.SetCenterPos(playerPos);
	//プレイヤーからカメラへの次の座標に対するベクトル
	MyEngine::Vector3 playerToCamera = cameraWorldPos - playerPos;
	//次のカメラのローカル座標
	m_nextCameraPos = m_localPos.ChangeWorldToLocal(playerPos + playerToCamera);

	//X,Y,Zそれぞれを一定の値の中にとどめるようにする

	//最大値の調整
	
	//X座標は離れるほどあそびを小さくしていく
	float maxX = kOnMoveMaxLocalPosX - ((m_nextCameraPos - m_targetPos).Length()) * kLangeSubRate;
	m_nextCameraPos.x = std::fmin(m_nextCameraPos.x, maxX);
	
	
	m_nextCameraPos.y = std::fmin(m_nextCameraPos.y, kOnMoveMaxLocalPosY);
	m_nextCameraPos.z = std::fmin(m_nextCameraPos.z, kOnMoveMaxLocalPosZ);

	//最小値の調整
	
	//X座標は離れるほどあそびを小さくしていく
	float minX = kOnMoveMinLocalPosX + ((m_nextCameraPos - m_targetPos).Length()) * kLangeSubRate;
	m_nextCameraPos.x = std::fmax(m_nextCameraPos.x, minX);
	
	
	m_nextCameraPos.y = std::fmax(m_nextCameraPos.y, kOnMoveMinLocalPosY);
	m_nextCameraPos.z = std::fmax(m_nextCameraPos.z, kOnMoveMinLocalPosZ);

	//プレイヤーが移動していないとき
	if (m_playerVelo.SqLength() < 0.001f)
	{
		//移動していない時間を計る
		m_stopTime++;
		
		//移動していない時間が一定時間たったら
		if (m_stopTime > kLocalPosMoveStartTime)
		{
			//一定時間かけてローカル座標を既定の座標に近づけていく
			MyEngine::Vector3 initPos(m_nextCameraPos.x, m_nextCameraPos.y, kLocalInitPosZ);

			//X座標は左右を超えていたら一定座標まで戻す
			if (m_nextCameraPos.x > kLocalInitPosX)
			{
				initPos.x = kLocalInitPosX - ((m_nextCameraPos - m_targetPos).Length()) * kLangeSubRate;
			}
			else if (m_nextCameraPos.x < -kLocalInitPosX)
			{
				initPos.x = -kLocalInitPosX + ((m_nextCameraPos - m_targetPos).Length()) * kLangeSubRate;
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
		m_stopTime = 0;
	}

	//ターゲット座標の設定
	MyEngine::Vector3 targetPos = (m_targetPos - m_localPos.GetCenterPos()) * kCameraTargetPosRate + m_localPos.GetCenterPos();

	SetCameraPositionAndTarget_UpVecY(cameraWorldPos.CastVECTOR(), targetPos.CastVECTOR());

	SetLightDirectionHandle(m_lightHandle, (cameraWorldPos - targetPos).Normalize().CastVECTOR());
}

void GameCamera::SetPlayerPosAndTarget(MyEngine::Vector3 player, MyEngine::Vector3 target)
{
	m_localPos.SetCenterPos(player);
	m_targetPos = target;
}

void GameCamera::SetPlayerFrontPos(MyEngine::Vector3 pos)
{
	m_localPos.SetFrontPos(pos);
}

void GameCamera::SetPlayerVelo(MyEngine::Vector3 velo)
{
	m_playerVelo = velo;
}

void GameCamera::SetCamera()
{
	SetCameraNearFar(kCameraNear, kCameraFar);

	MyEngine::Vector3 cameraPos = m_localPos.GetWorldPos();
	SetCameraPositionAndTarget_UpVecY(cameraPos.CastVECTOR(), m_targetPos.CastVECTOR());

	SetLightDirectionHandle(m_lightHandle, (cameraPos - m_targetPos).Normalize().CastVECTOR());
}
