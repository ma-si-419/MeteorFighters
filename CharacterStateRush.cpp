#include "CharacterStateRush.h"
#include "CharacterStateIdle.h"
#include "CharacterStateDash.h"
#include "CharacterStateNormalAttack.h"
#include "DxLib.h"
#include "Input.h"
#include "CharacterBase.h"
#include "GameSceneConstant.h"
#include <cmath>

namespace
{
	//�X�e�B�b�N���삪����Ă��Ȃ��Ƃ��㉺�ړ�
	constexpr float kMoveVerticalPower = 0.03f;

	//�㉺�ړ��̊p�x�̍ő�
	constexpr float kVerticalMoveMax = 0.5f;
	//�㉺�ړ��̊p�x�̍ŏ�
	constexpr float kVerticalMoveMin = -0.5f;

	//1�t���[���łǂ̂��炢�s����������]�ł��邩
	constexpr float kRotScaleMax = 10.0f;

	//�h���S���_�b�V�����ɃJ�[�u���鋗��
	constexpr float kCurveDistance = 300.0f;

	//�A�C�h���ɖ߂�Ƃ��̃A�j���[�V�����u�����h�̑���
	constexpr float kEndAnimBlendSpeed = 0.08f;

	//�A�C�h���ɖ߂�Ƃ��̃A�j���[�V�����̍Đ����x
	constexpr float kEndAnimPlaySpeed = 0.5f;

	//�A�C�h���ɖ߂�Ƃ��̍d������
	constexpr int kEndLandingTime = 20;

	//�A�C�h���ɖ߂�Ƃ��ɃJ������h�炷����
	constexpr int kCameraShakeTime = 3;

	//�ŏ��̃X�^�[�g���[�V�����̎���
	constexpr float kStartAnimTime = 8.0f;
}

CharacterStateRush::CharacterStateRush(std::shared_ptr<CharacterBase> character) :
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
	float vX = GetTargetPos().x - m_pCharacter->GetPos().x;
	float vZ = GetTargetPos().z - m_pCharacter->GetPos().z;

	float yAngle = std::atan2f(vX, vZ);

	MyEngine::Vector3 rotation;

	rotation = MyEngine::Vector3(0.0f, yAngle, 0.0f);

	MATRIX mat = rotation.GetRotationMat();

	m_moveDir = dir.MatTransform(mat);

	//�O���͓͂G�Ɍ������Ă����������͂�y���W���v�Z�ɓ���Ȃ��̂ł��̌v�Z
	if (dir.z > 0)
	{
		MyEngine::Vector3 toTargetDir = (GetTargetPos() - m_pCharacter->GetPos()).Normalize();

		m_moveDir = m_moveDir * (1.0 - dir.z) + toTargetDir * dir.z;
	}

	m_moveTarget.SetCenterPos(m_pCharacter->GetPos());
	m_moveTarget.SetLocalPos(dir.MatTransform(mat));
	m_moveTarget.SetFrontPos(GetTargetPos());
}

void CharacterStateRush::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kRush;
	m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kRushStart, true);
}

