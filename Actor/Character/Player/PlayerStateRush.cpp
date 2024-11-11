#include "PlayerStateRush.h"
#include "PlayerStateDash.h"
#include "PlayerStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"
#include "GameSceneConstant.h"

PlayerStateRush::PlayerStateRush(std::shared_ptr<Player> player) :
	PlayerStateBase(player)
{
}

void PlayerStateRush::SetMoveDir(MyEngine::Vector3 dir)
{
}

void PlayerStateRush::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kRush;
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

		//�ړ��̕����x�N�g���̌v�Z
		m_moveDir = m_moveDir * (1.0f - GameSceneConstant::kRushMoveDirChangeRate) + stickDir.Normalize() * GameSceneConstant::kRushMoveDirChangeRate;
	}

	MyEngine::Vector3 velo = m_moveDir * speed;

	SetPlayerVelo(velo);

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
