#include "EffectManager.h"
#include "Effect.h"
#include "EffekseerForDXLib.h"

void EffectManager::Entry(std::shared_ptr<Effect> effect,MyEngine::Vector3 pos)
{
	int handle = LoadEffekseerEffect(effect->GetPath().c_str());

	PlayEffekseer3DEffect(handle);

	SetPosPlayingEffekseer3DEffect(handle,pos.x, pos.y, pos.z);

	effect->SetHandle(handle);

	m_effects.push_back(effect);
}

void EffectManager::Exit(std::shared_ptr<Effect> effect)
{
	bool found = (std::find(m_effects.begin(), m_effects.end(), effect) != m_effects.end());
	//“o˜^‚³‚ê‚Ä‚¢‚½ê‡
	if (found)
	{


		m_effects.remove(effect);
	}
}
