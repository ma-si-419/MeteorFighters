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

	//ポジションの設定
	MV1SetPosition(m_modelHandle, m_rigidbody.GetPos().CastVECTOR());
	m_camera->SetPlayerPosAndTarget(m_rigidbody.GetPos(), m_pGameManager->GetEnemyPos());
	
	//プレイヤーからエネミーへのベクトル
	MyEngine::Vector3 playerToTarget = m_pGameManager->GetEnemyPos() - m_rigidbody.GetPos();

	m_camera->SetFrontPos(m_rigidbody.GetPos() + playerToTarget);


	m_camera->Update();

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

		//エネミーの方向に移動方向を回転させる
		float vX = m_pGameManager->GetEnemyPos().x - m_rigidbody.GetPos().x;
		float vZ = m_pGameManager->GetEnemyPos().z - m_rigidbody.GetPos().z;

		float angle = std::atan2f(vX,vZ);

		MyEngine::Vector3 rotation(0.0f,angle,0.0f);

		MATRIX mat = rotation.GetRotationMat();

		moveDir = moveDir.MatTransform(mat);

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

	//敵の方向を向くようにする
	MV1SetRotationZYAxis(m_modelHandle, (m_rigidbody.GetPos() - m_pGameManager->GetEnemyPos()).CastVECTOR(), VGet(0.0f, 1.0f, 0.0f), 0.0f);

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
