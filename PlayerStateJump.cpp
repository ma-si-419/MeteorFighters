#include "PlayerStateJump.h"
#include "PlayerStateIdle.h"
#include "Player.h"
#include "CapsuleColliderData.h"
#include "Physics.h"

namespace
{
	//�W�����v��
	constexpr float kJumpPower = 6.0f;
	//�d�͂̋���
	constexpr float kGravityPower = -0.5f;
	//�������x�̌��E
	constexpr float kMaxFallSpeed = -4.0f;
	//�O�㍶�E�ړ����ǂ̂��炢���f�����邩
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
	//�ړ��x�N�g������������Ă��Ȃ����
	if (m_moveVec.y <= 0)
	{
		//�W�����v���ɂ���
		m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kJumping, true);
	}
	//�ړ��x�N�g������������Ă����
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
	//�W�����v���n�߂�A�j���[�V���������Ă���
	if (m_pPlayer->GetPlayAnimKind() == CharacterBase::AnimKind::kJumpStart)
	{
		//�A�j���[�V�������I�������
		if (m_pPlayer->IsEndAnim())
		{
			//�W�����v���̃A�j���[�V�����ɕω�������
			m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kJumping, true);
		}
	}
	//�d�͂�������
	m_moveVec.y += kGravityPower;
	if (m_moveVec.y < kMaxFallSpeed)
	{
		//�������x�������Ȃ肷���Ȃ��悤��
		m_moveVec.y = kMaxFallSpeed;
	}
	//�ړ��x�N�g�������������Ă���ꍇ
	if (m_moveVec.y < 0)
	{
		//�n�ʂƂ̓����蔻������ɐL�΂�
		m_pFallCollider->m_lange = MyEngine::Vector3(0.0f, m_moveVec.y, 0.0f);
	}

	//�n�ʂƂ̓����蔻����擾����
	if (Physics::GetInstance().GetHitObject(m_pFallCollider->m_endPos - m_pFallCollider->m_lange, m_pFallCollider, ObjectTag::kStage))
	{
		//�Ԃ����Ă�����
		m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kJumpEnd,false);
	}


	//�n��ɂ���Ȃ�
	if (m_pPlayer->IsGround())
	{
		//�A�C�h����Ԃɖ߂�
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
