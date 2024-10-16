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
	//�����ł̍폜
	if (m_lifeTime > m_status.lifeTime)
	{
		m_isExist = false;
		Collidable::Final();
	}

	//�V�[���ɏo�Ă���̃t���[�����𐔂���
	m_lifeTime++;
}

void Attack::Draw()
{
}

void Attack::OnCollide(std::shared_ptr<Collidable> collider)
{
}
