#include "PlayerStateRush.h"
#include "PlayerStateDash.h"
#include "PlayerStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"
#include "GameSceneConstant.h"
#include <cmath>

namespace
{
	//�X�e�B�b�N���삪����Ă��Ȃ��Ƃ��㉺�ړ�
	constexpr float kMoveVerticalPower = 0.03f;
	//�X�e�B�b�N���삪����Ă���Ƃ��̏㉺�ړ�
	constexpr float kOnStickVerticalPower = 200.0f;

	constexpr float kVerticalMoveMax = 0.5f;
	constexpr float kVerticalMoveMin = -0.5f;

	constexpr float kCurveScale = -80;

	//�ړ��x�N�g�����J�[�u���鋗��
	constexpr float kCurveDistance = 300.0f;
}

PlayerStateRush::PlayerStateRush(std::shared_ptr<Player> player) :
	PlayerStateBase(player),
	m_isRushEnemy(false)
{
}

void PlayerStateRush::SetMoveDir(MyEngine::Vector3 dir)
{
	//�G�l�~�[�̕����Ɉړ���������]������
	float vX = GetEnemyPos().x - m_pPlayer->GetPos().x;
	float vZ = GetEnemyPos().z - m_pPlayer->GetPos().z;

	float yAngle = std::atan2f(vX, vZ);

	MyEngine::Vector3 rotation;

	rotation = MyEngine::Vector3(0.0f, yAngle, 0.0f);

	MATRIX mat = rotation.GetRotationMat();

	m_moveDir = dir.MatTransform(mat);
}

void PlayerStateRush::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kRush;
	m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kSkyDash, true);
}

void PlayerStateRush::Update()
{
	auto& input = MyEngine::Input::GetInstance();

	//�X�e�B�b�N�̌X��
	MyEngine::Vector3 stickDir(input.GetStickInfo().leftStickX, 0, -input.GetStickInfo().leftStickY);

	//A�{�^���������ꂽ�Ƃ���
	if (input.IsTrigger("A"))
	{
		//���t�g�V�����_�[��������Ă�����
		if (input.IsPushTrigger(false))
		{
			//�G�̋߂��܂Ō������ˌ���ԂɂȂ�
			m_isRushEnemy = true;
			m_rushTargetPos = GetEnemyBackPos(GameSceneConstant::kEnemyBackPosDistance);
		}
		//������Ă��Ȃ����
		else
		{
			//�X�e�B�b�N�̌X��������
			if (stickDir.SqLength() > 0.001f)
			{
				//�_�b�V����ԂɈڍs����
				auto next = std::make_shared<PlayerStateDash>(m_pPlayer);

				next->SetMoveDir(stickDir.Normalize());

				ChangeState(next);

				return;
			}
			//�X�e�B�b�N���X���Ă��Ȃ����
			else
			{
				//�A�C�h����Ԃɖ߂�
				auto next = std::make_shared<PlayerStateIdle>(m_pPlayer);

				ChangeState(next);

				return;
			}
		}
	}


	float speed = m_pPlayer->GetSpeed() * GameSceneConstant::kRushMoveSpeedRate;

	if (stickDir.SqLength() > 0.001f)
	{
		//�X�e�B�b�N�̓��̓x�N�g�����ړ��x�N�g���̐^���΂ł���΃X�e�[�g���A�C�h���ɖ߂�
		if (-stickDir.x == m_moveDir.x && -stickDir.z == m_moveDir.z)
		{
			auto next = std::make_shared<PlayerStateIdle>(m_pPlayer);

			ChangeState(next);

			return;
		}

		//�ړ��̕����x�N�g��
		MyEngine::Vector3 moveDir = stickDir;

		if (input.IsPress("RB"))
		{
			moveDir.y += kOnStickVerticalPower;
		}
		if (input.IsPushTrigger(true))
		{
			moveDir.y -= kOnStickVerticalPower;
		}

		//�G�l�~�[�̕����Ɉړ���������]������
		float vX = GetEnemyPos().x - m_pPlayer->GetPos().x;
		float vZ = GetEnemyPos().z - m_pPlayer->GetPos().z;

		float yAngle = std::atan2f(vX, vZ);

		MyEngine::Vector3 rotation;

		rotation = MyEngine::Vector3(0.0f, yAngle, 0.0f);

		MATRIX mat = rotation.GetRotationMat();

		if (stickDir.z > 0)
		{
			MyEngine::Vector3 toTarget;////////////////////

			moveDir = moveDir * (1.0 - stickDir.Normalize().z) + ;
		}

		//MyEngine::Vector3 dir = m_moveDir.MatTransform(mat);
		m_moveDir = moveDir.Normalize().MatTransform(mat);
	}
	else
	{
		if (input.IsPress("RB"))
		{
			m_moveDir.y += kMoveVerticalPower;
		}
		if (input.IsPushTrigger(true))
		{
			m_moveDir.y -= kMoveVerticalPower;
		}
	}

	//��ɂ����ɂ��X�������Ȃ��悤��

	m_moveDir.y = std::fmax(m_moveDir.y, kVerticalMoveMin);
	m_moveDir.y = std::fmin(m_moveDir.y, kVerticalMoveMax);
	MyEngine::Vector3 velo = m_moveDir * speed;

	//�G�̔w��Ɍ������t���O�������Ă����
	if (m_isRushEnemy)
	{
		//��Q���ɓ�����Ȃ��悤�ɂ���
		m_pPlayer->SetIsTrigger(true);

		MyEngine::Vector3 toTarget = m_rushTargetPos - m_pPlayer->GetPos();

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

			printfDx("%0.2f\n", langeRate);

			float yAngle = curveRate * kCurveScale;

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
			velo = m_moveDir * toTarget.Length();
		}

		//��苗���܂ł͖ړI���W���X�V��������
		if ((GetEnemyBackPos(GameSceneConstant::kEnemyBackPosDistance) - m_pPlayer->GetPos()).Length() > GameSceneConstant::kCameraMoveDistance)
		{
			m_rushTargetPos = GetEnemyBackPos(GameSceneConstant::kEnemyBackPosDistance);
			StopMoveCamera();
		}
		//��苗���܂ŋ߂Â�����
		else
		{
			StartMoveCamera();

			//�J�����������ړ�������
			m_pPlayer->StartFastCameraMove();

			//����ɋ߂��܂ŋ߂Â�����
			if (toTarget.Length() < GameSceneConstant::kEnemyBackPosDistance)
			{
				auto next = std::make_shared<PlayerStateIdle>(m_pPlayer);

				ChangeState(next);

				//��Q���ɓ�����悤�ɂ���
				m_pPlayer->SetIsTrigger(false);

				//�G�̕���������
				MyEngine::Vector3 nextPos = m_pPlayer->GetPos() + velo;

				MyEngine::Vector3 nextToTarget = GetEnemyPos() - nextPos;

				m_pPlayer->SetFrontPos(nextToTarget + m_pPlayer->GetPos());

				return;
			}
		}
	}

	SetPlayerVelo(velo);

	m_pPlayer->SetFrontPos(m_pPlayer->GetPos() + velo);

#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Rush", GetColor(255, 255, 255));

#endif // _DEBUG

}
void PlayerStateRush::Exit()
{
}

void PlayerStateRush::OnCollide(std::shared_ptr<Collidable> collider)
{
}
