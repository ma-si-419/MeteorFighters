#include "CharacterStateSpecialAttack.h"
#include <cassert>
#include "CharacterBase.h"
#include <map>

namespace
{

	const std::map<CharacterBase::AttackKind,int> kAttackTimeMap = 
	{
		{CharacterBase::AttackKind::kEnergy,30},
		{CharacterBase::AttackKind::kPhysical,30},
		{CharacterBase::AttackKind::kBeam,30},
		{CharacterBase::AttackKind::kRush,30},
		{CharacterBase::AttackKind::kThrow,30},
		{CharacterBase::AttackKind::kAssault,30}
	}

}

CharacterStateSpecialAttack::CharacterStateSpecialAttack(std::shared_ptr<CharacterBase> character):
	CharacterStateBase(character),
	m_specialAttackNumber(-1),
	m_attackTime(0)
{

}

void CharacterStateSpecialAttack::Enter()
{
	//•KE‹Z‚ªİ’è‚³‚ê‚Ä‚¢‚È‚¯‚ê‚Î~‚ß‚é
	if (m_specialAttackNumber == -1) assert(false);

	//•KE‹Z‚Ìî•ñæ“¾
	auto attackData = m_pCharacter->GetSpecialAttackData(m_specialAttackNumber);

	//ƒAƒjƒ[ƒVƒ‡ƒ“‚Ìí—Şİ’è
	CharacterBase::AnimKind anim = CharacterBase::AnimKind::kIdle;

	//ˆê‚Â–Ú‚Ì•KE‹Z‚ğg—p‚·‚éê‡
	if (m_specialAttackNumber == 1)
	{
		anim = CharacterBase::AnimKind::kFirstSpecialAttack;
	}
	//“ñ‚Â–Ú‚Ì•KE‹Z‚ğg—p‚·‚éê‡
	else if (m_specialAttackNumber == 2)
	{
		anim = CharacterBase::AnimKind::kSecondSpecialAttack;
	}

	//ƒAƒjƒ[ƒVƒ‡ƒ“•ÏX
	m_pCharacter->ChangeAnim(anim,false);

	//‹Z‚Ìí—Ş‚ğİ’è‚·‚é
	auto kind = m_pCharacter->GetSpecialAttackData(m_specialAttackNumber).kind;

	//‹Z‚ğo‚µ‚Ä‚¢‚éŠÔ‚ğİ’è‚·‚é
	m_attackTime = kAttackTimeMap.at(kind);

}

void CharacterStateSpecialAttack::Update()
{
	//‚±‚ÌState‚É‚¢‚éŠÔ
	m_time++;

}

void CharacterStateSpecialAttack::Exit()
{

}
