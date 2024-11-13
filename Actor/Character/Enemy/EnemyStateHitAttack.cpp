#include "EnemyStateHitAttack.h"
#include "EnemyStateIdle.h"
#include "LocalPos.h"
#include "DxLib.h"
#include "Enemy.h"
#include "Attack.h"

namespace
{
	const std::map<CharacterBase::HitReactionKind, int> kDownTimeMap =
	{
		{CharacterBase::HitReactionKind::kLow,40},
		{CharacterBase::HitReactionKind::kMiddle,40},
		{CharacterBase::HitReactionKind::kUpBurst,120},
		{CharacterBase::HitReactionKind::kFarBurst,120},
		{CharacterBase::HitReactionKind::kDownBurst,120},
		{CharacterBase::HitReactionKind::kMiddleStan,60},
		{CharacterBase::HitReactionKind::kBottomStan,60}
	};

	const std::map<CharacterBase::HitReactionKind, float> kMoveSpeedMap =
	{
		{CharacterBase::HitReactionKind::kLow,0.4f},
		{CharacterBase::HitReactionKind::kMiddle,0.8f},
		{CharacterBase::HitReactionKind::kUpBurst,2.0f},
		{CharacterBase::HitReactionKind::kFarBurst,2.0f},
		{CharacterBase::HitReactionKind::kDownBurst,2.0f},
		{CharacterBase::HitReactionKind::kMiddleStan,0.1f},
		{CharacterBase::HitReactionKind::kBottomStan,0.1f}
	};

	//�O����U�����󂯂����̔���
	const std::map<CharacterBase::HitReactionKind, CharacterBase::AnimKind> kFrontHitReactionMap =
	{
		{CharacterBase::HitReactionKind::kLow,CharacterBase::AnimKind::kLowHit1},
		{CharacterBase::HitReactionKind::kMiddle,CharacterBase::AnimKind::kMiddleHit},
		{CharacterBase::HitReactionKind::kUpBurst,CharacterBase::AnimKind::kFrontBurst},
		{CharacterBase::HitReactionKind::kDownBurst,CharacterBase::AnimKind::kFrontBurst},
		{CharacterBase::HitReactionKind::kFarBurst,CharacterBase::AnimKind::kFrontBurst},
		{CharacterBase::HitReactionKind::kBottomStan,CharacterBase::AnimKind::kBottomStan},
		{CharacterBase::HitReactionKind::kMiddleStan,CharacterBase::AnimKind::kFrontMiddleStan}
	};

	//��납��U�����󂯂����̔���
	const std::map<CharacterBase::HitReactionKind, CharacterBase::AnimKind> kBackHitReactionMap =
	{
		{CharacterBase::HitReactionKind::kLow,CharacterBase::AnimKind::kBackLowHit1},
		{CharacterBase::HitReactionKind::kMiddle,CharacterBase::AnimKind::kBackMiddleHit},
		{CharacterBase::HitReactionKind::kUpBurst,CharacterBase::AnimKind::kBackBurst},
		{CharacterBase::HitReactionKind::kDownBurst,CharacterBase::AnimKind::kBackBurst},
		{CharacterBase::HitReactionKind::kFarBurst,CharacterBase::AnimKind::kBackBurst},
		{CharacterBase::HitReactionKind::kBottomStan,CharacterBase::AnimKind::kBottomStan},
		{CharacterBase::HitReactionKind::kMiddleStan,CharacterBase::AnimKind::kBackMiddleStan}
	};

	//�ړ����鎞�Ԃ̊���
	constexpr float kMoveTimeRate = 0.7f;

	//�X�^�����̃A�j���[�V�������������Đ����鎞�Ԃ̊���
	constexpr float kSlowAnimTimeRate = 0.4f;
	//�X�^�����̃A�j���[�V�������������Đ�����Ƃ��̍Đ����x
	constexpr float kSlowAnimPlaySpeed = 0.3f;
}

