#include "PlayerStateIdle.h"
#include "PlayerStateNormalAttack.h"
#include "PlayerStateMove.h"
#include "PlayerStateJump.h"
#include "PlayerStateDash.h"
#include "PlayerStateRush.h"
#include "PlayerStateGuard.h"
#include "PlayerStateCharge.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"
#include "GameSceneConstant.h"
PlayerStateIdle::PlayerStateIdle(std::shared_ptr<Player> player) :
	PlayerStateBase(player),
	m_attackKey("empty"),
	m_attackButtonPushTime(0),
	m_isPlayEndAnim(false),
	m_endAnimTime(0)
{
}

void PlayerStateIdle::SetEndAnim(int kind, int time)
{
	m_pPlayer->ChangeAnim(static_cast<CharacterBase::AnimKind>(kind), false);
	m_endAnimTime = time;
	m_isPlayEndAnim = true;
}

void PlayerStateIdle::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kIdle;
}

void PlayerStateIdle::Update()
{

#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Idle", GetColor(255, 255, 255));

#endif // _DEBUG


	//State�ɂ��鎞�Ԃ��v������
	m_time++;

	//�A�C�h����Ԃ̎��͈ړ����Ȃ�
	SetPlayerVelo(MyEngine::Vector3(0, 0, 0));
	
	//�A�j���[�V�������I��鎞�ԂɂȂ��Ă����
	if (m_time > m_endAnimTime)
	{
		m_isPlayEndAnim = false;
		//�A�C�h���A�j���[�V�����łȂ����
		if (m_pPlayer->GetPlayAnimKind() != CharacterBase::AnimKind::kIdle)
		{
			m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kIdle, true);
		}
	}

	//�O�̃t���[���̏I���A�j���[�V�������Đ�����Ă����牺�̏��������ʂ�Ȃ�
	if (m_isPlayEndAnim) return;

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

	//��莞�ԃ��t�g�V�����_�[�{�^���������ꂽ��
	if (input.GetPushTriggerTime(false) > GameSceneConstant::kChargeStateChangeTime)
	{
		//����State�̃|�C���^�쐬
		auto next = std::make_shared<PlayerStateCharge>(m_pPlayer);
		//State��Charge�ɕύX����
		ChangeState(next);
		return;
	}

	//�_�b�V���{�^���������ꂽ��
	if (input.IsTrigger("A"))
	{
		//�ꏏ�Ƀ��t�g�V�����_�[��������Ă�����
		if (input.IsPushTrigger(false))
		{
			//�_�b�V���̃R�X�g�������
			if (m_pPlayer->SubMp(GameSceneConstant::kDashCost))
			{
				//�ˌ���ԂɈڍs����
				auto next = std::make_shared<PlayerStateRush>(m_pPlayer);

				next->SetMoveDir(MyEngine::Vector3(0,0,1));

				ChangeState(next);
				return;
			}
		}

		//�G�Ƃ̋�������_�b�V�����X�e�b�v�����f����
		//(�X�e�b�v���_�b�V�����̔����DashState�̒��ł��s��)
		//(�����ł�MP������邩���Ȃ����ADashState�ɂ͂��邩�ǂ����𔻒f����)
		if ((GetEnemyPos() - m_pPlayer->GetPos()).Length() > GameSceneConstant::kNearLange)
		{
			//���������ꍇMp������ă_�b�V������
			if (m_pPlayer->SubMp(GameSceneConstant::kDashCost))
			{
				auto next = std::make_shared<PlayerStateDash>(m_pPlayer);

				next->SetMoveDir(MyEngine::Vector3(0.0f, 0.0f, 1.0f));

				ChangeState(next);
				return;
			}
		}
		//�G�Ƃ̋������߂��ꍇ
		else
		{
			//MP��������ɃX�e�b�v������
			auto next = std::make_shared<PlayerStateDash>(m_pPlayer);

			next->SetMoveDir(MyEngine::Vector3(0.0f, 0.0f, 1.0f));

			ChangeState(next);
			return;
		}
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
		if (input.IsPress("RB") ||
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


}
void PlayerStateIdle::Exit()
{

}

void PlayerStateIdle::OnCollide(std::shared_ptr<Collidable> collider)
{
}
