#include <iostream>


// すべてのStateの基底クラス
class StateBase
{
public:

	// 開始処理
	virtual void Enter() abstract;

	// 更新処理
	virtual void Update() abstract;

	// 終了処理
	virtual void Exit() abstract;

	// 状態を変更する時に使用する
	void ChangeState(StateBase* next)
	{
		// 終了処理を行う
		Exit();

		// 次の状態を設定する
		nextState = next;

		// 次の状態の開始処理を行う
		nextState->Enter();
	}
	// 次のStateのポインタを取得する
	StateBase* GetNextPointer() const
	{
		return nextState;
	}

private:

	// 次のStateのポインタ
	StateBase* nextState = nullptr;

};

class StateIdle : public StateBase
{

public:
	// 開始処理
	void Enter() override
	{
		std::cout << "アイドル開始" << std::endl;
	}
	// 更新処理
	void Update() override
	{
		std::cout << "アイドル更新" << std::endl;
	}
	// 終了処理
	void Exit() override
	{
		std::cout << "アイドル終了" << std::endl;
	}
};

class StateMove : public StateBase
{
public:
	// 開始処理
	void Enter() override
	{
		std::cout << "移動開始" << std::endl;
	}
	// 更新処理
	void Update() override
	{
		std::cout << "移動更新" << std::endl;
	}
	// 終了処理
	void Exit() override
	{
		std::cout << "移動終了" << std::endl;
	}
};

class StateAttack : public StateBase
{
public:
	// 開始処理
	void Enter() override
	{
		std::cout << "攻撃開始" << std::endl;
	}
	// 更新処理
	void Update() override
	{
		std::cout << "攻撃更新" << std::endl;
	}
	// 終了処理
	void Exit() override
	{
		std::cout << "攻撃終了" << std::endl;
	}
};

class StateDefense : public StateBase
{
public:
	// 開始処理
	void Enter() override
	{
		std::cout << "防御開始" << std::endl;
	}
	// 更新処理
	void Update() override
	{
		std::cout << "防御更新" << std::endl;
	}
	// 終了処理
	void Exit() override
	{
		std::cout << "防御終了" << std::endl;
	}
};

class Character
{
public:



private:
	// 現在の状態
	StateBase* m_nowState = nullptr;

	// 次の状態
	StateBase* m_nextState = nullptr;
};

int main()
{
	while (true)
	{

	}

	return 0;
}