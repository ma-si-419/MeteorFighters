#include "CharacterStateJump.h"
#include "CharacterStateIdle.h"
#include "CharacterStateMove.h"
#include "CharacterStateDash.h"
#include "CharacterStateNormalAttack.h"
#include "CharacterBase.h"
#include "CapsuleColliderData.h"
#include "Physics.h"
#include "GameSceneConstant.h"
#include "Input.h"

namespace
{
	//�O�㍶�E�ړ����ǂ̂��炢���f�����邩
	constexpr float kMoveVecRate = 0.9f;

	//���n���̍d������
	constexpr int kLandingTime = 7;
}

CharacterStateJump::CharacterStateJump(std::shared_ptr<CharacterBase> character) :
	CharacterStateBase(character),
	m_attackKey("empty"),
	m_attackButtonPushTime(0)
{
}

void CharacterStateJump::StartJump(MyEngine::Vector3 moveVec)
{
	m_moveVec = (moveVec * kMoveVecRate) + MyEngine::Vector3(0.0f, GameSceneConstant::kJumpPower, 0.0f);
}

void CharacterStateJump::StartJump()
{
	StartJump(MyEngine::Vector3(0.0f, 0.0f, 0.0f));
}

void CharacterStateJump::Enter()
{
	//�ړ��x�N�g������������Ă��Ȃ����
	if (m_moveVec.y <= 0)
	{
		//�W�����v���ɂ���
		m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kJumping, true);
	}
	//�ړ��x�N�g������������Ă����
	else
	{
		m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kJumpStart, false);
	}

	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kJump;

}

void CharacterStateJump::Update()
{
	//�W�����v���n�߂�A�j���[�V���������Ă���
	if (m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kJumpStart)
	{
		//�A�j���[�V�������I�������
		if (m_pCharacter->IsEndAnim())
		{
			//�W�����v���̃A�j���[�V�����ɕω�������
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kJumping, true);
		}
	}
	//�d�͂�������
	m_moveVec.y += GameSceneConstant::kSkyGravityPower;
	if (m_moveVec.y < GameSceneConstant::kMaxFallSpeed)
	{
		//�������x�������Ȃ肷���Ȃ��悤��
		m_moveVec.y = GameSceneConstant::kMaxFallSpeed;
	}

	auto& input = MyEngine::Input::GetInstance();

	//�W�����v�{�^����������x��������󒆂Ɏ~�܂�(�X�e�[�g���ύX����)
	if (m_isPlayer && input.IsTrigger("RB"))
	{
		auto next = std::make_shared<CharacterStateMove>(m_pCharacter);

		ChangeState(next);
	}
	//�U���{�^����������Ă��Ȃ��Ƃ���
	if (m_attackKey == "empty")
	{
		//�i���{�^���������ꂽ��
		if (m_isPlayer && input.IsPress("X"))
		{
			m_attackKey = "X";
		}
		else if (m_isPlayer && input.IsPress("Y"))
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
		if (m_isPlayer && input.IsRelease(m_attackKey) ||
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
					if (input.GetStickInfo().leftStickY < -GameSceneConstant::kPhysicalAttackStickPower)
					{
						attackName = "UpCharge";
					}
					//�X�e�B�b�N�����ɌX���Ă�����
					else if (input.GetStickInfo().leftStickY > GameSceneConstant::kPhysicalAttackStickPower)
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

			next->SetAttack(m_attackKey, attackName);
			//State��Attack�ɕύX����
			ChangeState(next);
			return;
		}
	}

	//�_�b�V���{�^���������ꂽ��
	if (m_isPlayer && input.IsTrigger("A"))
	{

		MyEngine::Vector3 leftStickDir(input.GetStickInfo().leftStickX, 0, -input.GetStickInfo().leftStickY);

		leftStickDir = leftStickDir.Normalize();

		//�G�Ƃ̋�������_�b�V�����X�e�b�v�����f����
		//(�X�e�b�v���_�b�V�����̔����DashState�̒��ł��s��)
		//(�����ł�MP������邩���Ȃ����ADashState�ɂ͂��邩�ǂ����𔻒f����)
		if ((GetTargetPos() - m_pCharacter->GetPos()).Length() > GameSceneConstant::kNearLange)
		{
			//���������ꍇMp������ă_�b�V������
			if (m_pCharacter->SubMp(GameSceneConstant::kDashCost))
			{
				auto next = std::make_shared<CharacterStateDash>(m_pCharacter);

				if (leftStickDir.SqLength() < 0.01f)
				{
					next->SetMoveDir(MyEngine::Vector3(0, 0, 1));
				}
				else
				{
					next->SetMoveDir(leftStickDir);
				}

				ChangeState(next);
				return;
			}
		}
		//�G�Ƃ̋������߂��ꍇ
		else
		{
			//MP��������ɃX�e�b�v������
			auto next = std::make_shared<CharacterStateDash>(m_pCharacter);

			if (leftStickDir.SqLength() < 0.01f)
			{
				next->SetMoveDir(MyEngine::Vector3(0, 0, 1));
			}
			else
			{
				next->SetMoveDir(leftStickDir);
			}

			ChangeState(next);
			return;
		}
	}

	SetCharacterVelo(m_moveVec);

	//�n��ɂ���Ȃ�
	if (m_pCharacter->IsGround() && m_moveVec.y < 0)
	{
		//�A�C�h����Ԃɖ߂�
		auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		next->SetEndAnim(static_cast<int>(CharacterBase::AnimKind::kJumpEnd), kLandingTime);

		ChangeState(next);

		return;
	}
}

void CharacterStateJump::Exit()
{
}

void CharacterStateJump::OnCollide(std::shared_ptr<Collidable> collider)
{
}