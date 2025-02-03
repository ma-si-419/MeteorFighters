#include "CharacterStateDash.h"
#include "CharacterStateIdle.h"
#include "Character.h"
#include "GameSceneConstant.h"
#include "TutorialManager.h"
#include "SoundManager.h"
#include <cmath>

namespace
{
	const MyEngine::Vector3 kDefaultMoveDir(0, 0, 1);

	constexpr float kDashSpeed = 4.0f;

	constexpr float kDodgeSpeed = 2.5f;

	constexpr int kDashTime = 15;

	constexpr int kJustGuardTime = 20;

	constexpr float kAnimationBlendSpeed = 0.12f;

	constexpr float kAnimPlaySpeed = 1.5f;
}

CharacterStateDash::CharacterStateDash(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
	m_moveDir(kDefaultMoveDir),
	m_isDodge(false)
{
}

void CharacterStateDash::SetMoveDir(MyEngine::Vector3 moveDir)
{
	m_moveDir = moveDir;
}

void CharacterStateDash::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = GetKind();
	if (m_pCharacter->IsGround())
	{
		m_pCharacter->ChangeAnim(Character::AnimKind::kGroundDash, true, kAnimationBlendSpeed);
	}
	else
	{
		m_pCharacter->ChangeAnim(Character::AnimKind::kSkyDash, true, kAnimationBlendSpeed);
	}

	//�G�̋������߂����
	if ((m_pManager->GetTargetPos(m_pCharacter) - m_pCharacter->GetPos()).Length() < GameSceneConstant::kNearLange)
	{
		//����s��������
		m_isDodge = true;

		//�����Ԃɂ��Ă���
		m_guardKind = CharacterGuardKind::kDodge;
	}

	//���
	if (m_isDodge)
	{
		//�����ɂ���ăA�j���[�V������ς���
		//����̏ꍇ�͈ړ�������O�㍶�E�̎l�ʂ�Ɍ��肷��(�΂߂��Ȃ���)
		if (m_moveDir.z >= 0.5f)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kDodgeFront, false, kAnimationBlendSpeed);
			m_moveDir = MyEngine::Vector3(0.0f, 0.0f, 1.0f);
		}
		else if (m_moveDir.z < -0.5f)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kDodgeBack, false, kAnimationBlendSpeed);
			m_moveDir = MyEngine::Vector3(0.0f, 0.0f, -1.0f);
		}
		else if (m_moveDir.x >= 0.5f)
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kDodgeRight, false, kAnimationBlendSpeed);
			m_moveDir = MyEngine::Vector3(1.0f, 0.0f, 0.0f);
		}
		else
		{
			m_pCharacter->ChangeAnim(Character::AnimKind::kDodgeLeft, false, kAnimationBlendSpeed);
			m_moveDir = MyEngine::Vector3(-1.0f, 0.0f, 0.0f);
		}

		//����̉����Đ�
		SoundManager::GetInstance().PlayOnceSound("Step");
	}
	//�_�b�V���̏ꍇ
	else
	{
		//�_�b�V���̉����Đ�
		SoundManager::GetInstance().PlayOnceSound("Dash");
	}

	m_pCharacter->SetAnimPlaySpeed(kAnimPlaySpeed);

}

void CharacterStateDash::Update()
{
	m_time++;

	//�G�l�~�[�̕����Ɉړ���������]������
	float vX = m_pManager->GetTargetPos(m_pCharacter).x - m_pCharacter->GetPos().x;
	float vZ = m_pManager->GetTargetPos(m_pCharacter).z - m_pCharacter->GetPos().z;

	float yAngle = std::atan2f(vX, vZ);

	MyEngine::Vector3 rotation(0.0f, yAngle, 0.0f);

	MATRIX mat = rotation.GetRotationMat();

	MyEngine::Vector3 dir = m_moveDir.MatTransform(mat);

	//�󒆂ɂ��đO���͂���Ă�����
	if (m_moveDir.z > 0 && !m_pCharacter->IsGround())
	{
		float frontRate = m_moveDir.z;

		MyEngine::LocalPos enemyLocal;

		enemyLocal.SetCenterPos(m_pManager->GetTargetPos(m_pCharacter));

		enemyLocal.SetLocalPos(MyEngine::Vector3(0.0f, 0.0f, GameSceneConstant::kCharacterRadius));

		enemyLocal.SetFrontPos(m_pCharacter->GetPos());

		MyEngine::Vector3 targetPos = m_pManager->GetTargetPos(m_pCharacter) + enemyLocal.GetWorldPos();

		MyEngine::Vector3 toTarget = (m_pManager->GetTargetPos(m_pCharacter) - m_pCharacter->GetPos()).Normalize();

		dir = (dir * (1.0f - frontRate)) + toTarget * frontRate;
	}

	MyEngine::Vector3 velo;

	if (m_isDodge)
	{
		velo = dir * kDodgeSpeed;
	}
	else
	{
		velo = dir * kDashSpeed;
	}

	//�ړ������ɃX�s�[�h��������
	SetCharacterVelo(velo);

	if (m_isDodge)
	{
		//����Ȃ�G�̕�������
		m_pCharacter->LookTarget();
	}
	else
	{
		//�_�b�V���Ȃ�ړ�����������
		m_pCharacter->SetFrontPos(m_pCharacter->GetPos() + velo);
	}

	//����̏ꍇ
	if (m_isDodge)
	{
		//�c�����쐬����^�C�~���O�ɂȂ�����
		if (static_cast<int>(m_time) % GameSceneConstant::kAfterImageCreateTime == 0)
		{
			m_pCharacter->CreateAfterImage();
		}

		//��莞�Ԉړ�������A�C�h��State�ɖ߂�
		if (m_time > kJustGuardTime)
		{
			auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

			ChangeState(next);
			return;
		}
	}
	//�_�b�V���̏ꍇ
	else
	{
		//��莞�Ԉړ�������A�C�h��State�ɖ߂�
		if (m_time > kDashTime)
		{
			auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

			ChangeState(next);
			return;
		}
	}
}

void CharacterStateDash::Exit()
{
	//����̏ꍇ
	if (m_isDodge)
	{
		//�G�̕���������
		m_pCharacter->LookTarget();
		m_pCharacter->SetFrontPos(m_pManager->GetTargetPos(m_pCharacter));

		//�X�e�b�v�`���[�g���A���N���A�����true�ɂ���
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kStep));
	}
	//�_�b�V���̏ꍇ
	else
	{
		//�_�b�V���`���[�g���A���N���A�����true�ɂ���
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kDash));
	}


	m_pCharacter->SetAnimPlaySpeed();
	m_guardKind = CharacterGuardKind::kNone;
}
