#include "Enemy.h"

namespace
{
#ifdef _DEBUG

	const MyEngine::Vector3 kDebugPos(500,0,500);

#endif // _DEBUG

}

Enemy::Enemy() :
	CharacterBase(ObjectTag::kEnemy)
{
	m_modelHandle = MV1LoadModel("data/model/protoFighter.mv1");
}

Enemy::~Enemy()
{
}

void Enemy::Init()
{
	m_rigidbody.SetPos(kDebugPos);
	MV1SetPosition(m_modelHandle,m_rigidbody.GetPos().CastVECTOR());
}

void Enemy::Update()
{
}

void Enemy::Draw()
{
	MV1DrawModel(m_modelHandle);
}

void Enemy::OnCollide(std::shared_ptr<Collidable> collider)
{
}
