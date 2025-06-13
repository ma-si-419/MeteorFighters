#pragma once

class Character;
class CharacterStateBase
{
public:
	virtual void Enter(Character* character) abstract;

	virtual void Update() abstract;

	virtual void Exit() abstract;

	/// <summary>
	/// Ÿ‚Ìó‘Ô‚ğæ“¾‚·‚é
	/// </summary>
	/// <returns>Ÿ‚ÌState‚Ìƒ|ƒCƒ“ƒ^</returns>
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

