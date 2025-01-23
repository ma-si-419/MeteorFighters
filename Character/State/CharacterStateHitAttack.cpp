#include "CharacterStateHitAttack.h"
#include "CharacterStateIdle.h"
#include "CharacterStateDown.h"
#include "LocalPos.h"
#include "DxLib.h"
#include "Character.h"
#include "Attack.h"
#include "GameManagerBase.h"

namespace
{
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

	//�X�^�����̃A�j���[�V�������������Đ����鎞�Ԃ̊���
	constexpr float kSlowAnimTimeRate = 0.4f;
	//�X�^�����̃A�j���[�V�������������Đ�����Ƃ��̍Đ����x
	constexpr float kSlowAnimPlaySpeed = 0.3f;

	//�ړ����x�����������Ă����^�C�~���O
	constexpr float kMoveSpeedDecelerationTime = 0.7f;

	//������ђ��ɂ�����d��
	constexpr float kBurstGravity = 0.1f;

	//�n�ʂƂԂ���Ɣ��肷��ړ��x�N�g����y�̒l
	constexpr float kGroundHitY = 3.0f;

	//�ǂƂԂ���Ɣ��肷��ړ��x�N�g���̒���
	constexpr float kWallHitLength = 10.0f;

}

CharacterStateHitAttack::CharacterStateHitAttack(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
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

	//���󂯂Ă���U�����X�^���U����������
	if (m_pCharacter->GetHitReaction() == Character::HitReactionKind::kBottomStan ||
		m_pCharacter->GetHitReaction() == Character::HitReactionKind::kMiddleStan)
	{
		int slowAnimTime = static_cast<int>(m_stopTime * kSlowAnimTimeRate);

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

	//������я�Ԃł����
	if (m_pCharacter->GetHitReaction() == Character::HitReactionKind::kUpBurst ||
		m_pCharacter->GetHitReaction() == Character::HitReactionKind::kDownBurst ||
		m_pCharacter->GetHitReaction() == Character::HitReactionKind::kFarBurst)
	{
		//�ړ��x�N�g�������ȏ㉺�������Ă��Ēn�ʂƂԂ����Ă�����
		if (m_pCharacter->IsGround() &&
			m_moveVec.y < -kGroundHitY)
		{
			//���̏�Ԃ��_�E����Ԃɐݒ肷��
			std::shared_ptr<CharacterStateDown> next = std::make_shared<CharacterStateDown>(m_pCharacter);
			//�_�E���̕�����ݒ肷��
			next->SetFrontHit(m_isFrontHit);
			//�����Ă����������ł�������Ƌt�����ɂ���
			m_pCharacter->SetFrontPos(m_pCharacter->GetPos() - m_moveVec);
			//�_�E����ԂɑJ�ڂ���
			ChangeState(next);

			return;
		}

		//�ǂƂԂ����Ă�����
		if (m_pCharacter->IsWall() &&
			m_moveVec.Length() < kWallHitLength)
		{
			//���̏�Ԃ��_�E����Ԃɐݒ肷��
			std::shared_ptr<CharacterStateDown> next = std::make_shared<CharacterStateDown>(m_pCharacter);
			//�_�E���̕�����ݒ肷��
			next->SetFrontHit(m_isFrontHit);
			//�����Ă����������ł�������Ƌt�����ɂ���
			m_pCharacter->SetFrontPos(m_pCharacter->GetPos() - m_moveVec);
			//�_�E����ԂɑJ�ڂ���
			ChangeState(next);
			return;
		}


		//�ݒ肵�����Ԃ�������
		if (m_stopTime <= m_time)
		{
			//���̏�Ԃ��A�C�h����Ԃɐݒ肷��
			std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);
			//�A�C�h����ԂɑJ�ڂ���
			ChangeState(next);
			
			return;
		}
	}
	//����ȊO�̏�Ԃł����
	else
	{
		//�ݒ肵�����Ԃ�������
		if (m_stopTime <= m_time)
		{
			//���̏�Ԃ��A�C�h����Ԃɐݒ肷��
			std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);
			//�A�C�h����ԂɑJ�ڂ���
			ChangeState(next);

			return;
		}
	}

	//���������Ă������ԂɂȂ��Ă�����
	if (m_stopTime * kMoveSpeedDecelerationTime < m_time)
	{
		//�����x�N�g�����ݒ肳��Ă��Ȃ����
		if (m_decelerationVec.SqLength() < 0.01f)
		{
			//�����x�N�g�����v�Z����
			m_decelerationVec = m_moveVec / static_cast<float>(m_stopTime * kMoveSpeedDecelerationTime);
		}

		//�����x�N�g�������ړ��x�N�g���̒������傫�����
		if (m_moveVec.SqLength() > m_decelerationVec.SqLength())
		{
			//�����x�N�g�����ړ��x�N�g���ɉ��Z����
			m_moveVec += m_decelerationVec;
		}
		//�����x�N�g�������ړ��x�N�g���̒��������������
		else
		{
			//�����x�N�g����0�ɂ���
			m_decelerationVec = MyEngine::Vector3();
		}

		//������ђ��͏d�͂�������
		if (m_pCharacter->GetHitReaction() == Character::HitReactionKind::kUpBurst ||
			m_pCharacter->GetHitReaction() == Character::HitReactionKind::kDownBurst ||
			m_pCharacter->GetHitReaction() == Character::HitReactionKind::kFarBurst)
		{
			m_moveVec.y -= kBurstGravity;
		}
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

	//���݂̂����Ԃ�ݒ肷��
	m_pCharacter->SetHitReaction(reaction);
	//����������ݒ肷��
	MyEngine::Vector3 moveDir;
	//�U�����ꂽ�L�����N�^�[�̍��W�𒆐S�Ƃ������[�J�����W���쐬����
	MyEngine::LocalPos local;
	local.SetCenterPos(m_pCharacter->GetPos());
	//���[�J�����W�̑O�������U���������̂���U�����ꂽ���̂Ɍ�����
	MyEngine::Vector3 centerFrontPos = (m_pCharacter->GetPos() - m_pManager->GetTargetPos(m_pCharacter)).Normalize() + m_pCharacter->GetPos();
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

		frontPos = (m_pCharacter->GetPos() - m_pManager->GetTargetPos(m_pCharacter)) + m_pCharacter->GetPos();

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
