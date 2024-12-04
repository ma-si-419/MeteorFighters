#include "CharacterStateHitAttack.h"
#include "CharacterStateIdle.h"
#include "LocalPos.h"
#include "DxLib.h"
#include "CharacterBase.h"
#include "Attack.h"

namespace
{
	const std::map<CharacterBase::HitReactionKind, int> kDownTimeMap =
	{
		{CharacterBase::HitReactionKind::kLow,40},
		{CharacterBase::HitReactionKind::kMiddle,40},
		{CharacterBase::HitReactionKind::kWeakUpBurst,60},
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
		{CharacterBase::HitReactionKind::kWeakUpBurst,1.2f},
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
		{CharacterBase::HitReactionKind::kWeakUpBurst,CharacterBase::AnimKind::kFrontBurst},
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
		{CharacterBase::HitReactionKind::kWeakUpBurst,CharacterBase::AnimKind::kBackBurst},
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

CharacterStateHitAttack::CharacterStateHitAttack(std::shared_ptr<CharacterBase> character) :
	CharacterStateBase(character),
	m_downTime(0),
	m_moveTime(0),
	m_isFrontHit(false)
{
}

void CharacterStateHitAttack::Enter()
{
	m_kind = CharacterStateKind::kHitAttack;
	m_pNextState = shared_from_this();
}

void CharacterStateHitAttack::Update()
{

	m_time++;
	printfDx("�q�b�g�A�^�b�N%.0f\n",m_time);

	//���󂯂Ă���U�����X�^���U����������
	if (m_pCharacter->GetHitReaction() == CharacterBase::HitReactionKind::kBottomStan ||
		m_pCharacter->GetHitReaction() == CharacterBase::HitReactionKind::kMiddleStan)
	{
		int slowAnimTime = static_cast<int>(m_downTime * kSlowAnimTimeRate);

		if (m_time < slowAnimTime)
		{
			//�Đ����x���������ɂ���
			m_pCharacter->SetAnimPlaySpeed(kSlowAnimPlaySpeed);
		}
		else
		{
			//�Đ����x�������l�ɖ߂�
			m_pCharacter->SetAnimPlaySpeed();
		}
	}

	//�ݒ肵�����Ԃ�������
	if (m_downTime <= m_time)
	{

		std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		//�A�C�h����Ԃɖ߂�
		ChangeState(next);
	}

	//�ړ����鎞�Ԃ��I�������
	if (m_time > static_cast<int>(m_downTime * kMoveTimeRate))
	{
		m_moveVec = MyEngine::Vector3(0, 0, 0);
		//TODO : ������я�Ԃł���ΐ�����я�Ԃ���ʏ��Ԃɖ߂�A�j���[�V�������Đ�����
	}

	SetCharacterVelo(m_moveVec);


#ifdef _DEBUG

	DrawString(0, 32, "PlayerState:HitAttack", GetColor(255, 255, 255));

#endif // _DEBUG
}

void CharacterStateHitAttack::Exit()
{
}

void CharacterStateHitAttack::HitAttack(int kind)
{

	CharacterBase::HitReactionKind reaction = static_cast<CharacterBase::HitReactionKind>(kind);

	//�R���{���ɉ��̍U�����󂯂�����ۑ����Ă���
	int hitAttackKinds[static_cast<int>(CharacterBase::HitReactionKind::kKindNum)] = {};

	for (auto item : m_hitReactions)
	{
		hitAttackKinds[item]++;
	}

	//�����Ȃ����Ԃ�ݒ肷��
	m_downTime = kDownTimeMap.at(reaction);
	//���݂̂����Ԃ�ݒ肷��
	m_pCharacter->SetHitReaction(reaction);
	//����������ݒ肷��
	MyEngine::Vector3 moveDir;
	//�U�����ꂽ�L�����N�^�[�̍��W�𒆐S�Ƃ������[�J�����W���쐬����
	MyEngine::LocalPos local;
	local.SetCenterPos(m_pCharacter->GetPos());
	//���[�J�����W�̑O�������U���������̂���U�����ꂽ���̂Ɍ�����
	MyEngine::Vector3 centerFrontPos = (m_pCharacter->GetPos() - GetTargetPos()).Normalize() + m_pCharacter->GetPos();
	local.SetFrontPos(centerFrontPos);


	//������ɐ�����΂�����
	if (reaction == CharacterBase::HitReactionKind::kUpBurst)
	{
		//�΂ߏ�ɐ�����΂�
		local.SetLocalPos(MyEngine::Vector3(0, 1, 1));
	}
	//�������ɐ�����΂�����
	else if (reaction == CharacterBase::HitReactionKind::kDownBurst)
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

	moveDir = (local.GetWorldPos() - m_pCharacter->GetPos());

	//�����Ԃɂ���Ĉړ����x��ύX����
	m_moveVec = moveDir * kMoveSpeedMap.at(reaction);

	//�R���{�Ƃ����Ԃ̊m�F

	//�y��������΂��U��
	if (reaction == CharacterBase::HitReactionKind::kMiddle)
	{
		//��x�󂯂Ă�����
		if (hitAttackKinds[static_cast<int>(kind)] > 0)
		{
			//�����Ԃ�ω�������
		}

	}
	//���i�X�^���U��
	else if (reaction == CharacterBase::HitReactionKind::kBottomStan)
	{
		//��x�󂯂Ă�����
		if (hitAttackKinds[static_cast<int>(kind)] > 0)
		{
			//�����Ԃ�ω�������

		}
	}
	//���i�X�^���U��
	else if (reaction == CharacterBase::HitReactionKind::kMiddleStan)
	{
		//��x�󂯂Ă�����
		if (hitAttackKinds[static_cast<int>(kind)] > 0)
		{
			//�����Ԃ�ω�������

		}
	}

	//�O�����牣��ꂽ���ǂ������擾����
	m_isFrontHit = m_pCharacter->IsFrontTarget();

	//�O�����牣���Ă�����v���C���[�̕���������
	if (m_isFrontHit)
	{
		m_pCharacter->LookTarget();
	}
	//������牣���Ă�����v���C���[�Ƃ͋t�̕���������
	else
	{
		MyEngine::Vector3 frontPos;

		frontPos = (m_pCharacter->GetPos() - GetTargetPos()) + m_pCharacter->GetPos();

		m_pCharacter->SetFrontPos(frontPos);
	}

	//�A�j���[�V�����̕ύX
	m_pCharacter->ChangeAnim(static_cast<CharacterBase::AnimKind>(GetNextAnimKind(kind)), false);

	//�O����̐�����уA�j���[�V�����̏ꍇ
	if (m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kFrontBurst)
	{
		//�����Ă�������Ƌt�̕���������
		MyEngine::Vector3 frontPos = m_pCharacter->GetPos() - m_moveVec;

		m_pCharacter->SetFrontPos(frontPos);
	}
	//��납��̏ꍇ
	else if (m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kBackBurst)
	{
		//�����Ă������������
		MyEngine::Vector3 frontPos = m_pCharacter->GetPos() + m_moveVec;

		m_pCharacter->SetFrontPos(frontPos);
	}

	//���܂Ŏ󂯂��U����ۑ����Ă���
	m_hitReactions.push_back(kind);
}

int CharacterStateHitAttack::GetNextAnimKind(int kind)
{

	CharacterBase::AnimKind ans = CharacterBase::AnimKind::kLowHit1;

	CharacterBase::AnimKind animKind = m_pCharacter->GetPlayAnimKind();

	CharacterBase::HitReactionKind reaction = static_cast<CharacterBase::HitReactionKind>(kind);

	//���ʂ���U�����󂯂Ă���ꍇ
	if (m_isFrontHit)
	{
		ans = kFrontHitReactionMap.at(reaction);
	}
	//��납��U�����󂯂Ă���ꍇ
	else
	{
		ans = kBackHitReactionMap.at(reaction);
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
