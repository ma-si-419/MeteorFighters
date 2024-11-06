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

	constexpr int kOnBlurDrawModelNum = 2;

	constexpr int kOnBlurShiftLange = 1;

	constexpr float kCharacterHeight = 4.5f;

	constexpr float kBlurLangeScale = 0.01f;

	constexpr float kModelScale = 0.1f;

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

	MV1SetScale(m_modelHandle, VGet(kModelScale, kModelScale, kModelScale));

	Collidable::Init();

	m_nowHp = m_status.hp;
	m_nowMp = m_status.startMp;

	auto player = std::static_pointer_cast<Player>(shared_from_this());

	m_pState = std::make_shared<PlayerStateIdle>(player);
	m_pState->Enter();
}

void Player::Update()
{
	//Stateに変化があれば変化させる
	if (m_pState != m_pState->m_pNextState)
	{
		m_pState = m_pState->m_pNextState;
	}

	//Stateの更新処理
	m_pState->Update();

	//アニメーションの更新
	PlayAnim();


	//描画座標の設定
	SetDrawPos(m_rigidbody.GetPos());

}
void Player::Draw()
{
	//ぼかす時の処理
	if (m_isBlur)
	{
		for (int i = 0; i < kOnBlurDrawModelNum; i++)
		{
			int handle = MV1DuplicateModel(m_modelHandle);
			MyEngine::Vector3 drawPos = m_rigidbody.GetPos();
			MV1SetPosition(handle,drawPos.CastVECTOR());
			MV1SetRotationZYAxis(handle, (drawPos - m_lookPos).CastVECTOR(),VGet(0.0f,1.0f,0.0f),0.0f);
			drawPos.y -= kCharacterHeight;
			drawPos.x += static_cast<float>(GetRand(kOnBlurShiftLange) - static_cast<int>(kOnBlurShiftLange * 0.5f)) * kBlurLangeScale;
			drawPos.y += static_cast<float>(GetRand(kOnBlurShiftLange) - static_cast<int>(kOnBlurShiftLange * 0.5f)) * kBlurLangeScale;
			drawPos.z += static_cast<float>(GetRand(kOnBlurShiftLange) - static_cast<int>(kOnBlurShiftLange * 0.5f)) * kBlurLangeScale;
			MV1SetPosition(handle,drawPos.CastVECTOR());
			MV1SetScale(handle, VGet(kModelScale, kModelScale, kModelScale));
			MV1AttachAnim(handle,MV1GetAttachAnim(m_modelHandle,m_lastAnim));
			MV1AttachAnim(handle,MV1GetAttachAnim(m_modelHandle,m_attachAnim));
			MV1SetAttachAnimTime(handle,m_lastAnim,m_lastPlayAnimTime);
			MV1SetAttachAnimTime(handle,m_attachAnim,m_nowPlayAnimTime);
			MV1SetAttachAnimBlendRate(handle,m_lastAnim,1.0 - m_animBlendRate);
			MV1SetAttachAnimBlendRate(handle,m_attachAnim,m_animBlendRate);
			MV1SetOpacityRate(handle,1.0 / kOnBlurDrawModelNum);
			MV1DrawModel(handle);
			MV1DeleteModel(handle);
		}
	}
	//通常時の処理
	else
	{
		MV1DrawModel(m_modelHandle);
	}
}

void Player::OnCollide(std::shared_ptr<Collidable> collider)
{
#ifdef _DEBUG

	DrawString(0, 64, "Playerがなにかとぶつかった", GetColor(255, 255, 255));

#endif // _DEBUG
}
