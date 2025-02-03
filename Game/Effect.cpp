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
		{Effect::EffectKind::kBigEnergy,"BigEnergy"},
		{Effect::EffectKind::kGuardHit,"GuardHit"},
		{Effect::EffectKind::kLowHit,"LowHit"},
		{Effect::EffectKind::kMiddleHit,"MiddleHit"},
		{Effect::EffectKind::kHighHit,"HighHit"},
		{Effect::EffectKind::kEnergyHit,"EnergyHit"},
		{Effect::EffectKind::kTeleportaion,"Teleportation"},
		{Effect::EffectKind::kStageHit,"StageHit"},
		{Effect::EffectKind::kLaserHit,"LaserHit"},
		{Effect::EffectKind::kStartLaser,"StartLaser"},
		{Effect::EffectKind::kFalls,"Falls"},
		{Effect::EffectKind::kShock,"Shock"}
	};

	constexpr float kDefaultPlaySpeed = 1.0f;
}

Effect::Effect(EffectKind kind) :
	m_handle(-1),
	m_path("empty"),
	m_time(0),
	m_loopStartTime(0),
	m_loopEndTime(0),
	m_lifeTime(0),
	m_playSpeed(kDefaultPlaySpeed)
{
	m_path = "data/effekseer/" + kEffectPathMap.at(kind) + ".efk";
}

Effect::Effect(std::string string) :
	m_handle(-1),
	m_path("empty"),
	m_time(0),
	m_loopStartTime(0),
	m_loopEndTime(0),
	m_lifeTime(0),
	m_playSpeed(kDefaultPlaySpeed)
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
	m_time += m_playSpeed;
}

void Effect::End()
{

}

void Effect::SetLoop(float loopStart, float loopEnd)
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

void Effect::SetPlaySpeed()
{
	m_playSpeed = kDefaultPlaySpeed;
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
