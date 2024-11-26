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
		{Effect::EffectKind::kEnergy,"Energy"},
		{Effect::EffectKind::kLowHit,"LowHit"},
		{Effect::EffectKind::kEnergyHit,"EnergyHit"},
		{Effect::EffectKind::kTeleportaion,"Teleportation"},
	};
}

Effect::Effect(EffectKind kind) :
	m_handle(-1),
	m_path("empty"),
	m_time(0),
	m_loopStartTime(0),
	m_loopEndTime(0),
	m_lifeTime(0)

{
	m_path = "data/effekseer/" + kEffectPathMap.at(kind) + ".efk";
}

Effect::Effect(std::string string) :
	m_handle(-1),
	m_path("empty"),
	m_time(0),
	m_loopStartTime(0),
	m_loopEndTime(0),
	m_lifeTime(0)
{
	m_path = "data/effekseer/" + string + ".efk";
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
	//���[�v����Ɛݒ肵�Ă�����
	if (m_loopEndTime > 0)
	{
		//���̍Đ��t���[���Ɣ�ׂ�
		if (m_time > m_loopEndTime)
		{
			//���[�v����
			return true;
		}
		else
		{
			//���[�v���Ȃ�
			return false;
		}
	}
	//�����������[�v�̐ݒ肪����Ă��Ȃ��ꍇ
	else
	{
		return false;
	}
}

bool Effect::IsEndLifeTime()
{
	//�ݒ肳��Ă��Ȃ����false��Ԃ�
	if (m_lifeTime == 0) return false;

	//�����Ă�����true��Ԃ�
	if (m_time > m_lifeTime)
	{
		return true;
	}

	//�����܂ŗ�����false
	return false;
}
