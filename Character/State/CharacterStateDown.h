#pragma once
#include "CharacterStateBase.h"
class CharacterStateDown : public CharacterStateBase, public std::enable_shared_from_this<CharacterStateDown>
{
public:
	CharacterStateDown(std::shared_ptr<Character> character);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

	void SetFrontHit(bool front);

	/// <summary>
	/// •œ‹A‚Å‚«‚é‚©‚Ç‚¤‚©‚ðŽæ“¾‚·‚é
	/// </summary>
	/// <returns>•œ‹A‚Å‚«‚é‚È‚çtrue</returns>
	bool IsReturn() { return m_isReturn; }
private:

	//•œ‹A‚Å‚«‚é‚©‚Ç‚¤‚©
	bool m_isReturn;

};