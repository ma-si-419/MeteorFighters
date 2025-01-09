#pragma once
#include "Vector3.h"
#include <string>

class Stage
{
public:
	Stage();
	virtual ~Stage();

	void Init();
	void Draw();

	void Final();

	void SetStageModelHandle(int handle);

	void SetSkyDomeModelHandle(int handle);

	std::string GetStagePath() { return m_stagePath; }

	std::string GetSkyDomePath() { return m_skyDomePath; }
private:
	
	int m_stageHandle;
	
	int m_skyDomeHandle;

	std::string m_stagePath;

	std::string m_skyDomePath;
};

