#pragma once
#include "LocalPos.h"

class GameCamera
{
public:
	GameCamera();
	~GameCamera();

	void Init(MyEngine::Vector3 centerPos);
	void Update();

	void SetPlayerPosAndTarget(MyEngine::Vector3 player, MyEngine::Vector3 target);

	void SetFrontPos(MyEngine::Vector3 pos);


private:

	LocalPos m_localPos;

	MyEngine::Vector3 m_targetPos;

	int m_cameraHandle;
};

