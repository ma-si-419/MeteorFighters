#include "EnemyStateHitAttack.h"
#include "EnemyStateIdle.h"
#include "LocalPos.h"
#include "DxLib.h"
#include "Enemy.h"

namespace
{
	constexpr int kDownTimes[static_cast<int>(EnemyStateHitAttack::HitReaction::kReactionNum)] =
	{
		40,
		40,
		50,
		60,
		180,
		180,
		180
	};
}

void EnemyStateHitAttack::HitAttack(HitReaction reaction, BurstPower power, HitDirection direction)
{

	int reactions[static_cast<int>(HitReaction::kReactionNum)] = {};

	for (auto item : m_hitReactions)
	{
		reactions[static_cast<int>(item)]++;
	}


	//�_�E���^�C����ݒ肷��
	m_downTime = kDownTimes[static_cast<int>(reaction)];
	//���݂̂����Ԃ�ݒ肷��
	m_hitReaction = reaction;
	//����������ݒ肷��
	MyEngine::Vector3 moveDir;
	//�U����������̍��W�𒆐S�Ƃ������[�J�����W���쐬����
	LocalPos local;
	local.SetCenterPos(GetPlayerPos());
	MyEngine::Vector3 centerFrontPos = m_pEnemy->GetPos() - GetPlayerPos() + GetPlayerPos();
	local.SetFrontPos(centerFrontPos);

	if (direction == HitDirection::kUp)
	{
		

	}
	else if (direction == HitDirection::kDown)
	{

	}
	else if (direction == HitDirection::kFar)
	{

	}

	//�R���{�Ƃ����Ԃ̊m�F
	if (reaction == HitReaction::kLow)
	{

	}
	else if (reaction == HitReaction::kMid)
	{

	}
	else if (reaction == HitReaction::kHigh)
	{

	}
	else if (reaction == HitReaction::kBurst)
	{

	}
	else if (reaction == HitReaction::kBottomStan)
	{
		//�����X�^���U�����x�󂯂Ă�����
		if (reactions[static_cast<int>(reaction)] > 0)
		{
			//�����Ԃ�ω�������
			
		}
	}
	else if (reaction == HitReaction::kMiddleStan)
	{
		//�����X�^���U�����x�󂯂Ă�����
		if (reactions[static_cast<int>(reaction)] > 0)
		{
			//�����Ԃ�ω�������

		}
	}
	else if (reaction == HitReaction::kTopStan)
	{
		//�����X�^���U�����x�󂯂Ă�����
		if (reactions[static_cast<int>(reaction)] > 0)
		{
			//�����Ԃ�ω�������

		}
	}

	//���܂Ŏ󂯂��U����ۑ����Ă���
	m_hitReactions.push_back(reaction);
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
	if (m_downTime = m_time)
	{

		std::shared_ptr<EnemyStateIdle> next = std::make_shared<EnemyStateIdle>();

		//�A�C�h����Ԃɖ߂�
		ChangeState(next);
	}


#ifdef _DEBUG

	DrawString(0, 32, "EnemyState:HitAttack", GetColor(255, 255, 255));

#endif // _DEBUG
}

void EnemyStateHitAttack::Exit()
{
}

void EnemyStateHitAttack::OnCollide(std::shared_ptr<Collidable> collider)
{
}