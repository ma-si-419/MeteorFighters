#include "CharacterStateRush.h"
#include "CharacterStateIdle.h"
#include "CharacterStateDash.h"
#include "CharacterStateNormalAttack.h"
#include "CharacterStateButtonBashing.h"
#include "DxLib.h"
#include "Input.h"
#include "Character.h"
#include "GameSceneConstant.h"
#include "Effect.h"
#include "GameManagerBase.h"
#include "TutorialManager.h"
#include <cmath>
#include "SoundManager.h"

namespace
{
	//�X�e�B�b�N���삪����Ă��Ȃ��Ƃ��㉺�ړ�
	constexpr float kMoveVerticalPower = 0.05f;

	//�㉺�ړ��̊p�x�̍ő�
	constexpr float kVerticalMoveMax = 0.5f;
	//�㉺�ړ��̊p�x�̍ŏ�
	constexpr float kVerticalMoveMin = -0.5f;

	//1�t���[���łǂ̂��炢�s����������]�ł��邩
	constexpr float kRotScaleMax = 10.0f;

	//���P�b�g�_�b�V�����ɃJ�[�u���鋗��
	constexpr float kCurveDistance = 300.0f;

	//�G�l�~�[�Ɍ������Ă����_�b�V���Ŏg�p����C�͗�
	constexpr float kEnemyRushCost = 10000.0f;

	//�ړ����ɂǂ̂��炢�C�͂��g�p���邩
	constexpr float kRushCost = 50.0f;

	//�A�C�h���ɖ߂�Ƃ��̃A�j���[�V�����u�����h�̑���
	constexpr float kEndAnimBlendSpeed = 0.08f;

	//�A�C�h���ɖ߂�Ƃ��̃A�j���[�V�����̍Đ����x
	constexpr float kEndAnimPlaySpeed = 0.6f;

	//�A�C�h���ɖ߂�Ƃ��̍d������
	constexpr int kEndLandingTime = 20;

	//�A�C�h���ɖ߂�Ƃ��ɃJ������h�炷����
	constexpr int kCameraShakeTime = 3;

	//�ŏ��̃X�^�[�g���[�V�����̎���
	constexpr float kStartAnimTime = 8.0f;

	//���`��Ԃ̑���
	constexpr float kEasingSpeed = 0.08f;

	//�ˌ����s���Œ᎞��
	constexpr int kRushTime = 20;

	//�ړ����ɐ^���΂���͂��ꂽ�Ɣ��f����x�N�g���̍��̑傫��
	constexpr float kReverseVecScale = 1.999f; 

	//�������Đ�����Ԋu
	constexpr int kSoundInterval = 60;
}

CharacterStateRush::CharacterStateRush(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
	m_isRushEnemy(false),
	m_isEndRush(false),
	m_rushEndTime(0),
	m_isAttackInput(false)
{
}

void CharacterStateRush::SetMoveDir(MyEngine::Vector3 dir)
{
	//�G�l�~�[�̕����Ɉړ���������]������
	float vX = m_pManager->GetTargetPos(m_pCharacter).x - m_pCharacter->GetPos().x;
	float vZ = m_pManager->GetTargetPos(m_pCharacter).z - m_pCharacter->GetPos().z;

	float yAngle = std::atan2f(vX, vZ);

	MyEngine::Vector3 rotation;

	rotation = MyEngine::Vector3(0.0f, yAngle, 0.0f);

	MATRIX mat = rotation.GetRotationMat();

	m_moveDir = dir.MatTransform(mat);

	//�O���͓͂G�Ɍ������Ă����������͂�y���W���v�Z�ɓ���Ȃ��̂ł��̌v�Z
	if (dir.z > 0)
	{
		MyEngine::Vector3 toTargetDir = (m_pManager->GetTargetPos(m_pCharacter) - m_pCharacter->GetPos()).Normalize();

		m_moveDir = m_moveDir * (1.0f - dir.z) + toTargetDir * dir.z;
	}

}

void CharacterStateRush::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kRush;
	m_pCharacter->ChangeAnim(Character::AnimKind::kRushStart, true);

	//�X�[�p�[�_�b�V���̃`���[�g���A�����N���A����
	SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kSuperDash));

	//�X�[�p�[�_�b�V���J�n�̃T�E���h���Đ�����
	SoundManager::GetInstance().PlayOnceSound("StartSuperDash");
}

