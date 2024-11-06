#include "Enemy.h"
#include "EnemyStateIdle.h"
#include "GameManager.h"

namespace
{
#ifdef _DEBUG

	const MyEngine::Vector3 kDebugPos(50, 0, 50);

#endif // _DEBUG

}

Enemy::Enemy(CharacterKind kind) :
	CharacterBase(ObjectTag::kEnemy, kind)
{
	m_modelHandle = MV1LoadModel("data/model/Fighter.mv1");

	MV1SetScale(m_modelHandle, VGet(0.1f, 0.1f, 0.1f));

	ChangeAnim(AnimKind::kLowAttack1, true);
}

Enemy::~Enemy()
{
}

void Enemy::Init()
{
	Collidable::Init();
	m_rigidbody.SetPos(kDebugPos);
	MV1SetPosition(m_modelHandle, m_rigidbody.GetPos().CastVECTOR());

	m_nowHp = m_status.hp;
	m_nowMp = m_status.startMp;

	auto enemy = std::static_pointer_cast<Enemy>(shared_from_this());

	m_pState = std::make_shared<EnemyStateIdle>(enemy);
	m_pState->Enter();

	//自身を中心にする
	m_targetLocalPos.SetCenterPos(m_rigidbody.GetPos());

	//前方向を設定する
	m_targetLocalPos.SetFrontPos(m_rigidbody.GetPos() + (m_pGameManager->GetPlayerPos() - m_rigidbody.GetPos()).Normalize());

}

void Enemy::Update()
{
	//Stateに変化があれば変化させる
	if (m_pState != m_pState->m_pNextState)
	{
		m_pState = m_pState->m_pNextState;
	}

#ifdef _DEBUG

	PlayAnim();

	m_targetLocalPos.SetCenterPos(m_rigidbody.GetPos());


	m_targetLocalPos.SetLocalPos(m_targetLocalPos.ChangeWorldToLocal(m_pGameManager->GetPlayerPos()));

	MyEngine::Vector3 pos = m_targetLocalPos.GetLocalPos().Normalize();

	DrawFormatString(0, 128, GetColor(255, 255, 255), "エネミーから見たプレイヤー座標(X:%0.1f,Y:%0.1f,Z:%0.1f)", pos.x, pos.y, pos.z);


#endif // _DEBUG


	//Stateの更新処理
	m_pState->Update();

	SetDrawPos(m_rigidbody.GetPos());
}

void Enemy::Draw()
{
	
	MV1DrawModel(m_modelHandle);

#ifdef _DEBUG

	DrawFormatString(0, 144, GetColor(255, 255, 255), "%.0f", m_nowHp);

#endif // _DEBUG

}

void Enemy::OnCollide(std::shared_ptr<Collidable> collider)
{

	m_pState->OnCollide(collider);

#ifdef _DEBUG

	DrawString(0, 48, "Enemyがなにかとぶつかった", GetColor(255, 255, 255));

#endif // _DEBUG


}
