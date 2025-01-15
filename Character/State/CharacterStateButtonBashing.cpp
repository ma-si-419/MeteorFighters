#include "CharacterStateButtonBashing.h"
#include "CharacterStateIdle.h"
#include "Character.h"
#include "DxLib.h"
#include "GameSceneConstant.h"


namespace
{
	const MyEngine::Vector3 kPlayerInitPos(-30, 100, -30);
	const MyEngine::Vector3 kEnemyInitPos(30, 100, 30);

	//���S���W
	const MyEngine::Vector3 kTargetPos(0, 100, 0);

	//�ŏ��̈ړ����x
	constexpr float kInitMoveSpeed = -3.5f;

	//�Ԃ��������̈ړ����x
	constexpr float kBumpSpeed = -5.0f;

	//�ő呬�x
	constexpr float kMaxMoveSpeed = 3.0f;

	//�����x
	constexpr float kAcceleration = 0.28f;
}

CharacterStateButtonBashing::CharacterStateButtonBashing(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
	m_moveSpeed(kInitMoveSpeed),
	m_isBump(false)
{
}

void CharacterStateButtonBashing::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kButtonBashing;

	//1P��2P���ōŏ��̍��W��ύX����
	if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
	{
		SetCharacterPos(kPlayerInitPos);
	}
	else
	{
		SetCharacterPos(kEnemyInitPos);
	}

	//�A�j���[�V�����ύX
	m_pCharacter->ChangeAnim(Character::AnimKind::kButtonBashingHitBack, false);

	//�{�^���A�ł��n�߂�ƃ}�l�[�W���[�ɓ`����
	StartButtonBashing();
}

void CharacterStateButtonBashing::Update()
{
	//�G�̕���������
	m_pCharacter->LookTarget();

	//���S�܂ł̃x�N�g��
	MyEngine::Vector3 toTarget = kTargetPos - m_pCharacter->GetPos();

	//���S�Ɍ������Ĉړ�����
	MyEngine::Vector3 moveDir = (toTarget).Normalize();

	//�ړ����x�𑫂��Ă���
	m_moveSpeed += kAcceleration;

	//�ړ����x���N�����v
	m_moveSpeed = fmin(m_moveSpeed, kMaxMoveSpeed);

	//�G�Ƃ̋���
	float toEnemyLength = (GetTargetPos() - m_pCharacter->GetPos()).Length();

	printfDx("%.3f\n", toEnemyLength);

	//�G�ƂԂ��鋗���ɂȂ�����
	if (toEnemyLength < (GameSceneConstant::kCharacterRadius * 2.0f) + 0.1f)
	{
		//��x�Ԃ����Ă�����
		if (m_isBump)
		{
			//////////////////////////////////////
			m_moveSpeed = 0.0f;
		}
		//���߂ĂԂ���^�C�~���O�ł����
		else
		{
			m_moveSpeed = kBumpSpeed;
			m_pCharacter->ChangeAnim(Character::AnimKind::kButtonBashingHitBack, false);
		}

		m_isBump = true;
	}

	//�ړ��x�N�g��
	MyEngine::Vector3 moveVec = moveDir * m_moveSpeed;

	//�ړ��x�N�g����ݒ�
	SetCharacterVelo(moveVec);

	//�A�j���[�V�����̕ύX

	//�ړ����x���O�����ɂȂ��Ă����
	if (m_moveSpeed > 0.0f)
	{
		//��x�Ԃ����Ă�����
		if (m_isBump)
		{
			if (m_pCharacter->GetPlayAnimKind() != Character::AnimKind::kOnButtonBashing)
			{
				m_pCharacter->ChangeAnim(Character::AnimKind::kOnButtonBashing, true);
			}
		}
		//��x���G�ƂԂ����Ă��Ȃ����
		else
		{
			//�A�j���[�V������ύX����
			if (m_pCharacter->GetPlayAnimKind() != Character::AnimKind::kButtonBashingPlayerAttack)
			{
				m_pCharacter->ChangeAnim(Character::AnimKind::kButtonBashingPlayerAttack, false);
			}
		}
	}

	//�{�^���A�ł���߂�^�C�~���O�ɂȂ�����
	if (!IsButtonBashing())
	{
		auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		ChangeState(next);

		//�����Ă����ꍇ
		if (IsBashWin())
		{

		}
		else
		{

		}
	}
}
void CharacterStateButtonBashing::Exit()
{
}
