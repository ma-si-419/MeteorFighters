#include "CharacterStateMove.h"
#include "CharacterStateIdle.h"
#include "CharacterStateDash.h"
#include "CharacterStateRush.h"
#include "CharacterStateJump.h"
#include "CharacterStateCharge.h"
#include "CharacterStateNormalAttack.h"
#include "CharacterStateGuard.h"
#include "GameSceneConstant.h"
#include "TutorialManager.h"
#include "DxLib.h"
#include "Input.h"
#include "Character.h"
#include <cmath>

namespace
{
	//�ړ����x��x�����銄��
	constexpr float kSlowMoveSpeed = 0.6f;
	//����Ɉړ����x��x�����鋗��
	constexpr float kNearestLange = 10.0f;
}

CharacterStateMove::CharacterStateMove(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
	m_attackButtonPushTime(0),
	m_attackKey("empty"),
	m_isFloat(false),
	m_gravityPower(0.0f),
	m_isLastGround(false),
	m_isReleaseX(true),
	m_isReleaseY(true)
{
	m_pCharacter->ChangeAnim(Character::AnimKind::kSkyIdle, true);
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
	MyEngine::Vector3 playerToTarget = m_pManager->GetTargetPos(m_pCharacter) - m_pCharacter->GetPos();

	//�C���v�b�g���Ǘ����Ă���N���X�̎Q��
	auto input = GetCharacterInput();
	//�ړ��x�N�g��
	MyEngine::Vector3 velo;
	//�ړ������x�N�g��
	MyEngine::Vector3 dir;

	MyEngine::Vector3 inputDir;

	//�X�e�B�b�N�̏��擾
	auto stick = input->GetStickInfo();

	//���X�e�B�b�N�̌X���擾
	inputDir = MyEngine::Vector3(stick.leftStickX, 0, -stick.leftStickY);

	//�ړ��x�N�g����0����Ȃ����
	if (inputDir.SqLength() > 0.001f)
	{
		inputDir = inputDir.Normalize();

		//�ړ�����
		dir = inputDir;

		//�G�l�~�[�̕����Ɉړ���������]������
		float vX = m_pManager->GetTargetPos(m_pCharacter).x - m_pCharacter->GetPos().x;
		float vZ = m_pManager->GetTargetPos(m_pCharacter).z - m_pCharacter->GetPos().z;

		float yAngle = std::atan2f(vX, vZ);

		MyEngine::Vector3 rotation;

		rotation = MyEngine::Vector3(0.0f, yAngle, 0.0f);

		MATRIX mat = rotation.GetRotationMat();

		dir = dir.MatTransform(mat);

		MyEngine::Vector3 toTarget = (m_pManager->GetTargetPos(m_pCharacter) - m_pCharacter->GetPos());
		MyEngine::Vector3 toTargetDir = toTarget.Normalize();

		//�󒆂ɂ��đO���͂���Ă�����
		if (inputDir.z > 0 && m_isFloat)
		{
			float frontRate = inputDir.z;

			dir = dir * (1.0f - frontRate) + toTargetDir * frontRate;
		}

		//�ړ����x
		float speed = m_pCharacter->GetSpeed();

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
			if (!(m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kJumping))
			{
				//�A�j���[�V������ύX����
				m_pCharacter->ChangeAnim(Character::AnimKind::kJumping, true);
			}
		}
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

				next->SetMoveDir(inputDir);

				ChangeState(next);
				return;
			}
		}

		//�G�Ƃ̋�������_�b�V�����X�e�b�v�����f����
		//(�X�e�b�v���_�b�V�����̔����DashState�̒��ł��s��)
		//(�����ł�MP������邩���Ȃ����ADashState�ɂ͂��邩�ǂ����𔻒f����)
		if ((m_pManager->GetTargetPos(m_pCharacter) - m_pCharacter->GetPos()).Length() > GameSceneConstant::kNearLange)
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
		if (input->IsTrigger("RB"))
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
			if (!(m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kRun))
			{
				//�A�j���[�V������ύX����
				m_pCharacter->ChangeAnim(Character::AnimKind::kRun, true);
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
		if (input->IsPress("RB"))
		{
			velo.y = m_pCharacter->GetSpeed();
			m_isFloat = true;

			//�㏸�`���[�g���A�����N���A�ɂ���
			SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kUp));
		}
		//���~�{�^���������ꂽ��
		else if (input->IsPushTrigger(true))
		{
			velo.y = -m_pCharacter->GetSpeed();

			//�v���C���̃A�j���[�V�������W�����v���łȂ����
			if (!(m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kJumping))
			{
				//�A�j���[�V������ύX����
				m_pCharacter->ChangeAnim(Character::AnimKind::kJumping, true);
			}

			//���~�`���[�g���A�����N���A�ɂ���
			SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kDown));

		}

		//�O�̃t���[������󒆂ɂ����
		if (!m_isLastGround)
		{
			//�G�̕���������(Y���W�̓v���C���[�Ɠ������W�ɂ���)
			auto frontPos = m_pManager->GetTargetPos(m_pCharacter);

			frontPos.y = m_pCharacter->GetPos().y;

			m_pCharacter->SetFrontPos(frontPos);

			//�㏸���Ă��邩�㉺�ړ������Ă��Ȃ��ꍇ
			if (velo.y >= 0)
			{
				//�v���C���̃A�j���[�V�������󒆑ҋ@�łȂ����
				if (!(m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kSkyIdle))
				{
					//�A�j���[�V������ύX����
					m_pCharacter->ChangeAnim(Character::AnimKind::kSkyIdle, true);
				}
			}
		}

		//���̃t���[���󒆂ɂ������Ƃ�ۑ�����
		m_isLastGround = false;

	}

	//��x�{�^���������ꂽ���ǂ���
	if (!input->IsPress("X"))
	{
		m_isReleaseX = true;
	}
	if (!input->IsPress("Y"))
	{
		m_isReleaseY = true;
	}


	//�U���{�^����������Ă��Ȃ��Ƃ���
	if (m_attackKey == "empty")
	{
		//�i���{�^���������ꂽ��
		if (input->IsPress("X") && m_isReleaseX)
		{
			m_attackKey = "X";
		}
		else if (input->IsPress("Y") && m_isReleaseY)
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
				if (m_attackKey == "X" && m_isReleaseX)
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
				else if (m_attackKey == "Y" && m_isReleaseY)
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

			//�U�����Z�b�g����Ă�����
			if (attackName != "empty")
			{
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

	//�K�[�h���͂�����Ă�����
	if (input->IsPress("B"))
	{
		//����State�̃|�C���^�쐬
		auto next = std::make_shared<CharacterStateGuard>(m_pCharacter);
		//State��Guard�ɕύX����
		ChangeState(next);
		return;
	}

	//�ړ����Ă��Ȃ�������
	if (velo.SqLength() < 0.001f)
	{
		//�A�C�h����Ԃɖ߂�
		std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		ChangeState(next);
	}
	//�ړ����Ă���Ƃ��̏ꍇ
	else
	{
		//�G�Ƃ̋���
		auto distance = (m_pManager->GetTargetPos(m_pCharacter) - m_pCharacter->GetPos()).Length();

		//�G�Ƃ̋������߂��Ɣ��f������ړ��`���[�g���A���N���A�t���O�𗧂Ă�
		if (distance < GameSceneConstant::kNearLange)
		{
			SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kMove));
		}
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