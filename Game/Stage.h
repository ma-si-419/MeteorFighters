#pragma once
#include "Vector3.h"
#include <string>
#include <memory>

class EffectManager;
class Effect;
class Stage
{
public:
	Stage(std::shared_ptr<EffectManager> manager);
	virtual ~Stage();

	void Init();

	void Update();

	void Draw();

	void Final();

	void SetStageModelHandle(int handle);

	void SetSkyDomeModelHandle(int handle);

	std::string GetStagePath() { return m_stagePath; }

	std::string GetSkyDomePath() { return m_skyDomePath; }
private:
	
	//エフェクトマネージャー
	std::shared_ptr<EffectManager> m_pEffectManager;

	int m_stageHandle;
	
	int m_skyDomeHandle;

	std::string m_stagePath;

	std::string m_skyDomePath;

	std::shared_ptr<Effect> m_pEffect;
};

