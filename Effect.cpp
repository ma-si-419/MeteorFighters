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

void Effect::SetRotationAndPos(MyEngine::Vector3 rotation, MyEngine::Vector3 pos)
{
	MATRIX mat;

	MATRIX rotMat = rotation.GetRotationMat();

	MATRIX posMat = MGetTranslate(pos.CastVECTOR());

	mat = MMult(rotMat, posMat);

	Effekseer::Matrix43 effMat;

	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 3; ++x)
		{
			effMat.Value[y][x] = mat.m[y][x];
		}
	}

	GetEffekseer3DManager()->SetBaseMatrix(m_handle,effMat);
}
