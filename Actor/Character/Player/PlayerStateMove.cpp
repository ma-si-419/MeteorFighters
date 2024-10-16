#include "PlayerStateMove.h"
#include "PlayerStateIdle.h"
#include "DxLib.h"
#include "Input.h"
#include "Player.h"
#include <cmath>

namespace
{
#ifdef _DEBUG
	constexpr float kSpeed = 0.7f;

#endif // _DEBUG
}

PlayerStateMove::PlayerStateMove(std::shared_ptr<Player> player) :
	PlayerStateBase(player)
{
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
	MyEngine::Vector3 moveVec;

	//�X�e�B�b�N�̏��擾
	MyEngine::Input::StickInfo stick = input.GetStickInfo();

	//���X�e�B�b�N�̌X���擾
	MyEngine::Vector3 leftStickDir(stick.leftStickX, 0, -stick.leftStickY);
	//�ړ��x�N�g����0����Ȃ����
	if (leftStickDir.sqLength() > 0.001)
	{
		//�ړ�����
		MyEngine::Vector3 moveDir = leftStickDir.Normalize();

		//�G�l�~�[�̕����Ɉړ���������]������
		float vX = GetEnemyPos().x - m_pPlayer->GetPos().x;
		float vZ = GetEnemyPos().z - m_pPlayer->GetPos().z;

		float angle = std::atan2f(vX, vZ);

		MyEngine::Vector3 rotation(0.0f, angle, 0.0f);

		MATRIX mat = rotation.GetRotationMat();

		moveDir = moveDir.MatTransform(mat);

		//�ړ������ɃX�s�[�h��������
		moveVec = moveDir * kSpeed;
	}
	
	//�ړ����Ă��Ȃ�������
	if (moveVec.sqLength() == 0)
	{
		//�A�C�h����Ԃɖ߂�
		std::shared_ptr<PlayerStateIdle> next = std::make_shared<PlayerStateIdle>(m_pPlayer);

		ChangeState(next);
	}
	//�ړ��x�N�g����ݒ肷��
	SetPlayerVelo(moveVec);


#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:Move", GetColor(255, 255, 255));

	if (MyEngine::Input::GetInstance().IsTrigger("A"))
	{
		std::shared_ptr<PlayerStateIdle> next = std::make_shared<PlayerStateIdle>(m_pPlayer);

		ChangeState(next);
	}
#endif // _DEBUG

}
void PlayerStateMove::Exit()
{
}
