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
	//Stateに変化があれば変化させる
	if (m_pState->GetKind() != m_pState->m_pNextState->GetKind())
	{
		m_pState = m_pState->m_pNextState;
	}

	//Stateの更新処理
	m_pState->Update();
}

void Enemy::Draw()
{
	MV1DrawModel(m_modelHandle);
}

void Enemy::OnCollide(std::shared_ptr<Collidable> collider)
{
#ifdef _DEBUG

	DrawString(0, 48, "Enemyがなにかとぶつかった", GetColor(255, 255, 255));

#endif // _DEBUG


}
