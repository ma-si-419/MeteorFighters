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
	void SetFrontPos(MyEngine::Vector3 pos);


private:

	LocalPos m_localPos;

	MyEngine::Vector3 m_targetPos;

	int m_cameraHandle;
};

