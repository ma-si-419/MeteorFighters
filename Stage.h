#pragma once
#include "Vector3.h"
class Stage
{
public:
	Stage();
	virtual ~Stage();

	void Init();
	void Draw();

private:
	int m_modelHandle;
	MyEngine::Vector3 m_pos;
};

