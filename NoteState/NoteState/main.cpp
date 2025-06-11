#include <iostream>


// ���ׂĂ�State�̊��N���X
class StateBase
{
public:

	// �J�n����
	virtual void Enter() abstract;

	// �X�V����
	virtual void Update() abstract;

	// �I������
	virtual void Exit() abstract;

	// ��Ԃ�ύX���鎞�Ɏg�p����
	void ChangeState(StateBase* next)
	{
		// �I���������s��
		Exit();

		// ���̏�Ԃ�ݒ肷��
		nextState = next;

		// ���̏�Ԃ̊J�n�������s��
		nextState->Enter();
	}
	// ����State�̃|�C���^���擾����
	StateBase* GetNextPointer() const
	{
		return nextState;
	}

private:

	// ����State�̃|�C���^
	StateBase* nextState = nullptr;

};

class StateIdle : public StateBase
{

public:
	// �J�n����
	void Enter() override
	{
		std::cout << "�A�C�h���J�n" << std::endl;
	}
	// �X�V����
	void Update() override
	{
		std::cout << "�A�C�h���X�V" << std::endl;
	}
	// �I������
	void Exit() override
	{
		std::cout << "�A�C�h���I��" << std::endl;
	}
};

class StateMove : public StateBase
{
public:
	// �J�n����
	void Enter() override
	{
		std::cout << "�ړ��J�n" << std::endl;
	}
	// �X�V����
	void Update() override
	{
		std::cout << "�ړ��X�V" << std::endl;
	}
	// �I������
	void Exit() override
	{
		std::cout << "�ړ��I��" << std::endl;
	}
};

class StateAttack : public StateBase
{
public:
	// �J�n����
	void Enter() override
	{
		std::cout << "�U���J�n" << std::endl;
	}
	// �X�V����
	void Update() override
	{
		std::cout << "�U���X�V" << std::endl;
	}
	// �I������
	void Exit() override
	{
		std::cout << "�U���I��" << std::endl;
	}
};

class StateDefense : public StateBase
{
public:
	// �J�n����
	void Enter() override
	{
		std::cout << "�h��J�n" << std::endl;
	}
	// �X�V����
	void Update() override
	{
		std::cout << "�h��X�V" << std::endl;
	}
	// �I������
	void Exit() override
	{
		std::cout << "�h��I��" << std::endl;
	}
};

class Character
{
public:



private:
	// ���݂̏��
	StateBase* m_nowState = nullptr;

	// ���̏��
	StateBase* m_nextState = nullptr;
};

int main()
{
	while (true)
	{

	}

	return 0;
}