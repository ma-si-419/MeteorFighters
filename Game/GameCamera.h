#pragma once
#include "LocalPos.h"

class GameCamera
{
public:
	GameCamera();
	~GameCamera();

	void SetPoseCamera();
	void Update();

	/// <summary>
	/// このクラスを削除するときに使用する関数
	/// </summary>
	void Final();

	void SetBattleCamera();

	/// <summary>
	/// プレイヤーの座標とカメラのターゲット座標を設定する
	/// </summary>
	/// <param name="player">プレイヤーの座標</param>
	/// <param name="target">カメラのターゲットの座標</param>
	void SetCenterAndTarget(MyEngine::Vector3 player, MyEngine::Vector3 target);

	/// <summary>
	/// プレイヤーの向いている方向を設定する
	/// </summary>
	/// <param name="pos">プレイヤーの前方向とする座標</param>
	void SetFrontPos(MyEngine::Vector3 pos);

	/// <summary>
	/// プレイヤーのベロシティを設定する
	/// </summary>
	/// <param name="velo">プレイヤーのベロシティ</param>
	void SetPlayerVelo(MyEngine::Vector3 velo);

	/// <summary>
	/// カメラのローカル座標を初期化する
	/// </summary>
	void SetLocalPos();

	/// <summary>
	/// カメラのローカル座標を設定する
	/// </summary>
	/// <param name="pos">カメラから見てどこにいるか<param>
	void SetLocalPos(MyEngine::Vector3 pos) { m_localPos.SetLocalPos(pos); }

	/// <summary>
	/// カメラを止める
	/// </summary>
	void StopCamera() { m_isStop = true; }

	/// <summary>
	/// カメラを止める(時間指定)
	/// </summary>
	/// <param name="stopTime">カメラを止める時間を設定する</param>
	void StopCamera(int stopTime) { m_stopTime = stopTime; m_isStop = true; }

	/// <summary>
	/// カメラの移動を始める
	/// </summary>
	void StartMoveCamera() { m_isStop = false; }

	/// <summary>
	/// カメラを高速移動で動かしたいときに呼ぶ関数
	/// </summary>
	void StartFastCameraMove() { m_isFastMove = true; }

	/// <summary>
	/// 強くカメラを揺らす(揺らす大きさ固定)
	/// </summary>
	/// <param name="time">揺らす時間</param>
	void ShakeCamera(int time);

	/// <summary>
	/// カメラを揺らす(揺らす大きさ可変)
	/// </summary>
	/// <param name="time">揺らす時間</param>
	/// <param name="power">揺らす大きさ</param>
	void ShakeCamera(int time,int power);

	/// <summary>
	/// この関数を呼んでいる間緩やかにカメラを揺らす
	/// </summary>
	void SwayCamera() { m_isSway = true;}

private:
	//演出時のカメラ
	void PoseUpdate();
	//バトル時のカメラ
	void BattleUpdate();

private:

	using UpdateFunc = void (GameCamera::*)();
	UpdateFunc m_updateFunc;

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

	//カメラの補正を止める時間
	int m_stopCorrectTime;

	//カメラを止める時間
	int m_stopTime;

	//カメラを高速移動させるか
	bool m_isFastMove;

	//カメラを止めるかどうか
	bool m_isStop;
	
	//揺らす時間
	int m_shakeTime;

	//揺らす大きさ
	int m_shakePower;

	//カメラを揺らすかどうか
	bool m_isSway;

	//カメラを緩やかに揺らす時間
	float m_swayTime;

	//ライトハンドル
	int m_lightHandle;


};


