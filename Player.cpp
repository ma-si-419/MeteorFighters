#include "Player.h"
#include "Input.h"
#include "GameCamera.h"
#include "GameManager.h"
#include <cmath>

namespace
{
#ifdef _DEBUG

	constexpr float kSpeed = 7.0f;
	constexpr float kJumpPower = 30.0f;
	constexpr float kGravityPower = 2.0f;

	const int kWhite = GetColor(255, 255, 255);

#endif // _DEBUG

}

Player::Player() :
	CharacterBase(ObjectTag::kPlayer)
{
	m_modelHandle = MV1LoadModel("data/model/protoFighter.mv1");

	m_camera = std::make_shared<GameCamera>();

#ifdef _DEBUG

	isGround = true;

#endif // _DEBUG
}

Player::~Player()
{
}


void Player::Init()
{
	Collidable::Init();
	m_camera->Init(m_rigidbody.GetPos());
}

void Player::Update()
{

	//�|�W�V�����̐ݒ�
	MV1SetPosition(m_modelHandle, m_rigidbody.GetPos().CastVECTOR());
	m_camera->SetPlayerPosAndTarget(m_rigidbody.GetPos(), m_pGameManager->GetEnemyPos());
	
	//�v���C���[����G�l�~�[�ւ̃x�N�g��
	MyEngine::Vector3 playerToTarget = m_pGameManager->GetEnemyPos() - m_rigidbody.GetPos();

	m_camera->SetFrontPos(m_rigidbody.GetPos() + playerToTarget);


	m_camera->Update();

	//�C���v�b�g���Ǘ����Ă���N���X�̎Q��
	MyEngine::Input& input = MyEngine::Input::GetInstance();
	//�ړ��x�N�g��
	MyEngine::Vector3 moveVec;

	//�X�e�B�b�N�̏��擾
	MyEngine::Input::StickInfo stick = input.GetStickInfo();

	//���X�e�B�b�N�̌X���擾
	MyEngine::Vector3 leftStickDir(stick.leftStickX, 0, -stick.leftStickY);
	//�ړ��x�N�g����0����Ȃ����
	if (leftStickDir.sqLength() > 0.01)
	{
		//�ړ�����
		MyEngine::Vector3 moveDir = leftStickDir.Normalize();

		//�G�l�~�[�̕����Ɉړ���������]������
		float vX = m_pGameManager->GetEnemyPos().x - m_rigidbody.GetPos().x;
		float vZ = m_pGameManager->GetEnemyPos().z - m_rigidbody.GetPos().z;

		float angle = std::atan2f(vX,vZ);

		MyEngine::Vector3 rotation(0.0f,angle,0.0f);

		MATRIX mat = rotation.GetRotationMat();

		moveDir = moveDir.MatTransform(mat);

		//�ړ������ɃX�s�[�h��������
		moveVec = moveDir * kSpeed;
	}
#ifdef _DEBUG

	//�W�����v���Ă�����
	if (!isGround)
	{
		moveVec.y = m_rigidbody.GetVelo().y;
		moveVec.y -= kGravityPower;
	}

	//A�{�^���������ꂽ��
	if (input.IsTrigger("A") && isGround)
	{
		moveVec.y = kJumpPower;
		isGround = false;
	}	

	if ((m_rigidbody.GetPos() + moveVec).y < 0)
	{
		moveVec.y = 0;
		isGround = true;
	}

	//�G�̕����������悤�ɂ���
	MV1SetRotationZYAxis(m_modelHandle, (m_rigidbody.GetPos() - m_pGameManager->GetEnemyPos()).CastVECTOR(), VGet(0.0f, 1.0f, 0.0f), 0.0f);

#endif // _DEBUG

	//�ړ��x�N�g����ݒ肷��
	m_rigidbody.SetVelo(moveVec);

}
void Player::Draw()
{
	MV1DrawModel(m_modelHandle);
}

void Player::OnCollide(std::shared_ptr<Collidable> collider)
{
}
