#include "Enemy.h"
#include "EnemyStateIdle.h"

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
	m_pState = std::make_shared<EnemyStateIdle>();
	m_pState->Enter();
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
	//State‚É•Ï‰»‚ª‚ ‚ê‚Î•Ï‰»‚³‚¹‚é
	if (m_pState->GetKind() != m_pState->m_pNextState->GetKind())
	{
		m_pState = m_pState->m_pNextState;
	}

	//State‚ÌXVˆ—
	m_pState->Update();
}

void Enemy::Draw()
{
	MV1DrawModel(m_modelHandle);
}

void Enemy::OnCollide(std::shared_ptr<Collidable> collider)
{
}
