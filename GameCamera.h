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

	/// <summary>
	/// カメラを揺れを設定する
	/// </summary>
	/// <param name="time">揺らす時間</param>
	void ShakeCamera(int time) { m_shakeTime = time; }
private:

	//ローカル座標(プレイヤーを中心として、自身の座標を計算する)
	MyEngine::LocalPos m_localPos;

	//カメラのターゲット座標
	MyEngine::Vector3 m_targetPos;

	//移動ベクトル
	MyEngine::Vector3 m_moveVec;

	//次のフレームのカメラの座標
	MyEngine::Vector3 m_nextCameraPos;

	//前のフレームのカメラの座標
	MyEngine::Vector3 m_lastCameraPos;

	//プレイヤーの移動ベクトル
	MyEngine::Vector3 m_playerVelo;

	//カメラを止める時間
	int m_stopTime;

	//カメラを高速移動させるか
	bool m_isFastMove;

	//カメラを止めるかどうか
	bool m_isStop;
	
	//揺らす時間
	int m_shakeTime;

#ifdef _DEBUG

	//ライトハンドル
	int m_lightHandle;

#endif // _DEBUG

};