EnemyStateHitAttack::EnemyStateHitAttack(std::shared_ptr<Enemy> enemy) :
	EnemyStateBase(enemy),
	m_downTime(0),
	m_moveTime(0),
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
	if (m_pEnemy->GetHitReaction() == CharacterBase::HitReactionKind::kBottomStan ||
		m_pEnemy->GetHitReaction() == CharacterBase::HitReactionKind::kMiddleStan)
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

	//�ړ����鎞�Ԃ��I�������
	if (m_time > static_cast<int>(m_downTime * kMoveTimeRate))
	{
		m_moveVec = MyEngine::Vector3(0, 0, 0);
		//TODO : ������я�Ԃł���ΐ�����я�Ԃ���ʏ��Ԃɖ߂�A�j���[�V�������Đ�����
	}

	SetEnemyVelo(m_moveVec);


#ifdef _DEBUG

	DrawString(0, 32, "EnemyState:HitAttack", GetColor(255, 255, 255));

#endif // _DEBUG
}

void EnemyStateHitAttack::Exit()
{
}

void EnemyStateHitAttack::HitAttack(CharacterBase::HitReactionKind kind)
{

	//�R���{���ɉ��̍U�����󂯂�����ۑ����Ă���
	int hitAttackKinds[static_cast<int>(CharacterBase::HitReactionKind::kKindNum)] = {};

	for (auto item : m_hitReactions)
	{
		hitAttackKinds[static_cast<int>(item)]++;
	}

	//�����Ȃ����Ԃ�ݒ肷��
	m_downTime = kDownTimeMap.at(kind);
	//���݂̂����Ԃ�ݒ肷��
	m_pEnemy->SetHitReaction(kind);
	//����������ݒ肷��
	MyEngine::Vector3 moveDir;
	//�U�����ꂽ�L�����N�^�[�̍��W�𒆐S�Ƃ������[�J�����W���쐬����
	LocalPos local;
	local.SetCenterPos(m_pEnemy->GetPos());
	//���[�J�����W�̑O�������U���������̂���U�����ꂽ���̂Ɍ�����
	MyEngine::Vector3 centerFrontPos = (m_pEnemy->GetPos() - GetPlayerPos()).Normalize() + m_pEnemy->GetPos();
	local.SetFrontPos(centerFrontPos);


	//������ɐ�����΂�����
	if (kind == CharacterBase::HitReactionKind::kUpBurst)
	{
		//�΂ߏ�ɐ�����΂�
		local.SetLocalPos(MyEngine::Vector3(0, 1, 1));
	}
	//�������ɐ�����΂�����
	else if (kind == CharacterBase::HitReactionKind::kDownBurst)
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
	m_moveVec = moveDir * kMoveSpeedMap.at(kind);

	//�R���{�Ƃ����Ԃ̊m�F

	//�y��������΂��U��
	if (kind == CharacterBase::HitReactionKind::kMiddle)
	{
		//��x�󂯂Ă�����
		if (hitAttackKinds[static_cast<int>(kind)] > 0)
		{
			//�����Ԃ�ω�������
		}

	}
	//���i�X�^���U��
	else if (kind == CharacterBase::HitReactionKind::kBottomStan)
	{
		//��x�󂯂Ă�����
		if (hitAttackKinds[static_cast<int>(kind)] > 0)
		{
			//�����Ԃ�ω�������

		}
	}
	//���i�X�^���U��
	else if (kind == CharacterBase::HitReactionKind::kMiddleStan)
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

		frontPos = (m_pEnemy->GetPos() - GetPlayerPos()) + m_pEnemy->GetPos();

		m_pEnemy->SetFrontPos(frontPos);
	}

	//�A�j���[�V�����̕ύX
	m_pEnemy->ChangeAnim(static_cast<CharacterBase::AnimKind>(GetNextAnimKind(kind)), false);

	//�O����̐�����уA�j���[�V�����̏ꍇ
	if (m_pEnemy->GetPlayAnimKind() == CharacterBase::AnimKind::kFrontBurst)
	{
		//�����Ă�������Ƌt�̕���������
		MyEngine::Vector3 frontPos = m_pEnemy->GetPos() - m_moveVec;

		m_pEnemy->SetFrontPos(frontPos);
	}
	//��납��̏ꍇ
	else if (m_pEnemy->GetPlayAnimKind() == CharacterBase::AnimKind::kBackBurst)
	{
		//�����Ă������������
		MyEngine::Vector3 frontPos = m_pEnemy->GetPos() + m_moveVec;

		m_pEnemy->SetFrontPos(frontPos);
	}
	
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

int EnemyStateHitAttack::GetNextAnimKind(CharacterBase::HitReactionKind kind)
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
