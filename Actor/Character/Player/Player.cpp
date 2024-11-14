#include "Player.h"
#include "Input.h"
#include "GameCamera.h"
#include "GameManager.h"
#include "GameSceneConstant.h"
#include <cmath>
#include "PlayerStateIdle.h"

namespace
{
#ifdef _DEBUG

	constexpr float kJumpPower = 3.0f;
	constexpr float kGravityPower = 0.2f;

	const int kWhite = GetColor(255, 255, 255);
#endif // _DEBUG

	constexpr float kCharacterHeight = 4.5f;
}

Player::Player(CharacterKind kind) :
	CharacterBase(ObjectTag::kPlayer, kind)
{
}

Player::~Player()
{
}


void Player::Init()
{
	m_modelHandle = MV1LoadModel("data/model/Fighter.mv1");

	MV1SetScale(m_modelHandle, VGet(GameSceneConstant::kModelScale, GameSceneConstant::kModelScale, GameSceneConstant::kModelScale));

	Collidable::Init();

	m_nowHp = m_status.hp;
	m_nowMp = m_status.startMp;

	auto player = std::static_pointer_cast<Player>(shared_from_this());

	m_pState = std::make_shared<PlayerStateIdle>(player);
	m_pState->Enter();
}

void Player::Update()
{
	
	//ローカル座標の更新
	m_lookPos.SetCenterPos(m_rigidbody.GetPos());
	
	//Stateに変化があれば変化させる
	if (m_pState != m_pState->m_pNextState)
	{
		m_pState = m_pState->m_pNextState;
	}

	//Stateの更新処理
	m_pState->Update();


	//残像の更新
	int deleteNum = 0;

	for (auto& item : m_afterImageList)
	{
		item.nowOpacityRate -= item.DeleteSpeed;

		float rate = fmin(item.maxOpacityRate,item.nowOpacityRate);

		MV1SetOpacityRate(item.handle,rate);
		
		if (item.nowOpacityRate < 0.0f)
		{
			MV1DeleteModel(item.handle);

			deleteNum++;
		}
	}

	for (int i = 0; i < deleteNum; i++)
	{
		m_afterImageList.pop_front();
	}


	//アニメーションの更新
	PlayAnim();

	//描画座標の設定
	SetDrawPos(m_rigidbody.GetPos());

}
void Player::Draw()
{
	//残像の描画
	for (auto item : m_afterImageList)
	{
		MV1DrawModel(item.handle);
	}

	//キャラクターの描画
	MV1DrawModel(m_modelHandle);

#ifdef _DEBUG

	//DrawSphere3D(GetBackPos(GameSceneConstant::kEnemyBackPosDistance).CastVECTOR(), 3, 3, GetColor(255, 0, 255), GetColor(255, 0, 255), true);;

#endif // _DEBUG


}

void Player::OnCollide(std::shared_ptr<Collidable> collider)
{
#ifdef _DEBUG

	DrawString(0, 64, "Playerがなにかとぶつかった", GetColor(255, 255, 255));

#endif // _DEBUG
}