void CharacterStateRush::Update()
{
	m_time++;

	//�T�E���h�����Ԋu�ōĐ�����
	if (static_cast<int>(m_time) % kSoundInterval == 0)
	{
		SoundManager::GetInstance().PlayOnceSound("OnSuperDash");
	}

	//�ʏ펞�ŋC�͂�����Ȃ����
	if (!m_pCharacter->SubMp(kRushCost) && !m_isRushEnemy)
	{
		//���b�V�����I������
		m_isEndRush = true;
	}

	auto input = GetCharacterInput();

	//�X�e�B�b�N�̌X��
	MyEngine::Vector3 stickDir;

	stickDir = MyEngine::Vector3(input->GetStickInfo().leftStickX, 0, -input->GetStickInfo().leftStickY);

	////////

	//���b�V���I�����̏���
	if (m_isEndRush)
	{
		m_rushEndTime++;

		//���b�V���I���A�j���[�V�����ɂȂ��Ă��Ȃ����
		if (m_pCharacter->GetPlayAnimKind() != Character::AnimKind::kRushEnd)
		{
			//���b�V���I���A�j���[�V�����ɂȂ�
			m_pCharacter->ChangeAnim(Character::AnimKind::kRushEnd, false, kEndAnimBlendSpeed);

			//�A�j���[�V�����̍Đ����x��ύX����
			m_pCharacter->SetAnimPlaySpeed(kEndAnimPlaySpeed);

			//�G�t�F�N�g��ύX����
			m_pManager->ExitEffect(m_pEffect);

			m_pEffect = std::make_shared<Effect>(Effect::EffectKind::kDashEnd);

			m_pEffect->SetPos(m_pCharacter->GetFrontPos());

			//��]��ݒ肷��
			MyEngine::Vector3 rotation;

			//�G�Ɍ��������͂�����Ă�����
			if (m_isRushEnemy)
			{
				MyEngine::Vector3 targetPos = m_pManager->GetTargetPos(m_pCharacter);
				MyEngine::Vector3 pos = m_pCharacter->GetPos();

				float vX = targetPos.x - pos.x;
				float vZ = targetPos.z - pos.z;

				rotation.y = std::atan2f(vX, vZ);
			}
			//�G�Ɍ��������͂�����Ă��Ȃ������Ƃ�
			else
			{
				MyEngine::Vector3 nextPos = m_pCharacter->GetPos() + m_moveDir;
				MyEngine::Vector3 pos = m_pCharacter->GetPos();

				float vX = nextPos.x - pos.x;
				float vZ = nextPos.z - pos.z;

				rotation.y = std::atan2f(vX, vZ);
			}

			m_pEffect->SetRotation(rotation);

			m_pManager->EntryEffect(m_pEffect);

		}
		//�U�����͂�����Ă�����U����ԂɈڍs����
		if (m_isAttackInput)
		{
			auto next = std::make_shared<CharacterStateNormalAttack>(m_pCharacter);

			next->SetAttack("X", "DashAttack");

			//�G�̕���������
			m_pCharacter->SetFrontPos(m_pManager->GetTargetPos(m_pCharacter));

			ChangeState(next);
		}

		//���b�V�����I�����Ă����莞�Ԃ�������
		if (m_rushEndTime > kEndLandingTime)
		{
			//�A�C�h����Ԃɖ߂�
			auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

			ChangeState(next);

			return;
		}


		SetCharacterVelo(MyEngine::Vector3(0.0f, 0.0f, 0.0f));
		return;//���̏��������Ȃ����߂�
	}

	////////



	////////

	//���b�V�����s���Ƃ��̏���

	//A�{�^���������ꂽ�Ƃ���
	if (input->IsTrigger("A"))
	{
		//���t�g�V�����_�[��������Ă�����
		if (input->IsPushTrigger(false))
		{
			//�G�̋߂��܂Ō������ˌ���Ԃ���Ȃ����
			if (!m_isRushEnemy)
			{
				//�C�͂����肽�ꍇ�̂�
				if (m_pCharacter->SubMp(kEnemyRushCost))
				{
					//�T�E���h���Đ�����
					SoundManager::GetInstance().PlayOnceSound("StartRocketDash");

					//�G�̋߂��܂Ō������ˌ���ԂɂȂ�
					m_isRushEnemy = true;
					m_rushTargetPos = m_pManager->GetTargetBackPos(GameSceneConstant::kEnemyBackPosDistance, m_pCharacter);
				}
			}
		}
		//������Ă��Ȃ��Ƃ�
		else
		{
			//�G�̋߂��܂Ō������ˌ���ԂłȂ����
			if (!m_isRushEnemy)
			{
				//�X�e�B�b�N�̌X��������
				if (stickDir.SqLength() > 0.001f)
				{
					//�_�b�V����ԂɈڍs����
					auto next = std::make_shared<CharacterStateDash>(m_pCharacter);

					next->SetMoveDir(stickDir.Normalize());

					ChangeState(next);

					return;
				}
				//�X�e�B�b�N���X���Ă��Ȃ����
				else
				{
					m_isEndRush = true;

					m_pManager->ShakeCamera(kCameraShakeTime);
				}
			}
		}
	}

	////////


	////////

	//�ړ�����

	float speed = m_pCharacter->GetSpeed() * GameSceneConstant::kRushMoveSpeedRate;

	//�s����������
	MyEngine::Vector3 dir;

	//�㉺�ړ�����
	if (input->IsPress("RB"))
	{
		dir.y += kMoveVerticalPower * speed;
	}
	if (input->IsPushTrigger(true))
	{
		dir.y -= kMoveVerticalPower * speed;
	}

	//�X�e�B�b�N���X���������㏸�A���~�{�^�����������Ƃ��̂ݍs������������ύX����
	if (stickDir.SqLength() > 0.001f)
	{
		stickDir = stickDir.Normalize();
		//�G�l�~�[�̕����Ɉړ���������]������
		float vX = m_pManager->GetTargetPos(m_pCharacter).x - m_pCharacter->GetPos().x;
		float vZ = m_pManager->GetTargetPos(m_pCharacter).z - m_pCharacter->GetPos().z;

		float yAngle = std::atan2f(vX, vZ);

		MyEngine::Vector3 rotation;

		rotation = MyEngine::Vector3(0.0f, yAngle, 0.0f);

		//��]�s��(y�����̂�)
		MATRIX mat = rotation.GetRotationMat();

		//�ړ�������ݒ�
		dir += stickDir.MatTransform(mat);

		//�O���͓͂G�Ɍ������Ă����������͂�y���W���v�Z�ɓ���Ȃ��̂ł��̌v�Z
		if (stickDir.z > 0)
		{
			MyEngine::Vector3 toTargetDir = (m_pManager->GetTargetPos(m_pCharacter) - m_pCharacter->GetPos()).Normalize();

			dir = dir * (1.0f - stickDir.z) + toTargetDir * stickDir.z;
		}
	}
	if (dir.SqLength() > 0.1f)
	{
		m_moveTarget = dir.Normalize();
	}

	//�J�n�A�j���[�V�������Đ����Ĉ�莞�Ԃ�����
	if (m_time > kStartAnimTime)
	{
		//�܂��J�n�A�j���[�V�������Đ����Ă�����
		if (m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kRushStart)
		{
			m_pManager->ShakeCamera(kCameraShakeTime);
			//�A�j���[�V������ύX����
			m_pCharacter->ChangeAnim(Character::AnimKind::kSkyDash, true);
			//�G�t�F�N�g���Đ�����
			m_pEffect = std::make_shared<Effect>(Effect::EffectKind::kDash);
			m_pEffect->SetPos(m_pCharacter->GetPos());
			m_pEffect->SetLoop(30, 32);
			m_pManager->EntryEffect(m_pEffect);
		}
	}
	//�܂��J�n�A�j���[�V�������Đ����Ă�����
	else
	{
		//�ړ����悤�Ƃ��Ă�������ɑ̂�������
		m_pCharacter->SetFrontPos(m_moveDir + m_pCharacter->GetPos());

		SetCharacterVelo(MyEngine::Vector3(0, 0, 0));

		//�ړ��������s��Ȃ�
		return;
	}

	//�������X�e�B�b�N���X���Ă�������Ɉړ�������ύX����
	if (m_moveDir.SqLength() > 0.1f && m_moveTarget.SqLength() > 0.1f)
	{
		m_moveDir = m_moveDir * (1.0f - kEasingSpeed) + m_moveTarget * kEasingSpeed;

		MyEngine::Vector3 checkVecA = m_moveDir;
		checkVecA.y = 0;
		checkVecA = checkVecA.Normalize();

		MyEngine::Vector3 checkVecB = m_moveTarget;
		checkVecB.y = 0;
		checkVecB = checkVecB.Normalize();

		if (std::fabs((checkVecA - checkVecB).Length()) > kReverseVecScale && m_time > kRushTime)
		{
			//�G�Ɏ����Ō������Ă������������Ă��Ȃ����
			if (!m_isRushEnemy)
			{
				m_isEndRush = true;
			}
		}
		m_moveDir = m_moveDir.Normalize();
	}
	else if (m_moveTarget.SqLength() > 0.1f)
	{
		m_moveDir = m_moveTarget.Normalize();
	}



	MyEngine::Vector3 velo = m_moveDir * speed;


	//�G�̔w��Ɍ������t���O�������Ă����
	if (m_isRushEnemy)
	{
		//���P�b�g�_�b�V���̃`���[�g���A�����N���A����
		SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kRocketDash));

		//�����G��RushState�ł����
		if (static_cast<CharacterStateKind>(m_pManager->GetTargetState(m_pCharacter)) == CharacterStateKind::kRush)
		{
			//�{�^���A�őΌ��Ɉڍs����
			auto next = std::make_shared<CharacterStateButtonBashing>(m_pCharacter);

			ChangeState(next);

			return;
		}

		//��Q���ɓ�����Ȃ��悤�ɂ���
		m_pCharacter->SetIsTrigger(true);

		MyEngine::Vector3 toTarget = m_rushTargetPos - m_pCharacter->GetPos();

		m_moveDir = toTarget.Normalize();

		//��苗������ړ�������ς��邽�߁A�����̊������Ƃ�
		float langeRate = toTarget.Length() / kCurveDistance;
		//�J�[�u�̊���
		float curveRate = 0.0f;

		//��苗���܂ŋ߂Â�����
		if (langeRate < 1.0f)
		{
			//0����1�̊Ԃ�-1����+1�ɂ��邽�߂�*2-1������
			curveRate = std::sinf(langeRate * 2.0f - 1.0f);

			MyEngine::Vector3 rotation(0.0f, curveRate, 0.0f);

			MATRIX mat = rotation.GetRotationMat();

			m_moveDir = m_moveDir.MatTransform(mat);
		}

		if (toTarget.Length() > GameSceneConstant::kRushSpeed)
		{
			velo = m_moveDir * GameSceneConstant::kRushSpeed;
		}
		else
		{
			velo = toTarget;
		}

		//��苗���܂ł͖ړI���W���X�V��������
		if ((m_pManager->GetTargetBackPos(GameSceneConstant::kEnemyBackPosDistance, m_pCharacter) - m_pCharacter->GetPos()).Length() > GameSceneConstant::kCameraMoveDistance)
		{
			m_rushTargetPos = m_pManager->GetTargetBackPos(GameSceneConstant::kEnemyBackPosDistance, m_pCharacter);

			//1P�̂݃J�����𓮂���
			if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
			{
				m_pManager->StopCameraCorrection();
			}
		}
		//��苗���܂ŋ߂Â�����
		else
		{
			m_pManager->StartCameraCorrection();

			//1P�̂݃J�����𓮂���
			if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
			{
				//�J�����������ړ�������
				m_pCharacter->StartFastCameraMove();
			}
			//����ɋ߂��܂ŋ߂Â�����
			if (toTarget.Length() < GameSceneConstant::kEndRushDistance)
			{
				//��Q���ɓ�����悤�ɂ���
				m_pCharacter->SetIsTrigger(false);

				//�G�̕���������
				MyEngine::Vector3 nextPos = m_pCharacter->GetPos() + velo;

				MyEngine::Vector3 nextToTarget = m_pManager->GetTargetPos(m_pCharacter) - nextPos;

				m_pCharacter->SetFrontPos(nextToTarget + m_pCharacter->GetPos());

				m_pManager->ShakeCamera(kCameraShakeTime);

				m_isEndRush = true;
			}
		}
	}
	//�ʏ�̃��b�V������
	else
	{
		//���̈ړ����W���G�l�~�[�ƂԂ���̂Ȃ�΃��b�V������߂�
		MyEngine::Vector3 nextPos = m_pCharacter->GetPos() + velo;

		float lange = (m_pManager->GetTargetPos(m_pCharacter) - nextPos).Length();

		//�L�����N�^�[�̔��a��̕������������߂����
		if (lange < GameSceneConstant::kCharacterRadius * 2)
		{
			m_isEndRush = true;
		}

		//�U�����͂����ꂽ�炷���ɍU���Ɉڂ�
		if (input->IsTrigger("X"))
		{
			auto next = std::make_shared<CharacterStateNormalAttack>(m_pCharacter);

			next->SetAttack("X", "MiddleCharge");
			next->SetAttackVelo(m_moveDir * 3.0f);

			//�ړ�����������
			m_pCharacter->SetFrontPos(m_moveDir + m_pCharacter->GetPos());

			ChangeState(next);
		}
	}

	//�G�t�F�N�g�ɉ�]�ƍ��W��ݒ肷��
	MyEngine::Vector3 rotation;

	MyEngine::Vector3 nextPos = m_pCharacter->GetPos() + velo;
	MyEngine::Vector3 pos = m_pCharacter->GetPos();

	float vX = nextPos.x - pos.x;
	float vZ = nextPos.z - pos.z;

	rotation.y = std::atan2f(vX, vZ);

	m_pEffect->SetPos(pos);
	m_pEffect->SetRotation(rotation);

	SetCharacterVelo(velo);
	//���b�V�����I��鏈�������Ă��Ȃ����
	if (!m_isEndRush)
	{
		//1P�̂݃J�����𓮂���
		if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
		{
			//�J�����������h�炷
			m_pManager->SwayCamera();
		}
		//�ړ�����������
		m_pCharacter->SetFrontPos(m_pCharacter->GetPos() + velo);
	}
}
void CharacterStateRush::Exit()
{
	m_pManager->ExitEffect(m_pEffect);

	//�T�E���h���~�߂�
	SoundManager::GetInstance().StopSound("OnSuperDash");
}
