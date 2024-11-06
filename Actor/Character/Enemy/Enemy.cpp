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

	//���g�𒆐S�ɂ���
	m_targetLocalPos.SetCenterPos(m_rigidbody.GetPos());

	//�O������ݒ肷��
	m_targetLocalPos.SetFrontPos(m_rigidbody.GetPos() + (m_pGameManager->GetPlayerPos() - m_rigidbody.GetPos()).Normalize());

}

void Enemy::Update()
{
	//State�ɕω�������Εω�������
	if (m_pState != m_pState->m_pNextState)
	{
		m_pState = m_pState->m_pNextState;
	}

#ifdef _DEBUG

	PlayAnim();

	m_targetLocalPos.SetCenterPos(m_rigidbody.GetPos());


	m_targetLocalPos.SetLocalPos(m_targetLocalPos.ChangeWorldToLocal(m_pGameManager->GetPlayerPos()));

	MyEngine::Vector3 pos = m_targetLocalPos.GetLocalPos().Normalize();

	DrawFormatString(0, 128, GetColor(255, 255, 255), "�G�l�~�[���猩���v���C���[���W(X:%0.1f,Y:%0.1f,Z:%0.1f)", pos.x, pos.y, pos.z);


#endif // _DEBUG


	//State�̍X�V����
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

	DrawString(0, 48, "Enemy���Ȃɂ��ƂԂ�����", GetColor(255, 255, 255));

#endif // _DEBUG


}
