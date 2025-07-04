#pragma once

class Character;
class CharacterStateBase
{
public:
	virtual void Enter(Character* character) abstract;

	virtual void Update() abstract;

	virtual void Exit() abstract;

	/// <summary>
	/// 次の状態を取得する
	/// </summary>
	/// <returns>次のStateのポインタ</returns>
	CharacterStateBase* GetNextState()
	{
		return m_pNextState;
	}

	void ChangeState(CharacterStateBase* nextState)
	{
		m_pNextState = nextState;
	}

protected:

	Character* m_pCharacter = nullptr;

	CharacterStateBase* m_pNextState = nullptr;
};