void CharacterStateRush::Update()
{
	m_time++;

	auto& input = MyEngine::Input::GetInstance();

	//�X�e�B�b�N�̌X��
	MyEngine::Vector3 stickDir;

	if (m_isPlayer)
	{
		stickDir = MyEngine::Vector3(input.GetStickInfo().leftStickX, 0, -input.GetStickInfo().leftStickY);
	}

	////////

	//���b�V���I�����̏���
	if (m_isEndRush)
	{
		m_rushEndTime++;

		if (m_pCharacter->GetPlayAnimKind() != CharacterBase::AnimKind::kRushEnd)
		{
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kRushEnd, false, kEndAnimBlendSpeed);

			m_pCharacter->SetAnimPlaySpeed(kEndAnimPlaySpeed);
		}
		//�U�����͂�����Ă�����U����ԂɈڍs����
		if (m_isAttackInput)
		{
			auto next = std::make_shared<CharacterStateNormalAttack>(m_pCharacter);

			next->SetAttack("X", "DashAttack");

			//�G�̕���������
			m_pCharacter->SetFrontPos(GetTargetPos());

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
	if (m_isPlayer && input.IsTrigger("A"))
	{
		//���t�g�V�����_�[��������Ă�����
		if (m_isPlayer && input.IsPushTrigger(false))
		{
			//�G�̋߂��܂Ō������ˌ���ԂɂȂ�
			m_isRushEnemy = true;
			m_rushTargetPos = GetTargetBackPos(GameSceneConstant::kEnemyBackPosDistance);
		}
		//������Ă��Ȃ����
		else
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

				ShakeCamera(kCameraShakeTime);
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
	if (m_isPlayer && input.IsPress("RB"))
	{
		dir.y += kMoveVerticalPower;
		m_moveDir.y += kMoveVerticalPower * speed;
	}
	if (m_isPlayer && input.IsPushTrigger(true))
	{
		dir.y -= kMoveVerticalPower;
		m_moveDir.y -= kMoveVerticalPower * speed;
	}

	//�ړ��x�N�g������ɂ����ɂ��X�������Ȃ��悤�ɃN�����v
	m_moveDir.y = std::fmax(m_moveDir.y, kVerticalMoveMin);
	m_moveDir.y = std::fmin(m_moveDir.y, kVerticalMoveMax);

	//�X�e�B�b�N���X���������㏸�A���~�{�^�����������Ƃ��̂ݍs������������ύX����
	if (stickDir.SqLength() > 0.001f)
	{
		stickDir = stickDir.Normalize();

		//�G�l�~�[�̕����Ɉړ���������]������
		float vX = GetTargetPos().x - m_pCharacter->GetPos().x;
		float vZ = GetTargetPos().z - m_pCharacter->GetPos().z;

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
			MyEngine::Vector3 toTargetDir = (GetTargetPos() - m_pCharacter->GetPos()).Normalize();

			dir = dir * (1.0 - stickDir.z) + toTargetDir * stickDir.z;
		}

		m_moveDir = dir.Normalize();
	}


	//�J�n�A�j���[�V�������Đ����Ĉ�莞�Ԃ�����
	if (m_time > kStartAnimTime)
	{
		//�܂��J�n�A�j���[�V�������Đ����Ă�����
		if (m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kRushStart)
		{
			ShakeCamera(kCameraShakeTime);
			//�A�j���[�V������ύX����
			m_pCharacter->ChangeAnim(CharacterBase::AnimKind::kSkyDash, true);
		}
	}
	//�܂��J�n�A�j���[�V�������Đ����Ă�����
	else
	{
		//�ړ����悤�Ƃ��Ă�������ɑ̂�������
		m_pCharacter->SetFrontPos(m_moveDir + m_pCharacter->GetPos());

		SetCharacterVelo(MyEngine::Vector3(0,0,0));

		//�ړ��������s��Ȃ�
		return;
	}


	////�s������������ݒ肷��
	//m_moveTarget.SetCenterPos(m_pCharacter->GetPos());
	//m_moveTarget.SetLocalPos(dir);
	//m_moveTarget.SetFrontPos(GetTargetPos());

	////�s���������W�̉�]�x�𒲂ׂ�
	//float toMoveTargetRotX = m_moveTarget.GetWorldPos().x - m_pCharacter->GetPos().x;
	//float toMoveTargetRotY = m_moveTarget.GetWorldPos().y - m_pCharacter->GetPos().y;
	//float toMoveTargetRotZ = m_moveTarget.GetWorldPos().z - m_pCharacter->GetPos().z;

	////y��]
	//float targetYAngle = std::atan2f(toMoveTargetRotX, toMoveTargetRotX);

	////���������Ă�������̉�]�x�𒲂ׂ�
	//float moveVecRotX = m_moveDir.x;
	//float moveVecRotY = m_moveDir.y;
	//float moveVecRotZ = m_moveDir.z;

	//float moveYAngle = std::atan2f(moveVecRotX, moveVecRotZ);

	////��]�̍������߂�
	//float difY = targetYAngle - moveYAngle;

	////��]����傫����ݒ�
	//float rotY = std::fmin(difY, kRotScaleMax);

	//MyEngine::Vector3 rotation(0.0f, rotY, 0.0f);

	////��]
	//MATRIX mat = rotation.GetRotationMat();

	////�ړ��x�N�g������]������
	//m_moveDir = m_moveDir.MatTransform(mat);

	//printfDx("moveY : %0.3f,tarY : % 0.3f\n", moveVecRotX, toMoveTargetRotX);
	//printfDx("X:%.2f,Y:%.2f,Z:%.2f\n", dir.x, dir.y, dir.z);

	MyEngine::Vector3 velo = m_moveDir * speed;


	//�G�̔w��Ɍ������t���O�������Ă����
	if (m_isRushEnemy)
	{
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
		if ((GetTargetBackPos(GameSceneConstant::kEnemyBackPosDistance) - m_pCharacter->GetPos()).Length() > GameSceneConstant::kCameraMoveDistance)
		{
			m_rushTargetPos = GetTargetBackPos(GameSceneConstant::kEnemyBackPosDistance);
			StopMoveCamera();
		}
		//��苗���܂ŋ߂Â�����
		else
		{
			StartMoveCamera();

			//�J�����������ړ�������
			m_pCharacter->StartFastCameraMove();

			//����ɋ߂��܂ŋ߂Â�����
			if (toTarget.Length() < GameSceneConstant::kEndRushDistance)
			{
				//��Q���ɓ�����悤�ɂ���
				m_pCharacter->SetIsTrigger(false);

				//�G�̕���������
				MyEngine::Vector3 nextPos = m_pCharacter->GetPos() + velo;

				MyEngine::Vector3 nextToTarget = GetTargetPos() - nextPos;

				m_pCharacter->SetFrontPos(nextToTarget + m_pCharacter->GetPos());

				ShakeCamera(kCameraShakeTime);

				m_isEndRush = true;
			}
		}
	}
	//�ʏ�̃��b�V������
	else
	{
		//���̈ړ����W���G�l�~�[�ƂԂ���̂Ȃ�΃��b�V������߂�
		MyEngine::Vector3 nextPos = m_pCharacter->GetPos() + velo;

		float lange = (GetTargetPos() - nextPos).Length();

		//�L�����N�^�[�̔��a��̕������������߂����
		if (lange < GameSceneConstant::kCharacterRadius * 2)
		{
			m_isEndRush = true;
		}

		//�U�����͂����ꂽ�炷���ɍU���Ɉڂ�
		if (m_isPlayer && input.IsTrigger("X"))
		{
			auto next = std::make_shared<CharacterStateNormalAttack>(m_pCharacter);

			next->SetAttack("X", "MiddleCharge");
			next->SetAttackVelo(m_moveDir * 3.0f);

			//�ړ�����������
			m_pCharacter->SetFrontPos(m_moveDir + m_pCharacter->GetPos());

			ChangeState(next);
		}
	}

	SetCharacterVelo(velo);
	//���b�V�����I��鏈�������Ă��Ȃ���Έړ�����������
	if (!m_isEndRush)
	{
		m_pCharacter->SetFrontPos(m_pCharacter->GetPos() + velo);
	}

#ifdef _DEBUG

	DrawSphere3D(m_rushTargetPos.CastVECTOR(), 3, 5, GetColor(255, 0, 255), GetColor(255, 0, 255), true);

	DrawString(0, 16, "PlayerState:Rush", GetColor(255, 255, 255));

#endif // _DEBUG

}
void CharacterStateRush::Exit()
{
}

void CharacterStateRush::OnCollide(std::shared_ptr<Collidable> collider)
{
}