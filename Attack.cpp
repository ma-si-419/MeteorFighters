#include "Attack.h"
#include "Physics.h"
#include "CapsuleColliderData.h"

Attack::Attack(ObjectTag tag, MyEngine::Vector3 pos):
	Collidable(tag, ColliderData::Kind::kCapsule),
	m_lifeTime(0),
	m_isExist(true)
{
	m_rigidbody.SetPos(pos);
	m_pColData->SetIsTrigger(true);
}

Attack::~Attack()
{
}

void Attack::Init(AttackStatus status)
{
	m_status = status;
	Collidable::Init();
	auto col = std::dynamic_pointer_cast<CapsuleColliderData>(m_pColData);
	col->m_radius = status.radius;
	col->m_length = 0.1f;
}

void Attack::Update()
{
	//寿命での削除
	if (m_lifeTime > m_status.lifeTime)
	{
		m_isExist = false;
	}

	//シーンに出てからのフレーム数を数える
	m_lifeTime++;
}

void Attack::Draw()
{
}

void Attack::Final()
{
	Collidable::Final();
}

void Attack::OnCollide(std::shared_ptr<Collidable> collider)
{
}
