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

	m_dir = (m_status.targetPos - m_rigidbody.GetPos()).Normalize();
}

void Attack::Update()
{
	//寿命での削除
	if (m_lifeTime > m_status.lifeTime)
	{
		m_isExist = false;
	}

	//攻撃に速度があれば敵に向かって飛んでいく	
	m_rigidbody.SetVelo(m_dir * m_status.speed);

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
	//自身がプレイヤーの攻撃で
	if(GetTag() == ObjectTag::kPlayerAttack)
	{
		//エネミーにぶつかったら
		if (collider->GetTag() == ObjectTag::kEnemy)
		{
			m_isExist = false;
			printfDx("ぶつかった");
		}
	}
	//自身がエネミーの攻撃で
	else if (GetTag() == ObjectTag::kEnemyAttack)
	{
		//プレイヤーにぶつかったら
		if (collider->GetTag() == ObjectTag::kPlayer)
		{
			m_isExist = false;
		}
	}

}
