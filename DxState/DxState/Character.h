#pragma once

class CharacterStateBase;
class Character
{
public:
	Character();
	~Character();

	void Init();

	void Update();

	void Draw();

private:

	CharacterStateBase* m_state;
};

