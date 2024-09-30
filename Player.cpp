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

	//インプットを管理しているクラスの参照
	MyEngine::Input& input = MyEngine::Input::GetInstance();
	//移動ベクトル
	MyEngine::Vector3 moveVec;

	//スティックの情報取得
	MyEngine::Input::StickInfo stick = input.GetStickInfo();

	//左スティックの傾き取得
	MyEngine::Vector3 leftStickDir(stick.leftStickX, 0, -stick.leftStickY);
	//移動ベクトルが0じゃなければ
	if (leftStickDir.sqLength() > 0.01)
	{
		//移動方向
		MyEngine::Vector3 moveDir = leftStickDir.Normalize();

		//移動方向にスピードをかける
		moveVec = moveDir * kSpeed;
	}
#ifdef _DEBUG

	//ジャンプしていたら
	if (!isGround)
	{
		moveVec.y = m_rigidbody.GetVelo().y;
		moveVec.y -= kGravityPower;
	}

	//Aボタンが押されたら
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

	DrawFormatString(0,16,kWhite,"Fighterの高さ:%0.1f",m_rigidbody.GetPos().y);

#endif // _DEBUG

	//移動ベクトルを設定する
	m_rigidbody.SetVelo(moveVec);

}
void Player::Draw()
{
	MV1DrawModel(m_modelHandle);
}

void Player::OnCollide(std::shared_ptr<Collidable> collider)
{
}
