#pragma once
#include "CharacterStateBase.h"
class CharacterStateTeleportation : public CharacterStateBase, public std::enable_shared_from_this<CharacterStateTeleportation>
{
public:
	CharacterStateTeleportation(std::shared_ptr<Character> character);

	/// <summary>
	/// �ړ�����ꏊ�Ǝ��Ԃ�ݒ肷��
	/// </summary>
	/// <param name="pos">�ړ�����ꏊ</param>
	/// <param name="time">�ړ��ɂ����鎞��</param>
	void Init(MyEngine::Vector3 pos, int time);

	virtual void Enter() override;

	virtual void Update() override;

	virtual void Exit() override;
private:
	int m_moveTime;

	MyEngine::Vector3 m_moveTargetPos;

	MyEngine::Vector3 m_moveVec;
};