#pragma once
#include "LocalPos.h"

class GameCamera
{
public:
	GameCamera();
	~GameCamera();

	void Init(MyEngine::Vector3 centerPos);
	void Update();

	/// <summary>
	/// プレイヤーの座標とカメラのターゲット座標を設定する
	/// </summary>
	/// <param name="player">プレイヤーの座標</param>
	/// <param name="target">カメラのターゲットの座標</param>
	void SetPlayerPosAndTarget(MyEngine::Vector3 player, MyEngine::Vector3 target);

	/// <summary>
	/// プレイヤーの向いている方向を設定する
	/// </summary>
	/// <param name="pos">プレイヤーの前方向とする座標</param>
	void SetPlayerFrontPos(MyEngine::Vector3 pos);

	/// <summary>
	/// プレイヤーのベロシティを設定する
	/// </summary>
	/// <param name="velo">プレイヤーのベロシティ</param>
	void SetPlayerVelo(MyEngine::Vector3 velo);

	/// <summary>
	/// スクリーンなどを変更した時にカメラの座標などを設定する
	/// </summary>
	void SetCamera();

	/// <summary>
	/// カメラを止める
	/// </summary>
	void StopCamera() { m_isStop = true; }

	/// <summary>
	/// カメラの移動を始める
	/// </summary>
	void StartMoveCamera() { m_isStop = false; }

	/// <summary>
	/// カメラを高速移動で動かしたいときに呼ぶ関数
	/// </summary>
	void StartFastCameraMove() { m_isFastMove = true; }

private:

	LocalPos m_localPos;

	MyEngine::Vector3 m_targetPos;

	MyEngine::Vector3 m_moveVec;

	MyEngine::Vector3 m_nextCameraPos;

	MyEngine::Vector3 m_lastCameraPos;

	MyEngine::Vector3 m_playerVelo;

	int m_stopTime;

	int m_lightHandle;

	bool m_isFastMove;

	bool m_isStop;
};

