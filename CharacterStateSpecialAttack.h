#pragma once
#include "CharacterStateBase.h"
class CharacterStateSpecialAttack : public CharacterStateBase
{
public:
	CharacterStateSpecialAttack(std::shared_ptr<CharacterBase> character);

	/// <summary>
	/// •KE‹Z‚Ì”Ô†‚ğİ’è‚·‚é
	/// </summary>
	/// <param name="number">•KE‹Z‚Ì”Ô†</param>
	void SetSpecialAttackNumber(int number) { m_specialAttackNumber = number; }

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

private:

	//ƒLƒƒƒ‰ƒNƒ^[‚Éİ’è‚³‚ê‚Ä‚¢‚é•KE‹Z‚Ì‰½”Ô–Ú‚ğg—p‚·‚é‚©
	int m_specialAttackNumber;

	//‹Z‚ğo‚µ‚Ä‚¢‚éŠÔ(ŒãŒ„‚ÌŠÔ‚Í‹Z‚²‚Æ‚Éİ’è)
	int m_attackTime;

};