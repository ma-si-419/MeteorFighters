#include "Enemy.h"
#include "EnemyStateIdle.h"

namespace
{
#ifdef _DEBUG

	const MyEngine::Vector3 kDebugPos(50, 0, 50);

#endif // _DEBUG

}

Enemy::Enemy(CharacterKind kind) :
	CharacterBase(ObjectTag::kEnemy,kind)
{
	m_modelHandle = MV1LoadModel("data/model/Fighter.mv1");

	MV1SetScale(m_modelHandle, VGet(0.1f, 0.1f, 0.1f));


}

Enemy::~Enemy()
{
}

void Enemy::Init()
{
	Collidable::Init();
	m_rigidbody.SetPos(kDebugPos);
	MV1SetPosition(m_modelHandle, m_rigidbody.GetPos().CastVECTOR());

	m_pState = std::make_shared<EnemyStateIdle>();
	m_pState->Enter();
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
#ifdef _DEBUG

	DrawString(0, 48, "Enemy‚ª‚È‚É‚©‚Æ‚Ô‚Â‚©‚Á‚½", GetColor(255, 255, 255));

#endif // _DEBUG


}
