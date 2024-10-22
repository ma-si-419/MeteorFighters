#include "EnemyStateHitAttack.h"
#include "EnemyStateIdle.h"
#include "LocalPos.h"
#include "DxLib.h"
#include "Enemy.h"

namespace
{
	constexpr int kDownTimes[static_cast<int>(EnemyStateHitAttack::HitKind::kKindNum)] =
	{
		15,
		20,
		40,
		40,
		40,
		60,
		60
	};

	constexpr float kMoveSpeed[static_cast<int>(EnemyStateHitAttack::HitKind::kKindNum)] =
	{
		0.3f,
		1.5f,
		3.0f,
		3.0f,
		3.0f,
		0.01f,
		0.01f
	};

	//�ړ����鎞�Ԃ̊���
	constexpr float kMoveTimeRate = 0.7f;
}

EnemyStateHitAttack::EnemyStateHitAttack(std::shared_ptr<Enemy> enemy):
	EnemyStateBase(enemy),
	m_downTime(0),
	m_moveTime(0),
	m_hitReaction(HitKind::kLow)
{
}

void EnemyStateHitAttack::Enter()
{
	m_kind = CharacterStateKind::kHitAttack;
	m_pNextState = shared_from_this();
}

void EnemyStateHitAttack::Update()
{

	m_time++;
	//�ݒ肵�����Ԃ�������
	if (m_downTime == m_time)
	{

		std::shared_ptr<EnemyStateIdle> next = std::make_shared<EnemyStateIdle>(m_pEnemy);

		//�A�C�h����Ԃɖ߂�
		ChangeState(next);
	}

	SetEnemyVelo(m_moveVec);


#ifdef _DEBUG

	DrawString(0, 32, "EnemyState:HitAttack", GetColor(255, 255, 255));

#endif // _DEBUG
}

void EnemyStateHitAttack::Exit()
{
}

void EnemyStateHitAttack::HitAttack(HitKind kind)
{

	//�R���{���ɉ��̍U�����󂯂�����ۑ����Ă���
	int hitAttackKinds[static_cast<int>(HitKind::kKindNum)] = {};

	for (auto item : m_hitReactions)
	{
		hitAttackKinds[static_cast<int>(item)]++;
	}


	//�����Ȃ����Ԃ�ݒ肷��
	m_downTime = kDownTimes[static_cast<int>(kind)];
	//���݂̂����Ԃ�ݒ肷��
	m_hitReaction = kind;
	//����������ݒ肷��
	MyEngine::Vector3 moveDir;
	//�U�����ꂽ�L�����N�^�[�̍��W�𒆐S�Ƃ������[�J�����W���쐬����
	LocalPos local;
	local.SetCenterPos(m_pEnemy->GetPos());
	//���[�J�����W�̑O�������U���������̂���U�����ꂽ���̂Ɍ�����
	MyEngine::Vector3 centerFrontPos = (m_pEnemy->GetPos() - GetPlayerPos()).Normalize() + m_pEnemy->GetPos();
	local.SetFrontPos(centerFrontPos);


	//������ɐ�����΂�����
	if (kind == HitKind::kUpBurst)
	{
		//�΂ߏ�ɐ�����΂�
		local.SetLocalPos(MyEngine::Vector3(0, 1, 1));
	}
	//�������ɐ�����΂�����
	else if (kind == HitKind::kDownBurst)
	{
		//�΂߉��ɐ�����΂�
		local.SetLocalPos(MyEngine::Vector3(0, -1, 1));
	}
	//����ȊO�̂���
	else
	{
		//��{�I�ɉ������ɓ�����
		local.SetLocalPos(MyEngine::Vector3(0, 0, 1));
	}

	moveDir = (local.GetWorldPos() - m_pEnemy->GetPos());

	//�����Ԃɂ���Ĉړ����x��ύX����
	m_moveVec = moveDir * kMoveSpeed[static_cast<int>(kind)];

	//�R���{�Ƃ����Ԃ̊m�F

	//�y��������΂��U��
	if (kind == HitKind::kMiddle)
	{
		//��x�󂯂Ă�����
		if (hitAttackKinds[static_cast<int>(kind)] > 0)
		{
			//�����Ԃ�ω�������
		}

	}
	//���i�X�^���U��
	else if (kind == HitKind::kBottomStan)
	{
		//��x�󂯂Ă�����
		if (hitAttackKinds[static_cast<int>(kind)] > 0)
		{
			//�����Ԃ�ω�������

		}
	}
	//���i�X�^���U��
	else if (kind == HitKind::kMiddleStan)
	{
		//��x�󂯂Ă�����
		if (hitAttackKinds[static_cast<int>(kind)] > 0)
		{
			//�����Ԃ�ω�������

		}
	}

	//���܂Ŏ󂯂��U����ۑ����Ă���
	m_hitReactions.push_back(kind);
}

void EnemyStateHitAttack::OnCollide(std::shared_ptr<Collidable> collider)
{

}