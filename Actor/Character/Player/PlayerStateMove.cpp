#include "PlayerStateMove.h"
#include "PlayerStateIdle.h"
#include "PlayerStateNormalAttack.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"
#include <cmath>

PlayerStateMove::PlayerStateMove(std::shared_ptr<Player> player) :
	PlayerStateBase(player)
{
	m_pPlayer->ChangeAnim(CharacterBase::AnimKind::kSkyIdle,true);
}

void PlayerStateMove::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kMove;
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
	//�W�����v�{�^���������ꂽ��
	if (input.IsPress("RB"))
	{
		velo.y = GetSpeed();
	}
	//���~�{�^���������ꂽ��
	else if (input.IsPushTrigger(true))
	{
		velo.y = -GetSpeed();
	}
	
	//�U�����͂����ꂽ��
	if (input.IsTrigger("X"))
	{
		//����State�̃|�C���^�쐬
		std::shared_ptr<PlayerStateNormalAttack> next = std::make_shared<PlayerStateNormalAttack>(m_pPlayer);
		//���̍U�����s������AttackState�ɓn��
		next->SetAttack("Low1");
		//State��Attack�ɕύX����
		ChangeState(next);
		return;
	}
	else if (input.IsTrigger("Y"))
	{
		//����State�̃|�C���^�쐬
		std::shared_ptr<PlayerStateNormalAttack> next = std::make_shared<PlayerStateNormalAttack>(m_pPlayer);
		//���̍U�����s������AttackState�ɓn��
		next->SetAttack("Energy1");
		//State��Attack�ɕύX����
		ChangeState(next);
		return;
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
