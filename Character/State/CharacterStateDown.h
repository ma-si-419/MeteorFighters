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
	/// ���A�ł��邩�ǂ������擾����
	/// </summary>
	/// <returns>���A�ł���Ȃ�true</returns>
	bool IsReturn() { return m_isReturn; }
private:

	//���A�ł��邩�ǂ���
	bool m_isReturn;

};