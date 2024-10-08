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

}

Player::Player() :
	CharacterBase(ObjectTag::kPlayer)
{
	m_modelHandle = MV1LoadModel("data/model/Fighter.mv1");

	MV1SetScale(m_modelHandle, VGet(0.1f, 0.1f, 0.1f));

	m_camera = std::make_shared<GameCamera>();
}

Player::~Player()
{
}


void Player::Init()
{
	Collidable::Init();
	m_camera->Init(m_rigidbody.GetPos());

	auto player = std::static_pointer_cast<Player>(shared_from_this());

	m_pState = std::make_shared<PlayerStateIdle>(player);
	m_pState->Enter();
}

void Player::Update()
{
	//Stateに変化があれば変化させる
	if (m_pState->GetKind() != m_pState->m_pNextState->GetKind())
	{
		m_pState = m_pState->m_pNextState;
	}
	
	//Stateの更新処理
	m_pState->Update();


	//ポジションの設定
	MV1SetPosition(m_modelHandle, m_rigidbody.GetPos().CastVECTOR());
	m_camera->SetPlayerPosAndTarget(m_rigidbody.GetPos(), m_pGameManager->GetEnemyPos());
	
	//プレイヤーからエネミーへのベクトル
	MyEngine::Vector3 playerToTarget = m_pGameManager->GetEnemyPos() - m_rigidbody.GetPos();

	m_camera->SetFrontPos(m_rigidbody.GetPos() + playerToTarget);


	m_camera->Update();

	
	//敵の方向を向くようにする
	MV1SetRotationZYAxis(m_modelHandle, (m_rigidbody.GetPos() - m_pGameManager->GetEnemyPos()).CastVECTOR(), VGet(0.0f, 1.0f, 0.0f), 0.0f);
}
void Player::Draw()
{
	MV1DrawModel(m_modelHandle);
}

void Player::OnCollide(std::shared_ptr<Collidable> collider)
{
#ifdef _DEBUG

	DrawString(0, 64, "Playerがなにかとぶつかった", GetColor(255, 255, 255));

#endif // _DEBUG
}
