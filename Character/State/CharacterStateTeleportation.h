#pragma once
#include "CharacterStateBase.h"
class CharacterStateTeleportation : public CharacterStateBase, public std::enable_shared_from_this<CharacterStateTeleportation>
{
public:
	CharacterStateTeleportation(std::shared_ptr<Character> character);

	/// <summary>
	/// 移動する場所と時間を設定する
	/// </summary>
	/// <param name="pos">移動する場所</param>
	/// <param name="time">移動にかける時間</param>
	void Init(MyEngine::Vector3 pos, int time);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;
private:
	int m_moveTime;

	MyEngine::Vector3 m_moveTargetPos;

	MyEngine::Vector3 m_moveVec;
};