#include "CharacterStateMove.h"
#include "CharacterStateIdle.h"
#include "CharacterStateDash.h"
#include "CharacterStateRush.h"
#include "CharacterStateJump.h"
#include "CharacterStateCharge.h"
#include "CharacterStateNormalAttack.h"
#include "CharacterStateJump.h"
#include "GameSceneConstant.h"
#include "DxLib.h"
#include "Input.h"
#include "CharacterBase.h"
#include <cmath>

namespace
{
	//�ړ����x��x�����銄��
	constexpr float kSlowMoveSpeed = 0.6f;
	//����Ɉړ����x��x�����鋗��
	constexpr float kNearestLange = 10.0f;
}

CharacterStateMove::CharacterStateMove(std::shared_ptr<CharacterBase> character) :
	CharacterStateBase(character),
	m_attackButtonPushTime(0.0f),
	m_attackKey("empty"),
	m_isFloat(false),
	m_gravityPower(0.0f),
	m_isLastGround(false)
{
	m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kSkyIdle, true);
}

void CharacterStateMove::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kMove;
	if (m_pCharacter->IsGround())
	{
		m_isFloat = false;
	}
	else
	{
		m_isFloat = true;
	}
}

void CharacterStateMove::Update()
{
	//�v���C���[����G�l�~�[�ւ̃x�N�g��
	MyEngine::Vector3 playerToTarget = GetTargetPos() - m_pCharacter->GetPos();

	//�C���v�b�g���Ǘ����Ă���N���X�̎Q��
	MyEngine::Input& input = MyEngine::Input::GetInstance();
	//�ړ��x�N�g��
	MyEngine::Vector3 velo;
	//�ړ������x�N�g��
	MyEngine::Vector3 dir;

	MyEngine::Vector3 inputDir;

	if (m_isPlayer)
	{
		//�X�e�B�b�N�̏��擾
		MyEngine::Input::StickInfo stick = input.GetStickInfo();

		//���X�e�B�b�N�̌X���擾
		inputDir = MyEngine::Vector3(stick.leftStickX, 0, -stick.leftStickY);
	}
	//�ړ��x�N�g����0����Ȃ����
	if (inputDir.SqLength() > 0.001)
	{

		inputDir = inputDir.Normalize();

		//�ړ�����
		dir = inputDir;

		//�G�l�~�[�̕����Ɉړ���������]������
		float vX = GetTargetPos().x - m_pCharacter->GetPos().x;
		float vZ = GetTargetPos().z - m_pCharacter->GetPos().z;

		float yAngle = std::atan2f(vX, vZ);

		MyEngine::Vector3 rotation;

		rotation = MyEngine::Vector3(0.0f, yAngle, 0.0f);

		MATRIX mat = rotation.GetRotationMat();

		dir = dir.MatTransform(mat);

		MyEngine::Vector3 toTarget = (GetTargetPos() - m_pCharacter->GetPos());
		MyEngine::Vector3 toTargetDir = toTarget.Normalize();

		//�󒆂ɂ��đO���͂���Ă�����
		if (inputDir.z > 0 && m_isFloat)
		{
			float frontRate = inputDir.z;

			dir = dir * (1.0 - frontRate) + toTargetDir * frontRate;
		}

		//�ړ����x
		float speed = GetSpeed();

		//���ȏ�G�ɋ߂��Ȃ�ƈړ����x��x������
		if (toTarget.Length() < GameSceneConstant::kNearLange)
		{
			speed *= kSlowMoveSpeed;
		}

		//�ړ������ɃX�s�[�h��������
		velo = dir * speed;
	}

	//���ɕ����Ă��Ȃ��ꍇ
	if (!m_isFloat)
	{
		m_gravityPower += GameSceneConstant::kSkyGravityPower;

		if (m_gravityPower < GameSceneConstant::kMaxFallSpeed)
		{
			m_gravityPower = GameSceneConstant::kMaxFallSpeed;
		}

		velo.y += m_gravityPower;

		//�n��ɂ��Ȃ�
		if (!m_pCharacter->IsGround())
		{

			//�v���C���̃A�j���[�V�������W�����v���łȂ����
			if (!(m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kJumping))
			{
				//�A�j���[�V������ύX����
				m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kJumping, true);
			}
		}
	}

	//�_�b�V���{�^���������ꂽ��
	if (m_isPlayer && input.IsTrigger("A"))
	{
		//�ꏏ�Ƀ��t�g�V�����_�[��������Ă�����
		if (m_isPlayer && input.IsPushTrigger(false))
		{
			//�_�b�V���̃R�X�g�������
			if (m_pCharacter->SubMp(GameSceneConstant::kDashCost))
			{
				//�ˌ���ԂɈڍs����
				auto next = std::make_shared<CharacterStateRush>(m_pCharacter);

				next->SetMoveDir(inputDir);

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

				next->SetMoveDir(inputDir);

				ChangeState(next);
				return;
			}
		}
		//�G�Ƃ̋������߂��ꍇ
		else
		{
			//MP��������ɃX�e�b�v������
			auto next = std::make_shared<CharacterStateDash>(m_pCharacter);

			next->SetMoveDir(inputDir);

			ChangeState(next);
			return;
		}
	}


	//�n���
	if (m_pCharacter->IsGround())
	{

		//�W�����v�{�^���������ꂽ��
		if (m_isPlayer && input.IsTrigger("RB"))
		{
			//�W�����vState�Ɉڍs����
			auto next = std::make_shared<CharacterStateJump>(m_pCharacter);

			//���̈ړ��x�N�g����n��
			next->StartJump(velo);

			//�X�e�[�g��ύX����
			ChangeState(next);

			return;
		}

		m_gravityPower = GameSceneConstant::kGroundGravityPower;

		m_isFloat = false;

		MyEngine::Vector3 groundVelo = velo;

		groundVelo.y = 0;

		//�ړ����Ă����
		if (groundVelo.SqLength() > 0.01f)
		{
			//�v���C���̃A�j���[�V�������n��ړ��łȂ����
			if (!(m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kRun))
			{
				//�A�j���[�V������ύX����
				m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kRun, true);
			}

			auto frontPos = m_pCharacter->GetPos() + groundVelo;

			frontPos.y = m_pCharacter->GetPos().y;

			m_pCharacter->SetFrontPos(frontPos);
		}
		//�ړ����Ă��Ȃ����
		else
		{
			//�A�C�h����Ԃɖ߂�
			std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);

			ChangeState(next);
		}

		//���̃t���[���n��ɂ������ǂ�����ۑ�����
		m_isLastGround = true;

	}
	//�󒆂�
	else
	{

		//�㏸�{�^���������ꂽ��
		if (m_isPlayer && input.IsPress("RB"))
		{
			velo.y = GetSpeed();
			m_isFloat = true;
		}
		//���~�{�^���������ꂽ��
		else if (m_isPlayer && input.IsPushTrigger(true))
		{
			velo.y = -GetSpeed();

			//�v���C���̃A�j���[�V�������W�����v���łȂ����
			if (!(m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kJumping))
			{
				//�A�j���[�V������ύX����
				m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kJumping, true);
			}

		}

		//�O�̃t���[������󒆂ɂ����
		if (!m_isLastGround)
		{
			//�G�̕���������(Y���W�̓v���C���[�Ɠ������W�ɂ���)
			auto frontPos = GetTargetPos();

			frontPos.y = m_pCharacter->GetPos().y;

			m_pCharacter->SetFrontPos(frontPos);

			//�㏸���Ă��邩�㉺�ړ������Ă��Ȃ��ꍇ
			if (velo.y >= 0)
			{
				//�v���C���̃A�j���[�V�������󒆑ҋ@�łȂ����
				if (!(m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kSkyIdle))
				{
					//�A�j���[�V������ύX����
					m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kSkyIdle, true);
				}
			}
		}

		//���̃t���[���󒆂ɂ������Ƃ�ۑ�����
		m_isLastGround = false;

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
					if (m_isPlayer && input.GetStickInfo().leftStickY < -GameSceneConstant::kPhysicalAttackStickPower)
					{
						attackName = "UpCharge";
					}
					//�X�e�B�b�N�����ɌX���Ă�����
					else if (m_isPlayer && input.GetStickInfo().leftStickY > GameSceneConstant::kPhysicalAttackStickPower)
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

	//��莞�ԃ��t�g�V�����_�[�{�^���������ꂽ��
	if (m_isPlayer && input.GetPushTriggerTime(false) > GameSceneConstant::kChargeStateChangeTime)
	{
		//����State�̃|�C���^�쐬
		auto next = std::make_shared<CharacterStateCharge>(m_pCharacter);
		//State��Charge�ɕύX����
		ChangeState(next);
		return;
	}

	//�ړ����Ă��Ȃ�������
	if (velo.SqLength() < 0.01f)
	{
		//�A�C�h����Ԃɖ߂�
		std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		ChangeState(next);
	}
	//�ړ��x�N�g����ݒ肷��
	SetCharacterVelo(velo);

#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Move", GetColor(255, 255, 255));

#endif // _DEBUG

}
void CharacterStateMove::Exit()
{
}

void CharacterStateMove::OnCollide(std::shared_ptr<Collidable> collider)
{
}