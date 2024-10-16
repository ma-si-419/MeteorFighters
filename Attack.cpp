#include "Attack.h"
#include "Physics.h"

Attack::Attack(ObjectTag tag, MyEngine::Vector3 pos):
	Collidable(tag, ColliderData::Kind::kCapsule),
	m_lifeTime(0),
	m_isExist(true)
{
}

Attack::~Attack()
{
}

void Attack::Init(AttackStatus status)
{
	m_status = status;
	Collidable::Init();
}

void Attack::Update()
{
	//寿命での削除
	if (m_lifeTime > m_status.lifeTime)
	{
		m_isExist = false;
		Collidable::Final();
	}

	//シーンに出てからのフレーム数を数える
	m_lifeTime++;
}

void Attack::Draw()
{
}

void Attack::OnCollide(std::shared_ptr<Collidable> collider)
{
}
