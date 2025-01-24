#pragma once
#include "Collidable.h"

class Rock : public Collidable
{
public:
	
	Rock();
	
	virtual ~Rock();

	/// <summary>
	/// ����������
	/// </summary>
	/// <param name="handle">���f���n���h��</param>
	void Init(int handle);

	void Update();
	
	void Draw();

	void End();

	/// <summary>
	/// �L�����N�^�[�̍��W�ƂԂ������ꏊ���Z�b�g����
	/// </summary>
	/// <param name="bumpPos">�Ԃ������ꏊ</param>
	/// <param name="characterPos">�L�����N�^�[�̍��W</param>
	void SetBumpPosAndCharacterPos(MyEngine::Vector3 bumpPos, MyEngine::Vector3 characterPos);
	
private:

	int m_alpha;

	int m_handle;

	MyEngine::Vector3 m_moveVec;
};

