#include "PlayerStateCharge.h"
#include "PlayerStateIdle.h"
#include "PlayerStateRush.h"
#include "Input.h"
#include "Player.h"

namespace
{
	//�`���[�W�I���A�j���[�V�����̎���
	constexpr int kEndChargeTime = 10;
}

PlayerStateCharge::PlayerStateCharge(std::shared_ptr<Player> player) :
	PlayerStateBase(player)
{
}

void PlayerStateCharge::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kCharge;
	m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kStartCharge,false);
}

void PlayerStateCharge::Update()
{
	auto& input = MyEngine::Input::GetInstance();

	m_pPlayer->ChargeMp();

	//�`���[�W�J�n���Đ����Ă���
	if (m_pPlayer->GetPlayAnimKind() == CharacterBase::AnimKind::kStartCharge)
	{
		//�A�j���[�V�������I����Ă�����
		if (m_pPlayer->IsEndAnim())
		{
			m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kInCharge,true);
		}
	}

	//���t�g�V�����_�[��������Ȃ��Ȃ�����A�C�h��State�ɖ߂�
	if (!input.IsPushTrigger(false))
	{
		auto next = std::make_shared<PlayerStateIdle>(m_pPlayer);

		next->SetEndAnim(static_cast<int>(CharacterBase::AnimKind::kEndCharge), kEndChargeTime);

		ChangeState(next);

		return;
	}

	//���t�g�V�����_�[�������Ȃ���A�{�^���������ꂽ��
	if (input.IsTrigger("A"))
	{
		MyEngine::Vector3 moveDir(input.GetStickInfo().leftStickX, 0, -input.GetStickInfo().leftStickY);

		//�ˌ�State�Ɉڍs����
		auto next = std::make_shared<PlayerStateRush>(m_pPlayer);

		if (moveDir.SqLength() > 0.01f)
		{
			moveDir = moveDir.Normalize();
			next->SetMoveDir(moveDir);

			ChangeState(next);

			return;
		}

		moveDir = MyEngine::Vector3(0,0,1);

		next->SetMoveDir(moveDir);

		ChangeState(next);

		return;
		
	}

	SetPlayerVelo(MyEngine::Vector3(0,0,0));


#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Charge", GetColor(255, 255, 255));

#endif // _DEBUG
}

void PlayerStateCharge::Exit()
{
}

void PlayerStateCharge::OnCollide(std::shared_ptr<Collidable> collider)
{
}
