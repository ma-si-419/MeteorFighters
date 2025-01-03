#pragma once
#include "CharacterStateBase.h"
class CharacterStateDash : public CharacterStateBase, public std::enable_shared_from_this<CharacterStateDash>
{
public:
	CharacterStateDash(std::shared_ptr<Character> character);

	void SetMoveDir(MyEngine::Vector3 moveDir);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;

private:
	MyEngine::Vector3 m_moveDir;

	bool m_isDodge;
};

