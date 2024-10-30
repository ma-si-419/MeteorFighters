#include "EnemyStateHitAttack.h"
#include "EnemyStateIdle.h"
#include "LocalPos.h"
#include "DxLib.h"
#include "Enemy.h"
#include "Attack.h"

namespace
{
	constexpr int kDownTimes[static_cast<int>(EnemyStateHitAttack::HitKind::kKindNum)] =
	{
		40,
		40,
		90,
		90,
		90,
		60,
		60
	};

	constexpr float kMoveSpeed[static_cast<int>(EnemyStateHitAttack::HitKind::kKindNum)] =
	{
		0.3f,
		1.0f,
		3.0f,
		3.0f,
		3.0f,
		0.01f,
		0.01f
	};

	//�O����U�����󂯂����̔���
	const std::map<EnemyStateHitAttack::HitKind, CharacterBase::AnimKind> kFrontHitReactionMap =
	{
		{EnemyStateHitAttack::HitKind::kLow,CharacterBase::AnimKind::kLowHit1},
		{EnemyStateHitAttack::HitKind::kMiddle,CharacterBase::AnimKind::kMiddleHit},
		{EnemyStateHitAttack::HitKind::kUpBurst,CharacterBase::AnimKind::kFrontBurst},
		{EnemyStateHitAttack::HitKind::kDownBurst,CharacterBase::AnimKind::kFrontBurst},
		{EnemyStateHitAttack::HitKind::kFarBurst,CharacterBase::AnimKind::kFrontBurst},
		{EnemyStateHitAttack::HitKind::kBottomStan,CharacterBase::AnimKind::kBottomStan},
		{EnemyStateHitAttack::HitKind::kMiddleStan,CharacterBase::AnimKind::kFrontMiddleStan}
	};

	//��납��U�����󂯂����̔���
	const std::map<EnemyStateHitAttack::HitKind, CharacterBase::AnimKind> kBackHitReactionMap =
	{
		{EnemyStateHitAttack::HitKind::kLow,CharacterBase::AnimKind::kBackLowHit1},
		{EnemyStateHitAttack::HitKind::kMiddle,CharacterBase::AnimKind::kBackMiddleHit},
		{EnemyStateHitAttack::HitKind::kUpBurst,CharacterBase::AnimKind::kBackBurst},
		{EnemyStateHitAttack::HitKind::kDownBurst,CharacterBase::AnimKind::kBackBurst},
		{EnemyStateHitAttack::HitKind::kFarBurst,CharacterBase::AnimKind::kBackBurst},
		{EnemyStateHitAttack::HitKind::kBottomStan,CharacterBase::AnimKind::kBottomStan},
		{EnemyStateHitAttack::HitKind::kMiddleStan,CharacterBase::AnimKind::kBackMiddleStan}
	};

	//�ړ����鎞�Ԃ̊���
	constexpr float kMoveTimeRate = 0.4f;

	//�X�^�����̃A�j���[�V�������������Đ����鎞�Ԃ̊���
	constexpr float kSlowAnimTimeRate = 0.4f;
	//�X�^�����̃A�j���[�V�������������Đ�����Ƃ��̍Đ����x
	constexpr float kSlowAnimPlaySpeed = 0.3f;
}

EnemyStateHitAttack::EnemyStateHitAttack(std::shared_ptr<Enemy> enemy) :
	EnemyStateBase(enemy),
	m_downTime(0),
	m_moveTime(0),
	m_hitReaction(HitKind::kLow),
	m_isFrontHit(false)
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

	//���󂯂Ă���U�����X�^���U����������
	if (m_hitReaction == HitKind::kBottomStan ||
		m_hitReaction == HitKind::kMiddleStan)
	{
		int slowAnimTime = static_cast<int>(m_downTime * kSlowAnimTimeRate);

		if (m_time < slowAnimTime)
		{
			//�Đ����x���������ɂ���
			m_pEnemy->SetAnimPlaySpeed(kSlowAnimPlaySpeed);
		}
		else
		{
			//�Đ����x�������l�ɖ߂�
			m_pEnemy->SetAnimPlaySpeed();
		}
	}

	//�ݒ肵�����Ԃ�������
	if (m_downTime <= m_time)
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

	//�O�����牣��ꂽ���ǂ������擾����
	m_isFrontHit = m_pEnemy->IsFrontTarget(false);

	//�O�����牣���Ă�����v���C���[�̕���������
	if (m_isFrontHit)
	{
		m_pEnemy->SetFrontPos(GetPlayerPos());
	}
	//������牣���Ă�����v���C���[�Ƃ͋t�̕���������
	else
	{
		MyEngine::Vector3 frontPos;

		frontPos = (m_pEnemy->GetPos() - GetPlayerPos()).Normalize() + m_pEnemy->GetPos();

		m_pEnemy->SetFrontPos(frontPos);
	}

	//�A�j���[�V�����̕ύX
	m_pEnemy->ChangeAnim(static_cast<CharacterBase::AnimKind>(GetNextAnimKind(kind)), false);

	//���܂Ŏ󂯂��U����ۑ����Ă���
	m_hitReactions.push_back(kind);
}

void EnemyStateHitAttack::OnCollide(std::shared_ptr<Collidable> collider)
{
	if (collider->GetTag() == ObjectTag::kPlayerAttack)
	{
		auto attack = std::static_pointer_cast<Attack>(collider);

		auto status = attack->GetStatus();

		EnemyStateBase::HitAttack(attack, GetKind());
		printfDx("Hit\n");
	}
}

int EnemyStateHitAttack::GetNextAnimKind(HitKind kind)
{

	CharacterBase::AnimKind ans = CharacterBase::AnimKind::kLowHit1;

	CharacterBase::AnimKind animKind = m_pEnemy->GetPlayAnimKind();

	//���ʂ���U�����󂯂Ă���ꍇ
	if (m_isFrontHit)
	{
		ans = kFrontHitReactionMap.at(kind);
	}
	//��납��U�����󂯂Ă���ꍇ
	else
	{
		ans = kBackHitReactionMap.at(kind);
	}

	//��U�����󂯂��ꍇ�A���ԂɃA�j���[�V������������
	if (ans == CharacterBase::AnimKind::kLowHit1)
	{
		//��U��1���󂯂Ă����Ԃ�������
		if (animKind == CharacterBase::AnimKind::kLowHit1)
		{
			ans = CharacterBase::AnimKind::kLowHit2;
		}
		//��U��2���󂯂Ă����Ԃ�������
		else if (animKind == CharacterBase::AnimKind::kLowHit2)
		{
			ans = CharacterBase::AnimKind::kLowHit3;
		}
	}
	else if (ans == CharacterBase::AnimKind::kBackLowHit1)
	{
		//��U��1���󂯂Ă����Ԃ�������
		if (animKind == CharacterBase::AnimKind::kLowHit1)
		{
			ans = CharacterBase::AnimKind::kLowHit2;
		}
		//��U��2���󂯂Ă����Ԃ�������
		else if (animKind == CharacterBase::AnimKind::kLowHit2)
		{
			ans = CharacterBase::AnimKind::kLowHit3;
		}
	}

	return static_cast<int>(ans);
}
