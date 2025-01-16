#include "CharacterStateButtonBashing.h"
#include "CharacterStateIdle.h"
#include "Character.h"
#include "DxLib.h"
#include "GameSceneConstant.h"
#include "GameManagerBase.h"


namespace
{
	const MyEngine::Vector3 kPlayerInitPos(-3, 100, -3);
	const MyEngine::Vector3 kEnemyInitPos(3, 100, 3);

	//���S���W
	const MyEngine::Vector3 kTargetPos(0, 100, 0);

	//�Ԃ��������̈ړ����x
	constexpr float kBumpSpeed = -10.0f;

	//�Ԃ����Ă���~�܂�͂��߂�܂ł̎���
	constexpr int kStopStartTime = 3;

	//�~�܂��
	constexpr float kStopPower = -(kBumpSpeed / 10.0f);

	//�~�܂������ɂǂ̂��炢�Ƃǂ܂邩
	constexpr int kHitBackStayTime = 15;

	//�ő呬�x
	constexpr float kMaxMoveSpeed = 3.0f;

	//�����x
	constexpr float kAcceleration = 0.2f;

	//����
	constexpr float kDashInitSpeed = 1.0f;

	//�u�����h�X�s�[�h
	constexpr float kBlendSpeed = 0.03f;
}

CharacterStateButtonBashing::CharacterStateButtonBashing(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
	m_moveSpeed(kBumpSpeed),
	m_isBump(false),
	m_bumpTime(0),
	m_stayTime(0),
	m_isStay(false)
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
	//�Ԃ����Ă��牽�t���[������������ۑ�����
	m_bumpTime++;

	//�G�̕���������
	m_pCharacter->LookTarget();

	//���S�܂ł̃x�N�g��
	MyEngine::Vector3 toTarget = kTargetPos - m_pCharacter->GetPos();

	//���S�Ɍ������Ĉړ�����
	MyEngine::Vector3 moveDir = (toTarget).Normalize();

	//�~�܂�n�߂�^�C�~���O�ɂȂ�����
	if (m_bumpTime > kStopStartTime)
	{
		//�ړ����x���}�C�i�X�ł����
		if (m_moveSpeed < 0.0f)
		{
			//�������~�߂Ă���
			m_moveSpeed += kStopPower;

			//�ړ����x���v���X�ɂȂ��
			if (m_moveSpeed > 0.0f)
			{
				m_isStay = true;
			}
		}
	}

	//��莞�ԂƂǂ܂�
	if (m_isStay)
	{
		//�~�܂��Ă��鎞�Ԃ𑫂��Ă���
		m_moveSpeed = 0.0f;
		m_stayTime++;

		//��莞�ԂƂǂ܂�����
		if (m_stayTime > kHitBackStayTime)
		{
			//�Ƃǂ܂�̂���߂�
			m_isStay = false;
			m_stayTime = 0;
			m_moveSpeed = kDashInitSpeed;
			if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
			{
				//��ڂ�Situation�ɍs��
				SetBashingSituation(static_cast<int>(GameManagerBase::ButtonBashingSituation::kSecondHit));
			}
		}
	}

	//�Ƃǂ܂�̂��I�������
	if (m_bumpTime > kStopStartTime && !m_isStay)
	{
		//�������Ă���
		m_moveSpeed += kAcceleration;
	}

	//�ړ����x�̃N�����v
	m_moveSpeed = fmin(m_moveSpeed, kMaxMoveSpeed);

	//�G�Ƃ̋���
	float toEnemyLength = (GetTargetPos() - m_pCharacter->GetPos()).Length();

	//�ړ��x�N�g��
	MyEngine::Vector3 moveVec;

	//�ړ��x�N�g���ňړ����邩�ǂ���
	bool isMove = true;

	//�G�ƂԂ��鋗���ɂȂ�����
	if ((GetTargetPos() - m_pCharacter->GetPos()).Length() < (GameSceneConstant::kCharacterRadius * 2.0f) + 3.0f)
	{
		//��x�Ԃ����Ă�����
		if (m_isBump)
		{
			//�ړ����Ȃ��悤�ɂ���
			isMove = false;
			SetCharacterVelo(MyEngine::Vector3(0, 0, 0));

			//��ڂ�Situation�ɍs��
			SetBashingSituation(static_cast<int>(GameManagerBase::ButtonBashingSituation::kFighting));

			//���W��␳����
			if (m_pCharacter->GetPlayerNumber() == Character::PlayerNumber::kOnePlayer)
			{
				SetCharacterPos(kPlayerInitPos);
			}
			else
			{
				SetCharacterPos(kEnemyInitPos);
			}

		}
		//���߂ĂԂ���^�C�~���O�ł����
		else
		{
			m_moveSpeed = kBumpSpeed;
			m_pCharacter->ChangeAnim(Character::AnimKind::kButtonBashingHitBack, false);
		}

		m_isBump = true;
		m_bumpTime = 0;
	}


	if (isMove)
	{
		moveVec = moveDir * m_moveSpeed;
	}

	//�ړ��x�N�g����ݒ�
	SetCharacterVelo(moveVec);

	//�A�j���[�V�����̕ύX

	//�ړ����x���O�����ɂȂ��Ă����
	if (m_moveSpeed >= 0.0f && !m_isStay)
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
			if (m_pCharacter->GetPlayAnimKind() != Character::AnimKind::kButtonBashingAttack)
			{
				m_pCharacter->ChangeAnim(Character::AnimKind::kButtonBashingAttack, false, kBlendSpeed);
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
