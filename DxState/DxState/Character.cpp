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
	// 最初のStateを設定
	m_state = new CharacterStateIdle();

	// Stateの初期化
	m_state->Enter(this);
}

void Character::Update()
{
	// 次のStateを取得
	CharacterStateBase* nextState = m_state->GetNextState();

	// 次のStateがある場合は切り替え
	if (nextState != nullptr)
	{
		// 現在のStateを終了
		m_state->Exit();
		// 次のStateに切り替え
		delete m_state;
		m_state = nextState;
		// 新しいStateの初期化
		m_state->Enter(this);
	}

	// 現在のStateの更新処理を実行
	m_state->Update();
}

void Character::Draw()
{
}
