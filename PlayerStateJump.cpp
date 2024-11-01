#include "PlayerStateJump.h"
#include "PlayerStateIdle.h"
#include "Player.h"
#include "CapsuleColliderData.h"
#include "Physics.h"

namespace
{
	//ジャンプ力
	constexpr float kJumpPower = 6.0f;
	//重力の強さ
	constexpr float kGravityPower = -0.5f;
	//落下速度の限界
	constexpr float kMaxFallSpeed = -4.0f;
	//前後左右移動をどのくらい反映させるか
	constexpr float kMoveVecRate = 0.3f;
}

PlayerStateJump::PlayerStateJump(std::shared_ptr<Player> player) :
	PlayerStateBase(player)
{
}

void PlayerStateJump::StartJump(MyEngine::Vector3 moveVec)
{
	m_moveVec = (moveVec * kMoveVecRate) + MyEngine::Vector3(0.0f, kJumpPower, 0.0f);
}

void PlayerStateJump::Enter()
{
	//移動ベクトルが上を向いていなければ
	if (m_moveVec.y <= 0)
	{
		//ジャンプ中にする
		m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kJumping, true);
	}
	//移動ベクトルが上を向いていれば
	else
	{
		m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kJumpStart, false);
	}

	//m_pFallCollider = std::make_shared<CapsuleColliderData>(ObjectTag::kPlayer, ColliderData::Kind::kCapsule);

	m_pFallCollider->m_radius = m_pPlayer->GetRadius();

	m_pFallCollider->m_lange = MyEngine::Vector3(0, 0, 0);


}

void PlayerStateJump::Update()
{
	//ジャンプを始めるアニメーションをしていて
	if (m_pPlayer->GetPlayAnimKind() == CharacterBase::AnimKind::kJumpStart)
	{
		//アニメーションが終わったら
		if (m_pPlayer->IsEndAnim())
		{
			//ジャンプ中のアニメーションに変化させる
			m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kJumping, true);
		}
	}
	//重力をかける
	m_moveVec.y += kGravityPower;
	if (m_moveVec.y < kMaxFallSpeed)
	{
		//落下速度が速くなりすぎないように
		m_moveVec.y = kMaxFallSpeed;
	}
	//移動ベクトルが下を向いている場合
	if (m_moveVec.y < 0)
	{
		//地面との当たり判定を下に伸ばす
		m_pFallCollider->m_lange = MyEngine::Vector3(0.0f, m_moveVec.y, 0.0f);
	}

	//地面との当たり判定を取得する
	if (Physics::GetInstance().GetHitObject(m_pFallCollider->m_endPos - m_pFallCollider->m_lange, m_pFallCollider, ObjectTag::kStage))
	{
		//ぶつかっていたら
		m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kJumpEnd,false);
	}


	//地上にいるなら
	if (m_pPlayer->IsGround())
	{
		//アイドル状態に戻る
		std::shared_ptr<PlayerStateIdle>next = std::make_shared<PlayerStateIdle>(m_pPlayer);
		ChangeState(next);
	}
}

void PlayerStateJump::Exit()
{
}

void PlayerStateJump::OnCollide(std::shared_ptr<Collidable> collider)
{
}
