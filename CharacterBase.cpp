#include "CharacterBase.h"

CharacterBase::CharacterBase(ObjectTag tag) :
	Actor(tag, ColliderData::Kind::kCapsule),
	m_modelHandle(-1),
	m_attachAnim(-1),
	m_totalAnimTime(-1),
	m_playAnimTime(-1),
	m_animPlaySpeed(0),
	m_isLoop(false)
{
}

CharacterBase::~CharacterBase()
{
}

void CharacterBase::SetGameManager(std::shared_ptr<GameManager> manager)
{
	m_pGameManager = manager;
}

MyEngine::Vector3 CharacterBase::GetPos()
{
	return m_rigidbody.GetPos();
}

void CharacterBase::ChangeAnim(int animNumber, bool loop)
{
	//アニメーションを再生していたらけしておく
	if (m_attachAnim != -1)
	{
		MV1DetachAnim(m_modelHandle, m_attachAnim);
	}

	m_attachAnim = MV1AttachAnim(m_modelHandle, animNumber);
	m_totalAnimTime = MV1GetAnimTotalTime(m_modelHandle, animNumber);
	m_playAnimTime = 0;
	m_animPlaySpeed = 1.0f;
	m_isLoop = loop;
}

void CharacterBase::PlayAnim()
{
	MV1SetAttachAnimTime(m_modelHandle, m_attachAnim, m_playAnimTime);
	m_playAnimTime += m_animPlaySpeed;
	if (m_playAnimTime > m_totalAnimTime)
	{
		if (m_isLoop)
		{
			m_playAnimTime = 0;
		}
		else
		{

		}
	}
}

bool CharacterBase::GetAnimEnd()
{
	return false;
}
