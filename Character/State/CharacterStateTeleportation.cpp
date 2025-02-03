#include "CharacterStateTeleportation.h"
#include "CharacterStateIdle.h"
#include "Character.h"

CharacterStateTeleportation::CharacterStateTeleportation(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
	m_moveTargetPos(),
	m_moveTime(0)
{
}

void CharacterStateTeleportation::Init(MyEngine::Vector3 pos, int time)
{
	//�ړ���̍��W�ݒ�
	m_moveTargetPos = pos;
	//�ړ��ɂ����鎞�Ԑݒ�
	m_moveTime = time;
	//�ړ��x�N�g���ݒ�
	MyEngine::Vector3 toTarget = m_moveTargetPos - m_pCharacter->GetPos();
	m_moveVec = toTarget / static_cast<float>(m_moveTime);
}

void CharacterStateTeleportation::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kTeleportation;

	SetDrawFlag(false);

	m_pCharacter->SetIsTrigger(true);

	m_guardKind = CharacterGuardKind::kDodge;

	//������̉����Đ�

	//�����_���ŉ�����ύX����
	if (GetRand(1))
	{
		m_pCharacter->PlayVoice(Character::VoiceKind::kDodge1);
	}
	else
	{
		m_pCharacter->PlayVoice(Character::VoiceKind::kDodge2);
	}
}

void CharacterStateTeleportation::Update()
{
	m_time++;

	SetCharacterVelo(m_moveVec);

	//�ݒ肵�����Ԃ�������A�C�h����Ԃɖ߂�
	if (m_time > static_cast<float>(m_moveTime))
	{
		auto next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		ChangeState(next);
	}
}

void CharacterStateTeleportation::Exit()
{
	m_pCharacter->LookTarget();

	SetDrawFlag(true);

	m_pCharacter->SetIsTrigger(false);
}

