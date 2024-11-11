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
	constexpr float kMoveVerticalPower = 0.03f;

	constexpr float kVerticalMoveMax = 0.5f;
	constexpr float kVerticalMoveMin = -0.5f;
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
		m_moveDir = stickDir = stickDir.Normalize();

	}
	//�G�l�~�[�̕����Ɉړ���������]������
	float vX = GetEnemyPos().x - m_pPlayer->GetPos().x;
	float vZ = GetEnemyPos().z - m_pPlayer->GetPos().z;

	float yAngle = std::atan2f(vX, vZ);

	MyEngine::Vector3 rotation;

	rotation = MyEngine::Vector3(0.0f, yAngle, 0.0f);

	MATRIX mat = rotation.GetRotationMat();

	if (input.IsPress("RB"))
	{
		m_moveDir.y += kMoveVerticalPower;
	}
	if (input.IsPushTrigger(true))
	{
		m_moveDir.y -= kMoveVerticalPower;
	}

	//��ɂ����ɂ��X�������Ȃ��悤��
	m_moveDir.y = std::fmax(m_moveDir.y, kVerticalMoveMin);
	m_moveDir.y = std::fmin(m_moveDir.y, kVerticalMoveMax);



	MyEngine::Vector3 dir = m_moveDir.MatTransform(mat);

	MyEngine::Vector3 velo = dir * speed;

	//�G�̔w��Ɍ������t���O�������Ă����
	if (m_isRushEnemy)
	{
		MyEngine::Vector3 toTarget = m_rushTargetPos - m_pPlayer->GetPos();

		dir = toTarget.Normalize();

		if (toTarget.Length() > GameSceneConstant::kRushSpeed)
		{
			velo = dir * GameSceneConstant::kRushSpeed;
		}
		else
		{
			velo = dir * toTarget.Length();
		}

		//��苗���܂ł͖ړI���W���X�V��������
		if ((GetEnemyBackPos(GameSceneConstant::kEnemyBackPosDistance) - m_pPlayer->GetPos()).Length() > GameSceneConstant::kCameraMoveDistance)
		{
			m_rushTargetPos = GetEnemyBackPos(GameSceneConstant::kEnemyBackPosDistance);
		}
		//��苗���܂ŋ߂Â�����
		else
		{
			//�J�����������ړ�������
			m_pPlayer->StartFastCameraMove();

			//����ɋ߂��܂ŋ߂Â�����
			if (toTarget.Length() < GameSceneConstant::kEnemyBackPosDistance)
			{
				auto next = std::make_shared<PlayerStateIdle>(m_pPlayer);

				ChangeState(next);
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
