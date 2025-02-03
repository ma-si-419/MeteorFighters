#pragma once
#include "CharacterStateBase.h"
class CharacterStateTeleportation : public CharacterStateBase, public std::enable_shared_from_this<CharacterStateTeleportation>
{
public:
	CharacterStateTeleportation(std::shared_ptr<Character> character);

	/// <summary>
	/// ˆÚ“®‚·‚éêŠ‚ÆŠÔ‚ğİ’è‚·‚é
	/// </summary>
	/// <param name="pos">ˆÚ“®‚·‚éêŠ</param>
	/// <param name="time">ˆÚ“®‚É‚©‚¯‚éŠÔ</param>
	void Init(MyEngine::Vector3 pos, int time);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;
private:
	int m_moveTime;

	MyEngine::Vector3 m_moveTargetPos;

	MyEngine::Vector3 m_moveVec;
};