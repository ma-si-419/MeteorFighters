#include "Player.h"
#include "Input.h"
#include "GameCamera.h"
#include "GameManager.h"
#include <cmath>
#include "PlayerStateIdle.h"

namespace
{
#ifdef _DEBUG

	constexpr float kJumpPower = 3.0f;
	constexpr float kGravityPower = 0.2f;

	const int kWhite = GetColor(255, 255, 255);
#endif // _DEBUG

	constexpr float kModelScale = 0.1f;

}

Player::Player(CharacterKind kind) :
	CharacterBase(ObjectTag::kPlayer,kind)
{
}

Player::~Player()
{
}


void Player::Init()
{
	m_modelHandle = MV1LoadModel("data/model/Fighter.mv1");

	MV1SetScale(m_modelHandle, VGet(kModelScale, kModelScale, kModelScale));

	m_camera = std::make_shared<GameCamera>();

	Collidable::Init();
	m_camera->Init(m_rigidbody.GetPos());

	m_nowHp = m_status.hp;
	m_nowMp = m_status.startMp;

	auto player = std::static_pointer_cast<Player>(shared_from_this());

	m_pState = std::make_shared<PlayerStateIdle>(player);
	m_pState->Enter();
}

void Player::Update()
{
	//State�ɕω�������Εω�������
	if (m_pState->GetKind() != m_pState->m_pNextState->GetKind())
	{
		m_pState = m_pState->m_pNextState;
	}
	
	//State�̍X�V����
	m_pState->Update();

	//�A�j���[�V�����̍X�V
	PlayAnim();


	//�|�W�V�����̐ݒ�
	MV1SetPosition(m_modelHandle, m_rigidbody.GetPos().CastVECTOR());
	
	MyEngine::Vector3 cameraTargetPos = m_pGameManager->GetEnemyPos();

	cameraTargetPos.y = m_rigidbody.GetPos().y;

	m_camera->SetPlayerPosAndTarget(m_rigidbody.GetPos(),cameraTargetPos);
	
	//�v���C���[����G�l�~�[�ւ̃x�N�g��
	MyEngine::Vector3 playerToTarget = (m_pGameManager->GetEnemyPos() - m_rigidbody.GetPos()).Normalize();

#ifdef _DEBUG

	LocalPos enemy;

	enemy.SetCenterPos(m_rigidbody.GetPos());

	enemy.SetFrontPos(m_rigidbody.GetPos() + playerToTarget);

	enemy.SetLocalPos(enemy.ChangeWorldToLocal(m_pGameManager->GetEnemyPos()));

	MyEngine::Vector3 pos = enemy.GetLocalPos();

	DrawFormatString(0,112,GetColor(255,255,255),"�v���C���[���猩���G�l�~�[���W(X:%0.1f,Y:%0.1f,Z:%0.1f)",pos.x, pos.y, pos.z);

#endif // _DEBUG



	//�J�����̐��ʕ�����ݒ�
	m_camera->SetPlayerFrontPos(m_rigidbody.GetPos() + playerToTarget);

	//�J�����̍X�V
	m_camera->Update();

	
	//�G�̕����������悤�ɂ���
	MV1SetRotationZYAxis(m_modelHandle, (m_rigidbody.GetPos() - m_pGameManager->GetEnemyPos()).CastVECTOR(), VGet(0.0f, 1.0f, 0.0f), 0.0f);
}
void Player::Draw()
{
	MV1DrawModel(m_modelHandle);
}

void Player::OnCollide(std::shared_ptr<Collidable> collider)
{
#ifdef _DEBUG

	DrawString(0, 64, "Player���Ȃɂ��ƂԂ�����", GetColor(255, 255, 255));

#endif // _DEBUG
}
