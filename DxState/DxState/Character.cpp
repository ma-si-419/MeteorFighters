#include "Character.h"
#include "CharacterStateIdle.h"

Character::Character():
	m_state(nullptr)
{
}

Character::~Character()
{
}

void Character::Init()
{
	// �ŏ���State��ݒ�
	m_state = new CharacterStateIdle();

	// State�̏�����
	m_state->Enter(this);
}

void Character::Update()
{
	// ����State���擾
	CharacterStateBase* nextState = m_state->GetNextState();

	// ����State������ꍇ�͐؂�ւ�
	if (nextState != nullptr)
	{
		// ���݂�State���I��
		m_state->Exit();
		// ����State�ɐ؂�ւ�
		delete m_state;
		m_state = nextState;
		// �V����State�̏�����
		m_state->Enter(this);
	}

	// ���݂�State�̍X�V���������s
	m_state->Update();
}

void Character::Draw()
{
}
