#include "Effect.h"
#include <map>
#include "EffekseerForDXLib.h"
#include "EffectManager.h"

namespace
{
	std::map<Effect::EffectKind, std::string> kEffectPathMap =
	{
		{Effect::EffectKind::kCharge,"EnergyCharge"},
		{Effect::EffectKind::kDash,"Dash"},
		{Effect::EffectKind::kDashEnd,"DashEnd"},
	};
}

Effect::Effect(EffectKind kind) :
	m_handle(-1),
	m_path("empty"),
	m_time(0),
	m_loopStartTime(0),
	m_loopEndTime(0)

{
	m_path = "data/effekseer/" + kEffectPathMap.at(kind) + ".efk";
}

void Effect::Init(std::shared_ptr<EffectManager> manager, MyEngine::Vector3 pos)
{
	m_pos = pos;

	manager->Entry(shared_from_this(), pos);
}

void Effect::Update()
{
	m_time++;
}

void Effect::End()
{

}

void Effect::SetLoop(int loopStart, int loopEnd)
{
	m_loopStartTime = loopStart;

	m_loopEndTime = loopEnd;
}

void Effect::SetLoop()
{
	m_loopStartTime = 0;
	m_loopEndTime = 0;
}

bool Effect::IsEndLoop()
{
	//ループすると設定していたら
	if (m_loopEndTime > 0)
	{
		//今の再生フレームと比べて
		if (m_time > m_loopEndTime)
		{
			//ループする
			return true;
		}
		else
		{
			//ループしない
			return false;
		}
	}
	//そもそもループの設定がされていない場合
	else
	{
		return false;
	}
}