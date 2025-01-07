#include "CharacterStateHitAttack.h"
#include "CharacterStateIdle.h"
#include "LocalPos.h"
#include "DxLib.h"
#include "Character.h"
#include "Attack.h"

namespace
{
	const std::map<Character::HitReactionKind, int> kDownTimeMap =
	{
		{Character::HitReactionKind::kGuard,10},
		{Character::HitReactionKind::kLow,40},
		{Character::HitReactionKind::kMiddle,40},
		{Character::HitReactionKind::kWeakUpBurst,60},
		{Character::HitReactionKind::kUpBurst,120},
		{Character::HitReactionKind::kFarBurst,120},
		{Character::HitReactionKind::kDownBurst,120},
		{Character::HitReactionKind::kMiddleStan,60},
		{Character::HitReactionKind::kBottomStan,60}
	};

	const std::map<Character::HitReactionKind, float> kMoveSpeedMap =
	{
		{Character::HitReactionKind::kGuard,0.2f},
		{Character::HitReactionKind::kLow,0.4f},
		{Character::HitReactionKind::kMiddle,0.8f},
		{Character::HitReactionKind::kWeakUpBurst,1.2f},
		{Character::HitReactionKind::kUpBurst,2.0f},
		{Character::HitReactionKind::kFarBurst,2.0f},
		{Character::HitReactionKind::kDownBurst,2.0f},
		{Character::HitReactionKind::kMiddleStan,0.1f},
		{Character::HitReactionKind::kBottomStan,0.1f}
	};

	//�O����U�����󂯂����̔���
	const std::map<Character::HitReactionKind, Character::AnimKind> kFrontHitReactionMap =
	{
		{Character::HitReactionKind::kGuard,Character::AnimKind::kGuardMiddle},
		{Character::HitReactionKind::kLow,Character::AnimKind::kLowHit1},
		{Character::HitReactionKind::kMiddle,Character::AnimKind::kMiddleHit},
		{Character::HitReactionKind::kWeakUpBurst,Character::AnimKind::kFrontBurst},
		{Character::HitReactionKind::kUpBurst,Character::AnimKind::kFrontBurst},
		{Character::HitReactionKind::kDownBurst,Character::AnimKind::kFrontBurst},
		{Character::HitReactionKind::kFarBurst,Character::AnimKind::kFrontBurst},
		{Character::HitReactionKind::kBottomStan,Character::AnimKind::kBottomStan},
		{Character::HitReactionKind::kMiddleStan,Character::AnimKind::kFrontMiddleStan}
	};

	//��납��U�����󂯂����̔���
	const std::map<Character::HitReactionKind, Character::AnimKind> kBackHitReactionMap =
	{
		{Character::HitReactionKind::kGuard,Character::AnimKind::kBackLowHit1},
		{Character::HitReactionKind::kLow,Character::AnimKind::kBackLowHit1},
		{Character::HitReactionKind::kMiddle,Character::AnimKind::kBackMiddleHit},
		{Character::HitReactionKind::kWeakUpBurst,Character::AnimKind::kBackBurst},
		{Character::HitReactionKind::kUpBurst,Character::AnimKind::kBackBurst},
		{Character::HitReactionKind::kDownBurst,Character::AnimKind::kBackBurst},
		{Character::HitReactionKind::kFarBurst,Character::AnimKind::kBackBurst},
		{Character::HitReactionKind::kBottomStan,Character::AnimKind::kBottomStan},
		{Character::HitReactionKind::kMiddleStan,Character::AnimKind::kBackMiddleStan}
	};

	//�ړ����鎞�Ԃ̊���
	constexpr float kMoveTimeRate = 0.7f;

	//�X�^�����̃A�j���[�V�������������Đ����鎞�Ԃ̊���
	constexpr float kSlowAnimTimeRate = 0.4f;
	//�X�^�����̃A�j���[�V�������������Đ�����Ƃ��̍Đ����x
	constexpr float kSlowAnimPlaySpeed = 0.3f;
}

CharacterStateHitAttack::CharacterStateHitAttack(std::shared_ptr<Character> character) :
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
	if (m_pCharacter->GetHitReaction() == Character::HitReactionKind::kBottomStan ||
		m_pCharacter->GetHitReaction() == Character::HitReactionKind::kMiddleStan)
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

	Character::HitReactionKind reaction = static_cast<Character::HitReactionKind>(kind);

	//�R���{���ɉ��̍U�����󂯂�����ۑ����Ă���
	int hitAttackKinds[static_cast<int>(Character::HitReactionKind::kKindNum)] = {};

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
	if (reaction == Character::HitReactionKind::kUpBurst)
	{
		//�΂ߏ�ɐ�����΂�
		local.SetLocalPos(MyEngine::Vector3(0, 1, 1));
	}
	//�������ɐ�����΂�����
	else if (reaction == Character::HitReactionKind::kDownBurst)
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
	if (reaction == Character::HitReactionKind::kMiddle)
	{
		//��x�󂯂Ă�����
		if (hitAttackKinds[static_cast<int>(kind)] > 0)
		{
			//�����Ԃ�ω�������
		}

	}
	//���i�X�^���U��
	else if (reaction == Character::HitReactionKind::kBottomStan)
	{
		//��x�󂯂Ă�����
		if (hitAttackKinds[static_cast<int>(kind)] > 0)
		{
			//�����Ԃ�ω�������

		}
	}
	//���i�X�^���U��
	else if (reaction == Character::HitReactionKind::kMiddleStan)
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
	m_pCharacter->ChangeAnim(static_cast<Character::AnimKind>(GetNextAnimKind(kind)), false);

	//�O����̐�����уA�j���[�V�����̏ꍇ
	if (m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kFrontBurst)
	{
		//�����Ă�������Ƌt�̕���������
		MyEngine::Vector3 frontPos = m_pCharacter->GetPos() - m_moveVec;

		m_pCharacter->SetFrontPos(frontPos);
	}
	//��납��̏ꍇ
	else if (m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kBackBurst)
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

	Character::AnimKind ans = Character::AnimKind::kLowHit1;

	Character::AnimKind animKind = m_pCharacter->GetPlayAnimKind();

	Character::HitReactionKind reaction = static_cast<Character::HitReactionKind>(kind);

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
	if (ans == Character::AnimKind::kLowHit1)
	{
		//��U��1���󂯂Ă����Ԃ�������
		if (animKind == Character::AnimKind::kLowHit1)
		{
			ans = Character::AnimKind::kLowHit2;
		}
		//��U��2���󂯂Ă����Ԃ�������
		else if (animKind == Character::AnimKind::kLowHit2)
		{
			ans = Character::AnimKind::kLowHit3;
		}
	}
	else if (ans == Character::AnimKind::kBackLowHit1)
	{
		//��U��1���󂯂Ă����Ԃ�������
		if (animKind == Character::AnimKind::kLowHit1)
		{
			ans = Character::AnimKind::kLowHit2;
		}
		//��U��2���󂯂Ă����Ԃ�������
		else if (animKind == Character::AnimKind::kLowHit2)
		{
			ans = Character::AnimKind::kLowHit3;
		}	
	}

	return static_cast<int>(ans);
}
