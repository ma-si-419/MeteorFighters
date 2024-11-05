#include "PlayerStateIdle.h"
#include "PlayerStateNormalAttack.h"
#include "PlayerStateMove.h"
#include "PlayerStateJump.h"
#include "PlayerStateGuard.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"
#include "GameSceneConstant.h"
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
		if (input.IsRelease(m_attackKey) ||
			m_attackButtonPushTime > GameSceneConstant::kChargeAttackTime)
		{
			//�`���[�W����Ă������ǂ�������
			bool isCharge = m_attackButtonPushTime > GameSceneConstant::kChargeAttackTime;
			//����State�̃|�C���^�쐬
			auto next = std::make_shared<PlayerStateNormalAttack>(m_pPlayer);
			//���̍U�����s������AttackState�ɓn��
			next->SetAttack(m_attackKey, isCharge);
			//State��Attack�ɕύX����
			ChangeState(next);
			return;
		}
	}

	//�ړ����͂�����Ă�����
	if (input.GetStickInfo().leftStickX != 0 ||
		input.GetStickInfo().leftStickY != 0)
	{
		//����State�̃|�C���^�쐬
		auto next = std::make_shared<PlayerStateMove>(m_pPlayer);
		//State��Move�ɕύX����
		ChangeState(next);
		return;
	}

	//�n��ɂ���Ƃ���
	if (m_pPlayer->IsGround())
	{
		//�W�����v�{�^���������ꂽ��
		if (input.IsPress("RB"))
		{
			//�W�����vState�쐬
			auto next = std::make_shared<PlayerStateJump>(m_pPlayer);

			next->StartJump();

			ChangeState(next);

			return;
		}


	}
	//�󒆂ɂ���Ƃ���
	else
	{
		//�㏸�{�^�������~�{�^���������ꂽ��
		if (input.IsPress("RB")||
			input.IsPushTrigger(true))
		{
			//����State�̃|�C���^�쐬
			auto next = std::make_shared<PlayerStateMove>(m_pPlayer);
			//State��Move�ɕύX����
			ChangeState(next);
			return;
		}
	}

	//�K�[�h���͂�����Ă�����
	if (input.IsPress("B"))
	{
		//����State�̃|�C���^�쐬
		auto next = std::make_shared<PlayerStateGuard>(m_pPlayer);
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
