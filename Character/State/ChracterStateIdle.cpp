#include "CharacterStateIdle.h"
#include "CharacterStateMove.h"
#include "CharacterStateNormalAttack.h"
#include "CharacterStateCharge.h"
#include "CharacterStateDash.h"
#include "CharacterStateRush.h"
#include "CharacterStateJump.h"
#include "CharacterStateGuard.h"
#include "Character.h"
#include "DxLib.h"
#include "Input.h"
#include "GameSceneConstant.h"

CharacterStateIdle::CharacterStateIdle(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
	m_attackKey("empty"),
	m_attackButtonPushTime(0),
	m_isPlayEndAnim(false),
	m_endAnimTime(0)
{
}

void CharacterStateIdle::SetEndAnim(int kind, int time)
{
	m_pCharacter->ChangeAnim(static_cast<Character::AnimKind>(kind), false);
	m_endAnimTime = time;
	m_isPlayEndAnim = true;
}

void CharacterStateIdle::SetEndAnim(int kind, int time, float blendSpeed)
{
	m_pCharacter->ChangeAnim(static_cast<Character::AnimKind>(kind), false, blendSpeed);
	m_endAnimTime = time;
	m_isPlayEndAnim = true;
}

void CharacterStateIdle::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kIdle;
}

void CharacterStateIdle::Update()
{
	auto input = GetCharacterInput();

#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Idle", GetColor(255, 255, 255));

#endif // _DEBUG

	//State�ɂ��鎞�Ԃ��v������
	m_time++;

	MyEngine::Vector3 velo;

	//�A�j���[�V�������I��鎞�ԂɂȂ��Ă����
	if (m_time > m_endAnimTime)
	{
		m_isPlayEndAnim = false;
		//�A�C�h���A�j���[�V�����łȂ����
		if (m_pCharacter->GetPlayAnimKind() != Character::AnimKind::kIdle)
		{
			//�A�C�h���A�j���[�V�����ɕς���
			m_pCharacter->ChangeAnim(Character::AnimKind::kIdle, true);
			//�ꉞ�Đ����x�����Z�b�g���Ă���
			m_pCharacter->SetAnimPlaySpeed();
		}
	}

	//�O�̃t���[���̏I���A�j���[�V�������Đ�����Ă����牺�̏��������ʂ�Ȃ�
	if (m_isPlayEndAnim) return;


	//�U���{�^����������Ă��Ȃ��Ƃ���
	if (m_attackKey == "empty")
	{
		//�i���{�^���������ꂽ��
		if (input->IsPress("X"))
		{
			m_attackKey = "X";
		}
		else if (input->IsPress("Y"))
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
		if (input->IsRelease(m_attackKey) ||
			m_attackButtonPushTime > GameSceneConstant::kChargeAttackTime)
		{
			//�`���[�W����Ă������ǂ�������
			bool isCharge = m_attackButtonPushTime > GameSceneConstant::kChargeAttackTime;
			//����State�̃|�C���^�쐬
			auto next = std::make_shared<CharacterStateNormalAttack>(m_pCharacter);

			//���̍U�����s������AttackState�ɓn��
			std::string attackName = "empty";

			//�`���[�W����Ă���
			if (isCharge)
			{
				//X�{�^����������Ă���
				if (m_attackKey == "X")
				{
					//�X�e�B�b�N����ɌX���Ă�����
					if (input->GetStickInfo().leftStickY < -GameSceneConstant::kPhysicalAttackStickPower)
					{
						attackName = "UpCharge";
					}
					//�X�e�B�b�N�����ɌX���Ă�����
					else if (input->GetStickInfo().leftStickY > GameSceneConstant::kPhysicalAttackStickPower)
					{
						attackName = "DownCharge";
					}
					//�X�e�B�b�N���X���Ă��Ȃ����
					else
					{
						attackName = "MiddleCharge";
					}
				}
				//Y�{�^����������Ă�����
				else if (m_attackKey == "Y")
				{
					attackName = "EnergyCharge";
				}
			}
			//�`���[�W����Ă��Ȃ����
			else
			{
				if (m_attackKey == "X")
				{
					attackName = "Low1";
				}
				else if (m_attackKey == "Y")
				{
					attackName = "Energy1";
				}
			}

			//�C�e�U���Ȃ�΋C�͂����炷
			if (attackName == "Energy1" || attackName == "EnergyCharge")
			{
				//���点�Ȃ���΍U�����Z�b�g���Ȃ�
				if (m_pCharacter->SubMp(GameSceneConstant::kEnergyAttackCost))
				{
					next->SetAttack(m_attackKey, attackName);

					//State��Attack�ɕύX����
					ChangeState(next);
					return;
				}
			}
			else
			{
				next->SetAttack(m_attackKey, attackName);


				//State��Attack�ɕύX����
				ChangeState(next);
				return;
			}
		}
	}

	//�ړ����͂�����Ă�����
	if (input->GetStickInfo().leftStickX != 0 ||
		input->GetStickInfo().leftStickY != 0)
	{
		//����State�̃|�C���^�쐬
		auto next = std::make_shared<CharacterStateMove>(m_pCharacter);
		//State��Move�ɕύX����
		ChangeState(next);
		return;
	}

	//��莞�ԃ��t�g�V�����_�[�{�^���������ꂽ��
	if (input->GetPushTriggerTime(false) > GameSceneConstant::kChargeStateChangeTime)
	{
		//����State�̃|�C���^�쐬
		auto next = std::make_shared<CharacterStateCharge>(m_pCharacter);
		//State��Charge�ɕύX����
		ChangeState(next);
		return;
	}

	//�_�b�V���{�^���������ꂽ��
	if (input->IsTrigger("A"))
	{
		//�ꏏ�Ƀ��t�g�V�����_�[��������Ă�����
		if (input->IsPushTrigger(false))
		{
			//�_�b�V���̃R�X�g�������
			if (m_pCharacter->SubMp(GameSceneConstant::kDashCost))
			{
				//�ˌ���ԂɈڍs����
				auto next = std::make_shared<CharacterStateRush>(m_pCharacter);

				next->SetMoveDir(MyEngine::Vector3(0.0f, 0.0f, 1.0f));

				ChangeState(next);
				return;
			}
		}

		//�G�Ƃ̋�������_�b�V�����X�e�b�v�����f����
		//(�X�e�b�v���_�b�V�����̔����DashState�̒��ł��s��)
		//(�����ł�MP������邩���Ȃ����ADashState�ɂ͂��邩�ǂ����𔻒f����)
		if ((GetTargetPos() - m_pCharacter->GetPos()).Length() > GameSceneConstant::kNearLange)
		{
			//���������ꍇMp������ă_�b�V������
			if (m_pCharacter->SubMp(GameSceneConstant::kDashCost))
			{
				auto next = std::make_shared<CharacterStateDash>(m_pCharacter);

				next->SetMoveDir(MyEngine::Vector3(0.0f, 0.0f, 1.0f));

				ChangeState(next);
				return;
			}
		}
		//�G�Ƃ̋������߂��ꍇ
		else
		{
			//MP��������ɃX�e�b�v������
			auto next = std::make_shared<CharacterStateDash>(m_pCharacter);

			next->SetMoveDir(MyEngine::Vector3(0.0f, 0.0f, 1.0f));

			ChangeState(next);
			return;
		}
	}

	//�n��ɂ���Ƃ���
	if (m_pCharacter->IsGround())
	{
		//�W�����v�{�^���������ꂽ��
		if (input->IsPress("RB"))
		{
			//�W�����vState�쐬
			auto next = std::make_shared<CharacterStateJump>(m_pCharacter);

			next->StartJump();

			ChangeState(next);

			return;
		}

		//�d�͂������Ă���
		velo.y += GameSceneConstant::kGroundGravityPower;

	}
	//�󒆂ɂ���Ƃ���
	else
	{
		//�㏸�{�^�������~�{�^���������ꂽ��
		if (input->IsPress("RB") ||
			input->IsPushTrigger(true))
		{
			//����State�̃|�C���^�쐬
			auto next = std::make_shared<CharacterStateMove>(m_pCharacter);
			//State��Move�ɕύX����
			ChangeState(next);
			return;
		}
	}

	//�K�[�h���͂�����Ă�����
	if (input->IsPress("B"))
	{
		//����State�̃|�C���^�쐬
		auto next = std::make_shared<CharacterStateGuard>(m_pCharacter);
		//State��Move�ɕύX����
		ChangeState(next);
		return;
	}

	//�A�C�h����Ԃ̎��͈ړ����Ȃ�
	SetCharacterVelo(velo);
}
void CharacterStateIdle::Exit()
{

}