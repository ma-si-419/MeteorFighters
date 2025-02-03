#pragma once
#include "ObjectBase.h"

class Rock : public ObjectBase
{
public:
	
	Rock();
	
	virtual ~Rock();

	virtual void Init() override;

	virtual void Update() override;

	/// <summary>
	/// �L�����N�^�[�̍��W�ƂԂ������ꏊ���Z�b�g����
	/// </summary>
	/// <param name="bumpPos">�Ԃ������ꏊ</param>
	/// <param name="characterPos">�L�����N�^�[�̍��W</param>
	void SetMoveVec(MyEngine::Vector3 bumpPos, MyEngine::Vector3 characterPos);
	
private:

	int m_time;

	MyEngine::Vector3 m_moveVec;
};

