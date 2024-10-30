#include "PlayerStateIdle.h"
#include "PlayerStateNormalAttack.h"
#include "PlayerStateMove.h"
#include "PlayerStateGuard.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"

namespace
{
	constexpr int kChargeAttackTime = 15;
}

PlayerStateIdle::PlayerStateIdle(std::shared_ptr<Player> player) :
	PlayerStateBase(player),
	m_attackKey("empty"),
	m_attackButtonPushTime(0)
{
}

void PlayerStateIdle::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kIdle;
	m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kIdle, true);
}

void PlayerStateIdle::Update()
{

#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Idle", GetColor(255, 255, 255));

	//if (MyEngine::Input::GetInstance().IsTrigger("A"))
	//{
	//	ChangeState(CharacterStateKind::kAttack);
	//}
	//else if (MyEngine::Input::GetInstance().IsTrigger("B"))
	//{
	//	ChangeState(CharacterStateKind::kGuard);
	//}
	//else if (MyEngine::Input::GetInstance().IsTrigger("X"))
	//{
	//	ChangeState(CharacterStateKind::kHitAttack);
	//}
	//else if (MyEngine::Input::GetInstance().IsTrigger("Y"))
	//{
	//	ChangeState(CharacterStateKind::kMove);
	//}
	//else if (MyEngine::Input::GetInstance().IsTrigger("Lb"))
	//{
	//	ChangeState(CharacterStateKind::kButtonBashing);
	//}
	//else if (MyEngine::Input::GetInstance().IsTrigger("Rb"))
	//{
	//	ChangeState(CharacterStateKind::kRush);
	//}
#endif // _DEBUG

	auto& input = MyEngine::Input::GetInstance();

	//�U���{�^����������Ă��Ȃ��Ƃ���
	if (m_attackKey == "empty")
	{
		//�i���{�^���������ꂽ��
		if (input.IsPress("X"))
		{
			m_attackKey = "X";
		}
		else if (input.IsPress("Y"))
		{
			m_attackKey = "Y";
		}
	}
	//�U���{�^����������Ă�����
	else
	{
		//�����Ă���t���[�����𐔂���
		m_attackButtonPushTime++;

		//�����Ă����{�^���������ꂽ��
		if (input.IsRelease(m_attackKey)||
			m_attackButtonPushTime > kChargeAttackTime)
		{
			//�`���[�W����Ă������ǂ�������
			bool isCharge = m_attackButtonPushTime > kChargeAttackTime;
			//����State�̃|�C���^�쐬
			std::shared_ptr<PlayerStateNormalAttack> next = std::make_shared<PlayerStateNormalAttack>(m_pPlayer);
			//���̍U�����s������AttackState�ɓn��
			next->SetAttack(m_attackKey,isCharge);
			//State��Attack�ɕύX����
			ChangeState(next);
			return;
		}
	}

	//�ړ����͂�����Ă�����
	if (input.GetStickInfo().leftStickX != 0 ||
		input.GetStickInfo().leftStickY != 0 ||
		input.IsPress("RB") ||
		input.IsPushTrigger(true))
	{
		//����State�̃|�C���^�쐬
		std::shared_ptr<PlayerStateMove> next = std::make_shared<PlayerStateMove>(m_pPlayer);
		//State��Move�ɕύX����
		ChangeState(next);
		return;
	}

	//�K�[�h���͂�����Ă�����
	if (input.IsPress("B"))
	{
		//����State�̃|�C���^�쐬
		std::shared_ptr<PlayerStateGuard> next = std::make_shared<PlayerStateGuard>(m_pPlayer);
		//State��Move�ɕύX����
		ChangeState(next);
		return;
	}

	SetPlayerVelo(MyEngine::Vector3(0, 0, 0));

}
void PlayerStateIdle::Exit()
{

}

void PlayerStateIdle::OnCollide(std::shared_ptr<Collidable> collider)
{
}
