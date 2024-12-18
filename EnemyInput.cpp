#include "EnemyInput.h"
#include "GameManager.h"
#include "CharacterStateBase.h"
#include "CharacterStateNormalAttack.h"
#include "LoadCsv.h"
#include "GameSceneConstant.h"

namespace
{
	//行動を行うまでの時間
	constexpr int kActionTime = 60;

	//チャージを行う時間
	constexpr int kChargeTime = 120;

	//中距離と判断する距離
	constexpr float kMiddleDistance = 150.0f;

	//近距離と判断する距離
	constexpr float kNearDistance = 80.0f;

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
	m_moveTime(0),
	m_stateTime(0),
	m_actionTime(0),
	m_isCountActionTime(true)
{
	m_pInputData = inputData;
	m_moveFunc = &EnemyInput::None;
	m_actionFunc = &EnemyInput::None;

	LoadCsv load;

	auto aiData = load.LoadFile("data/csv/enemyAiData.csv");

	for (auto item : aiData)
	{
		AiData pushData;

		pushData.nearRate = stoi(item[static_cast<int>(DataIndex::kNearRate)]);
		pushData.middleRate = stoi(item[static_cast<int>(DataIndex::kMiddleRate)]);
		pushData.farRate = stoi(item[static_cast<int>(DataIndex::kFarRate)]);
		pushData.minMp = stoi(item[static_cast<int>(DataIndex::kMinMp)]);

		m_aiData.push_back(pushData);
	}

}

EnemyInput::~EnemyInput()
{

}

void EnemyInput::Update()
{
	auto player = m_pManager->GetOnePlayerPointer();
	auto enemy = m_pManager->GetTwoPlayerPointer();

	//プレイヤーとの距離
	float toPlayerLength = (player->GetPos() - enemy->GetPos()).Length();
	m_moveTime--;

	//一定時間ごとに移動方向を設定する
	if (m_moveTime < 0)
	{
		m_moveTime = kMoveUpdateTime;

		//プレイヤーとの距離で移動方向を判断する

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

	//Stateがいれかわったタイミングで
	if (m_pEnemyState != m_pEnemyState->m_pNextState)
	{
		//Stateにいる時間をリセットする
		//m_stateTime = 0;
	}

	if (m_isCountActionTime)
	{
		m_actionTime++;

		m_actionFunc = &EnemyInput::None;

		//行う行動を選択するとき
		if (m_actionTime > kActionTime)
		{
			//ランダムで使用する数を計算
			int randNum = 0;

			for (auto item : m_aiData)
			{
				//近いと判断した時
				if (toPlayerLength < kNearDistance)
				{
					//気力量が足りているなら
					if (enemy->GetMp() >= item.minMp)
					{
						randNum += item.nearRate;
					}
				}
				//中距離だと判断した時
				else if (toPlayerLength < kMiddleDistance)
				{
					//気力量が足りているなら
					if (enemy->GetMp() >= item.minMp)
					{
						randNum += item.middleRate;
					}
				}
				//遠距離だと判断した時
				else
				{
					//気力量が足りているなら
					if (enemy->GetMp() >= item.minMp)
					{
						randNum += item.farRate;
					}
				}
			}

			//度の行動をするかをランダムで決定
			int actionNum = GetRand(randNum);

			Action action = Action::PhysicalAttack;

			for (auto item : m_aiData)
			{
				//近いと判断した時
				if (toPlayerLength < kNearDistance)
				{
					//気力量が足りているなら
					if (enemy->GetMp() >= item.minMp)
					{
						actionNum -= item.nearRate;
					}
				}
				//中距離だと判断した時
				else if (toPlayerLength < kMiddleDistance)
				{
					//気力量が足りているなら
					if (enemy->GetMp() >= item.minMp)
					{
						actionNum -= item.middleRate;
					}
				}
				//遠距離だと判断した時
				else
				{
					//気力量が足りているなら
					if (enemy->GetMp() >= item.minMp)
					{
						actionNum -= item.farRate;
					}
				}

				//行動が決まったら
				if (actionNum <= 0)
				{
					switch (action)
					{
					case EnemyInput::Action::PhysicalAttack:
						m_actionFunc = &EnemyInput::PhysicalAttack;
						break;
					case EnemyInput::Action::EnergyAttack:
						m_actionFunc = &EnemyInput::EnergyAttack;
						break;
					case EnemyInput::Action::Dash:
						m_actionFunc = &EnemyInput::Dash;
						break;
					case EnemyInput::Action::Rush:
						m_actionFunc = &EnemyInput::Rush;
						break;
					case EnemyInput::Action::SpecialAttack:
						m_actionFunc = &EnemyInput::SpecialAttack;
						break;
					case EnemyInput::Action::EnergyCharge:
						m_actionFunc = &EnemyInput::EnergyCharge;
						break;
					default:
						m_actionFunc = &EnemyInput::None;
						break;
					}
					//ループから抜ける
					break;
				}

				action = static_cast<Action>(static_cast<int>(action) + 1);
			}

			m_isCountActionTime = false;
		}
	}
	else
	{
		m_actionTime = 0;
	}

	//移動処理
	//(this->*m_moveFunc)();

	//アクション処理
	//(this->*m_actionFunc)();
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

	m_isCountActionTime = true;
}

void EnemyInput::Rush()
{

	m_pInputData->PushTrigger(true);

	m_pInputData->BashButton("A");

	auto player = m_pManager->GetOnePlayerPointer();
	auto enemy = m_pManager->GetTwoPlayerPointer();

	float distance = (player->GetPos() - enemy->GetPos()).Length();

	if (distance < kNearDistance)
	{
		m_isCountActionTime = true;
	}

}

void EnemyInput::SpecialAttack()
{

	m_pInputData->PushTrigger(true);

	if (m_pEnemyState->GetKind() == CharacterStateBase::CharacterStateKind::kCharge)
	{
		m_pInputData->PushButton("Y");

		m_isCountActionTime = true;
	}
}

void EnemyInput::EnergyCharge()
{
	m_stateTime++;

	m_pInputData->PushTrigger(true);

	auto player = m_pManager->GetOnePlayerPointer();
	auto enemy = m_pManager->GetTwoPlayerPointer();

	float distance = (player->GetPos() - enemy->GetPos()).Length();

	//一定時間チャージ、敵が近くに来る、MPマックスのどれかになれば終了
	if (m_stateTime > kChargeTime || distance < kNearDistance || enemy->GetMp() >= GameSceneConstant::kMaxMp)
	{
		//チャージをやめる
		m_isCountActionTime = true;
		m_stateTime = 0;
	}

}

void EnemyInput::PhysicalAttack()
{

	m_pInputData->BashButton("X");

	//攻撃Stateに入っていたら
	if (m_pEnemyState->GetKind() == CharacterStateBase::CharacterStateKind::kNormalAttack)
	{
		auto attackState = std::dynamic_pointer_cast<CharacterStateNormalAttack>(m_pEnemyState);

		//攻撃終了のフラグが立っていたら
		if (attackState->GetNowAttackName() == "Low8" || attackState->GetEndAttack())
		{
			m_isCountActionTime = true;
		}
	}
	
	auto player = m_pManager->GetOnePlayerPointer();
	auto enemy = m_pManager->GetTwoPlayerPointer();

	float distance = (player->GetPos() - enemy->GetPos()).Length();

	//もし距離が離れていたら攻撃をやめる
	if (distance > kNearDistance)
	{
		m_isCountActionTime = true;
	}
}

void EnemyInput::EnergyAttack()
{

	m_pInputData->BashButton("Y");

	if (m_pEnemyState->GetKind() == CharacterStateBase::CharacterStateKind::kNormalAttack)
	{
		auto attackState = std::dynamic_pointer_cast<CharacterStateNormalAttack>(m_pEnemyState);

		if (attackState->GetNowAttackName() == "Energy6" || attackState->GetEndAttack())
		{
			m_isCountActionTime = true;
		}
	}
}

void EnemyInput::Guard()
{
	m_pInputData->PushButton("B");
}
