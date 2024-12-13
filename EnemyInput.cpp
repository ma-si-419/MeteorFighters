#include "EnemyInput.h"
#include "GameManager.h"

namespace
{
	//中距離と判断する距離
	constexpr float kMiddleDistance = 150.0f;

	//近距離と判断する距離
	constexpr float kNearDistance = 75.0f;

	//移動方向を更新する時間
	constexpr int kMoveUpdateTime = 120;

	//移動方向のランダムの数
	constexpr int kMoveDirRandomNum = 3;

	//近距離で選択できる移動方向
	const EnemyInput::MoveDir kNearMoveDir[kMoveDirRandomNum] =
	{
		EnemyInput::MoveDir::kBack,
		EnemyInput::MoveDir::kRight,
		EnemyInput::MoveDir::kLeft
	};

	//中距離で選択できる移動方向
	const EnemyInput::MoveDir kMiddleMoveDir[kMoveDirRandomNum] =
	{
		EnemyInput::MoveDir::kFront,
		EnemyInput::MoveDir::kRight,
		EnemyInput::MoveDir::kLeft
	};

	//遠距離で選択できる移動方向
	const EnemyInput::MoveDir kFarMoveDir[kMoveDirRandomNum] =
	{
		EnemyInput::MoveDir::kFront,
		EnemyInput::MoveDir::kRight,
		EnemyInput::MoveDir::kLeft
	};
}

EnemyInput::EnemyInput(std::shared_ptr<MyEngine::InputData> inputData) :
	m_moveTime(0)
{
	m_pInputData = inputData;
}

EnemyInput::~EnemyInput()
{

}

void EnemyInput::Update()
{
	auto player = m_pManager->GetOnePlayerPointer();
	auto enemy = m_pManager->GetTwoPlayerPointer();

	m_moveTime--;

	//一定時間ごとに移動方向を設定する
	if (m_moveTime < 0)
	{
		m_moveTime = kMoveUpdateTime;

		//プレイヤーとの距離で移動方向を判断する
		float toPlayerLength = (player->GetPos() - enemy->GetPos()).Length();

		//ランダムな数を取得する
		int rand = GetRand(kMoveDirRandomNum - 1);
		//移動方向
		MoveDir moveDir;

		//近いと判断した時
		if (toPlayerLength < kNearDistance)
		{
			moveDir = kNearMoveDir[rand];
		}
		//中距離だと判断した時
		else if (toPlayerLength < kMiddleDistance)
		{
			moveDir = kMiddleMoveDir[rand];
		}
		//遠距離だと判断した時
		else
		{
			moveDir = kFarMoveDir[rand];
		}

		if (moveDir == MoveDir::kFront)
		{
			m_moveFunc = &EnemyInput::MoveFront;
		}
		else if (moveDir == MoveDir::kBack)
		{
			m_moveFunc = &EnemyInput::MoveBack;
		}
		else if (moveDir == MoveDir::kRight)
		{
			m_moveFunc = &EnemyInput::MoveRight;
		}
		else if (moveDir == MoveDir::kLeft)
		{
			m_moveFunc = &EnemyInput::MoveLeft;
		}
	}
	//自分の状態を見て行動する
	auto enemyState = enemy->GetStateKind();





	//移動処理
	(this->*m_moveFunc)();
}

void EnemyInput::MoveFront()
{
	m_pInputData->TiltStick(MyEngine::Vector2(0, -1000), true);
}

void EnemyInput::MoveRight()
{
	m_pInputData->TiltStick(MyEngine::Vector2(1000, 0), true);
}

void EnemyInput::MoveLeft()
{
	m_pInputData->TiltStick(MyEngine::Vector2(-1000, 0), true);
}

void EnemyInput::MoveBack()
{
	m_pInputData->TiltStick(MyEngine::Vector2(0, 1000), true);
}

void EnemyInput::Dash()
{
	m_pInputData->PushButton("A");
}

void EnemyInput::PhysicalAttack()
{
	m_pInputData->BashButton("X");
}

void EnemyInput::EnergyAttack()
{
	m_pInputData->BashButton("Y");
}

void EnemyInput::Guard()
{
	m_pInputData->PushButton("B");
}
