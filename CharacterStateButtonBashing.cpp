#include "CharacterStateButtonBashing.h"
#include "CharacterStateIdle.h"
#include "DxLib.h"

CharacterStateButtonBashing::CharacterStateButtonBashing(std::shared_ptr<CharacterBase> character) :
	CharacterStateBase(character)
{
}

void CharacterStateButtonBashing::Enter()
{
	m_pNextState = shared_from_this();
	m_kind = CharacterStateKind::kButtonBashing;
}

void CharacterStateButtonBashing::Update()
{
#ifdef _DEBUG

	DrawString(0, 16, "PlayerState:ButtonBashing", GetColor(255, 255, 255));

	if (GetCharacterInput()->IsTrigger("A"))
	{
		std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		ChangeState(next);
	}
#endif // _DEBUG

}
void CharacterStateButtonBashing::Exit()
{
}
