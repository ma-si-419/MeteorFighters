#include "Effect.h"
#include <map>
#include "EffekseerForDXLib.h"
#include "EffectManager.h"

namespace
{
	std::map<Effect::EffectKind, std::string> kEffectPathMap =
	{
		{Effect::EffectKind::kCharge,"EnergyCharge"},
	};
}

Effect::Effect(EffectKind kind):
	m_handle(-1),
	m_path("empty")
{
	std::string path = "data/effekseer/" + kEffectPathMap.at(kind) + ".efk";
}

void Effect::Init(std::shared_ptr<EffectManager> manager, MyEngine::Vector3 pos)
{
	m_pos = pos;

	manager->Entry(shared_from_this(),pos);

}

void Effect::End()
{
	
}
