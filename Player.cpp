#include "Player.h"
#include "Input.h"
namespace
{
#ifdef _DEBUG

	constexpr float kSpeed = 3.0f;
	constexpr float kJumpPower = 30.0f;
	constexpr float kGravityPower = 2.0f;

	const int kWhite = GetColor(255, 255, 255);

#endif // _DEBUG

}

Player::Player() :
	CharacterBase(ObjectTag::kPlayer)
{
	m_modelHandle = MV1LoadModel("data/model/protoFighter.mv1");
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
}

void Player::Update()
{

	MV1SetPosition(m_modelHandle, m_rigidbody.GetPos().CastVECTOR());

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
	if (input.IsPress("A") && isGround)
	{
		moveVec.y = kJumpPower;
		isGround = false;
	}	

	if ((m_rigidbody.GetPos() + moveVec).y < 0)
	{
		moveVec.y = 0;
		isGround = true;
	}

	DrawFormatString(0,16,kWhite,"Fighter�̍���:%0.1f",m_rigidbody.GetPos().y);

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
