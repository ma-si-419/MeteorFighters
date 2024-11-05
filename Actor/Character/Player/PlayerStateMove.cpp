#include "PlayerStateMove.h"
#include "PlayerStateIdle.h"
#include "PlayerStateJump.h"
#include "PlayerStateNormalAttack.h"
#include "GameSceneConstant.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"
#include <cmath>
PlayerStateMove::PlayerStateMove(std::shared_ptr<Player> player) :
	PlayerStateBase(player),
	m_attackButtonPushTime(0.0f),
	m_attackKey("empty"),
	m_isFloat(false),
	m_gravityPower(0.0f),
	m_isLastGround(false)
{
	m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kSkyIdle, true);
}

void PlayerStateMove::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kMove;
	if (m_pPlayer->IsGround())
	{
		m_isFloat = false;
	}
	else
	{
		m_isFloat = true;
	}
}

void PlayerStateMove::Update()
{
	//�v���C���[����G�l�~�[�ւ̃x�N�g��
	MyEngine::Vector3 playerToTarget = GetEnemyPos() - m_pPlayer->GetPos();

	//�C���v�b�g���Ǘ����Ă���N���X�̎Q��
	MyEngine::Input& input = MyEngine::Input::GetInstance();
	//�ړ��x�N�g��
	MyEngine::Vector3 velo;
	//�ړ������x�N�g��
	MyEngine::Vector3 dir;

	//�X�e�B�b�N�̏��擾
	MyEngine::Input::StickInfo stick = input.GetStickInfo();

	//���X�e�B�b�N�̌X���擾
	MyEngine::Vector3 leftStickDir(stick.leftStickX, 0, -stick.leftStickY);
	//�ړ��x�N�g����0����Ȃ����
	if (leftStickDir.SqLength() > 0.001)
	{
		//�ړ�����
		dir = leftStickDir.Normalize();

		//�G�l�~�[�̕����Ɉړ���������]������
		float vX = GetEnemyPos().x - m_pPlayer->GetPos().x;
		float vZ = GetEnemyPos().z - m_pPlayer->GetPos().z;

		float angle = std::atan2f(vX, vZ);

		MyEngine::Vector3 rotation(0.0f, angle, 0.0f);

		MATRIX mat = rotation.GetRotationMat();

		dir = dir.MatTransform(mat);

		//�ړ������ɃX�s�[�h��������
		velo = dir * GetSpeed();
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
	}

	//�n���
	if (m_pPlayer->IsGround())
	{

		//�W�����v�{�^���������ꂽ��
		if (input.IsTrigger("RB"))
		{
			//�W�����vState�Ɉڍs����
			auto next = std::make_shared<PlayerStateJump>(m_pPlayer);

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
		if (groundVelo.SqLength() > 0.001f)
		{
			//�v���C���̃A�j���[�V�������n��ړ��łȂ����
			if (!(m_pPlayer->GetPlayAnimKind() == CharacterBase::AnimKind::kRun))
			{
				//�A�j���[�V������ύX����
				m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kRun, true);
			}

			auto frontPos = m_pPlayer->GetPos() + groundVelo;

			frontPos.y = m_pPlayer->GetPos().y;

			m_pPlayer->SetFrontPos(frontPos);
		}
		//�ړ����Ă��Ȃ����
		else
		{
			//�v���C���̃A�j���[�V�������n��ҋ@�łȂ����
			if (!(m_pPlayer->GetPlayAnimKind() == CharacterBase::AnimKind::kIdle))
			{
				//�A�j���[�V������ύX����
				m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kIdle, true);
			}
		}

		//���̃t���[���n��ɂ������ǂ�����ۑ�����
		m_isLastGround = true;

	}
	//�󒆂�
	else
	{

		//�㏸�{�^���������ꂽ��
		if (input.IsPress("RB"))
		{
			velo.y = GetSpeed();
			m_isFloat = true;
		}
		//���~�{�^���������ꂽ��
		else if (input.IsPushTrigger(true))
		{
			velo.y = -GetSpeed();
		}

		//�O�̃t���[������󒆂ɂ����
		if (!m_isLastGround)
		{
			//�G�̕���������(Y���W�̓v���C���[�Ɠ������W�ɂ���)
			auto frontPos = GetEnemyPos();

			frontPos.y = m_pPlayer->GetPos().y;

			m_pPlayer->SetFrontPos(frontPos);
			//���~����
			if (velo.y < 0)
			{
				//�v���C���̃A�j���[�V�������W�����v���łȂ����
				if (!(m_pPlayer->GetPlayAnimKind() == CharacterBase::AnimKind::kJumping))
				{
					//�A�j���[�V������ύX����
					m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kJumping, true);
				}
			}
			//�㏸���Ă��邩�㉺�ړ������Ă��Ȃ��ꍇ
			else if (velo.y >= 0)
			{
				//�v���C���̃A�j���[�V�������󒆑ҋ@�łȂ����
				if (!(m_pPlayer->GetPlayAnimKind() == CharacterBase::AnimKind::kSkyIdle))
				{
					//�A�j���[�V������ύX����
					m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kSkyIdle, true);
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
		if (input.IsPress("X"))
		{
			m_attackKey = "X";
		}
		else if (input.IsPress("Y"))
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
		if (input.IsRelease(m_attackKey) ||
			m_attackButtonPushTime > GameSceneConstant::kChargeAttackTime)
		{
			//�`���[�W����Ă������ǂ�������
			bool isCharge = m_attackButtonPushTime > GameSceneConstant::kChargeAttackTime;
			//����State�̃|�C���^�쐬
			std::shared_ptr<PlayerStateNormalAttack> next = std::make_shared<PlayerStateNormalAttack>(m_pPlayer);
			//���̍U�����s������AttackState�ɓn��
			next->SetAttack(m_attackKey, isCharge);
			//State��Attack�ɕύX����
			ChangeState(next);
			return;
		}
	}

	//�ړ����Ă��Ȃ�������
	if (velo.SqLength() == 0)
	{
		//�A�C�h����Ԃɖ߂�
		std::shared_ptr<PlayerStateIdle> next = std::make_shared<PlayerStateIdle>(m_pPlayer);

		ChangeState(next);
	}
	//�ړ��x�N�g����ݒ肷��
	SetPlayerVelo(velo);


#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Move", GetColor(255, 255, 255));

#endif // _DEBUG

}
void PlayerStateMove::Exit()
{
}

void PlayerStateMove::OnCollide(std::shared_ptr<Collidable> collider)
{
}
