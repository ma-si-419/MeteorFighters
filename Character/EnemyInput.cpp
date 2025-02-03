#include "EnemyInput.h"
#include "GameManagerBase.h"
#include "CharacterStateBase.h"
#include "CharacterStateNormalAttack.h"
#include "CharacterStateHitAttack.h"
#include "CharacterStateDown.h"
#include "LoadCsv.h"
#include "GameSceneConstant.h"

namespace
{
	//行動を行うまでの時間
	constexpr int kActionTime = 60;

	//状況に応じた行動を行うまでの時間
	constexpr int kSituationActionTime = 60;

	//チャージを行う時間
	constexpr int kChargeTime = 120;

	//最大何フレーム攻撃を行うか
	constexpr int kMaxAttackTime = 180;

	//中距離と判断する距離
	constexpr float kMiddleDistance = 150.0f;

	//近距離と判断する距離
	constexpr float kNearDistance = 80.0f;

	//移動方向を更新する時間
	constexpr int kMoveUpdateTime = 120;

	//移動方向のランダムの数
	constexpr int kMoveDirRandomNum = 3;

	//最大何フレームガードを行うか
	constexpr int kMaxGuardTime = 180;

	//最低何フレームガードを行うか
	constexpr int kMinGuardTime = 60;

	//スーパーダッシュのアクションにはいって何フレーム目でスーパーダッシュを行うか
	constexpr int kSuperDashActionTime = 45;

	//最大何フレームスーパーダッシュを行うか
	constexpr int kMaxSuperDashTime = 120;

	//最低何フレームスーパーダッシュを行うか
	constexpr int kMinSuperDashTime = 60;

	//アイドル状態にいる時間
	constexpr int kNoneTime = 120;

	//難易度によって読み取る場所を変える
	constexpr int kLevelShiftIndex = 4;

	//ジャストガードの猶予時間
	constexpr int kJustGuardTime = 10;

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
	m_isCountActionTime(true),
	m_situationAction(SituationAction::kNone),
	m_situationActionTime(0),
	m_guardTime(0),
	m_superDashTime(0),
	m_pManager(nullptr),
	m_tutorialAction(Action::kNone)
{
	m_pInputData = inputData;
	m_moveFunc = &EnemyInput::None;
	m_actionFunc = &EnemyInput::None;

	LoadCsv load;

	m_loadActionAiData = load.LoadFile("data/csv/enemyActionAiData.csv");

	//AIの難易度を低いに設定しておく
	m_aiLevel = AiLevel::kEasy;

	//AIのデータを読み込む
	for (auto data : m_loadActionAiData)
	{
		AiData aiData;
		aiData.nearRate = std::stoi(data[static_cast<int>(DataIndex::kNearRate)]);
		aiData.middleRate = std::stoi(data[static_cast<int>(DataIndex::kMiddleRate)]);
		aiData.farRate = std::stoi(data[static_cast<int>(DataIndex::kFarRate)]);
		aiData.minMp = std::stoi(data[static_cast<int>(DataIndex::kMinMp)]);
		m_aiData.push_back(aiData);
	}

	//状況に応じた行動の確率を設定する
	auto situationActionData = load.LoadFile("data/csv/enemySituationAiData.csv");

	for (auto data : situationActionData)
	{
		for (auto item : data)
		{
			if (item == "受け身")
			{
				m_situationActionRate[static_cast<int>(SituationAction::kFalls)][static_cast<int>(AiLevel::kEasy)] = std::stoi(data[1]);
				m_situationActionRate[static_cast<int>(SituationAction::kFalls)][static_cast<int>(AiLevel::kNormal)] = std::stoi(data[2]);
				m_situationActionRate[static_cast<int>(SituationAction::kFalls)][static_cast<int>(AiLevel::kHard)] = std::stoi(data[3]);
			}
			else if (item == "復帰")
			{
				m_situationActionRate[static_cast<int>(SituationAction::kReturn)][static_cast<int>(AiLevel::kEasy)] = std::stoi(data[1]);
				m_situationActionRate[static_cast<int>(SituationAction::kReturn)][static_cast<int>(AiLevel::kNormal)] = std::stoi(data[2]);
				m_situationActionRate[static_cast<int>(SituationAction::kReturn)][static_cast<int>(AiLevel::kHard)] = std::stoi(data[3]);
			}
			else if (item == "ガード")
			{
				m_situationActionRate[static_cast<int>(SituationAction::kGuard)][static_cast<int>(AiLevel::kEasy)] = std::stoi(data[1]);
				m_situationActionRate[static_cast<int>(SituationAction::kGuard)][static_cast<int>(AiLevel::kNormal)] = std::stoi(data[2]);
				m_situationActionRate[static_cast<int>(SituationAction::kGuard)][static_cast<int>(AiLevel::kHard)] = std::stoi(data[3]);
			}
			else if (item == "ジャストガード")
			{
				m_situationActionRate[static_cast<int>(SituationAction::kJustGuard)][static_cast<int>(AiLevel::kEasy)] = std::stoi(data[1]);
				m_situationActionRate[static_cast<int>(SituationAction::kJustGuard)][static_cast<int>(AiLevel::kNormal)] = std::stoi(data[2]);
				m_situationActionRate[static_cast<int>(SituationAction::kJustGuard)][static_cast<int>(AiLevel::kHard)] = std::stoi(data[3]);
			}
		}
	}
	int a = 0;
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

			Action action = Action::kPhysicalAttack;

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
					//チュートリアルの時は行動を変更しない
					if (m_pManager->GetGameKind() == GameManagerBase::GameKind::kTutorial)
					{
						action = m_tutorialAction;
					}

					m_stateTime = 0;

					//		action = Action::kSpecialAttack;

							//行動を変更する
					SetAction(action);

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

	//状況に応じたアクションを行うまでの時間をカウント
	m_situationActionTime++;

	//状況に応じたアクション
	SituationAction situationAction = SituationAction::kNone;

	//状況に応じたアクションを行うまでの時間が一定時間経過したら
	if (m_situationActionTime > kSituationActionTime)
	{
		//状況に応じたアクションを取得
		situationAction = GetSituationAction();

	}

	//状況に応じたアクションが設定されていたら
	if (situationAction != SituationAction::kNone)
	{
		//確率を見て状況に応じたアクションを行うかを決定
		int randNum = GetRand(100);

		//状況に応じたアクションを行う確率
		int rate = m_situationActionRate[static_cast<int>(situationAction)][static_cast<int>(m_aiLevel)];

		//確率で状況に応じたアクションを行うかを決定
		if (randNum < rate)
		{
			m_situationAction = situationAction;
		}
		else
		{
			m_situationActionTime = 0;
		}
	}

	if (m_pManager->GetGameKind() != GameManagerBase::GameKind::kTutorial)
	{
		//移動処理
		(this->*m_moveFunc)();
	}
	//状況に応じたアクションが設定されてないのなら
	if (m_situationAction == SituationAction::kNone)
	{
		//アクション処理
		(this->*m_actionFunc)();
	}
	else
	{
		//状況に応じたアクション処理
		switch (m_situationAction)
		{
		case SituationAction::kFalls:
			Falls();
			break;
		case SituationAction::kReturn:
			Return();
			break;
		case SituationAction::kGuard:
			Guard();
			break;
		case SituationAction::kJustGuard:
			JustGuard();
			break;
		}
	}

}

void EnemyInput::SetAiLevel(AiLevel level)
{
	m_aiData.clear();

	//難易度によって読み取る場所を変える
	int shiftIndex = static_cast<int>(level) * kLevelShiftIndex;

	//AIデータを読み込む
	for (auto item : m_loadActionAiData)
	{
		AiData pushData;

		//近距離の確率
		pushData.nearRate = stoi(item[static_cast<int>(DataIndex::kNearRate) + shiftIndex]);

		//中距離の確率
		pushData.middleRate = stoi(item[static_cast<int>(DataIndex::kMiddleRate) + shiftIndex]);

		//遠距離の確率
		pushData.farRate = stoi(item[static_cast<int>(DataIndex::kFarRate) + shiftIndex]);

		//最低気力量
		pushData.minMp = stoi(item[static_cast<int>(DataIndex::kMinMp) + shiftIndex]);

		//AIデータを保存
		m_aiData.push_back(pushData);
	}
}

EnemyInput::SituationAction EnemyInput::GetSituationAction()
{
	//自身の状況がふっとんでいるなら
	if (m_pEnemyState->GetKind() == CharacterStateBase::CharacterStateKind::kHitAttack)
	{
		//ヒットアタックのステートに変換
		auto hitState = std::dynamic_pointer_cast<CharacterStateHitAttack>(m_pEnemyState);

		//受け身がとれる状況なら
		if (hitState->IsFalls())
		{
			//受け身アクション
			return SituationAction::kFalls;
		}
	}

	//自身の状態がダウン状態なら
	if (m_pEnemyState->GetKind() == CharacterStateBase::CharacterStateKind::kDown)
	{
		//ダウン状態のステートに変換
		auto downState = std::dynamic_pointer_cast<CharacterStateDown>(m_pEnemyState);

		//復帰できる状況なら
		if (downState->IsReturn())
		{
			//復帰アクション
			return SituationAction::kReturn;
		}
	}

	//プレイヤーの状態が攻撃状態なら
	if (m_pPlayerState->GetKind() == CharacterStateBase::CharacterStateKind::kNormalAttack)
	{
		//攻撃状態のステートに変換
		auto attackState = std::dynamic_pointer_cast<CharacterStateNormalAttack>(m_pPlayerState);

		//攻撃までの時間を取得
		int attackTime = attackState->GetTimeToAttack();

		//攻撃までの時間が-1でないなら
		if (attackTime != -1)
		{
			//ジャストガードの猶予時間内なら
			if (attackTime < kJustGuardTime)
			{
				//ジャストガードアクション
				return SituationAction::kJustGuard;
			}
			//それ以外なら
			else
			{
				//ガードアクション
				return SituationAction::kGuard;
			}
		}
	}

	//それ以外なら
	return SituationAction::kNone;
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
#ifdef _DEBUG

	printfDx("ダッシュ\n");

#endif // _DEBUG


}

void EnemyInput::SuperDash()
{
	//スーパーダッシュのアクションに入って何フレーム目か計測
	m_stateTime++;

	//まだスーパーダッシュのアクションに入っていないなら
	if (m_pEnemyState->GetKind() != CharacterStateBase::CharacterStateKind::kRush)
	{
		m_pInputData->PushTrigger(true);

		if (m_stateTime > kSuperDashActionTime)
		{
			m_pInputData->PushButton("A");
		}
	}

	//一定時間以上スーパーダッシュを行っていたら
	if (m_stateTime > m_superDashTime)
	{
		m_isCountActionTime = true;
	}

#ifdef _DEBUG

	printfDx("スーパーダッシュ\n");

#endif // _DEBUG


}

void EnemyInput::RocketDash()
{
	//ロケットダッシュを行う
	m_pInputData->PushTrigger(true);
	m_pInputData->BashButton("A");

	//プレイヤーとの距離
	auto player = m_pManager->GetOnePlayerPointer();
	auto enemy = m_pManager->GetTwoPlayerPointer();

	float distance = (player->GetPos() - enemy->GetPos()).Length();

	//敵の近くに近づいたら
	if (distance < kNearDistance)
	{
		m_isCountActionTime = true;
	}

#ifdef _DEBUG

	printfDx("ロケットダッシュ\n");

#endif // _DEBUG

}

void EnemyInput::SpecialAttack()
{

	m_pInputData->PushTrigger(true);

	if (m_pEnemyState->GetKind() == CharacterStateBase::CharacterStateKind::kCharge)
	{
		m_pInputData->PushButton("Y");

		m_isCountActionTime = true;
	}

#ifdef _DEBUG

	printfDx("必殺技\n");

#endif // _DEBUG
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
	}

#ifdef _DEBUG

	printfDx("気力チャージ\n");

#endif // _DEBUG

}

void EnemyInput::PhysicalAttack()
{
	m_stateTime++;

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

	//最大時間よりも長く攻撃を行っていたら
	if (m_stateTime > kMaxAttackTime)
	{
		m_isCountActionTime = true;
	}

#ifdef _DEBUG

	printfDx("格闘攻撃\n");

#endif // _DEBUG

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

#ifdef _DEBUG

	printfDx("気弾攻撃\n");

#endif // _DEBUG
}

void EnemyInput::Guard()
{
	m_stateTime++;

	m_pInputData->PushButton("B");

	//ガードをやめる
	if (m_stateTime > m_guardTime)
	{
		m_isCountActionTime = true;
		m_situationAction = SituationAction::kNone;
	}
#ifdef _DEBUG

	printfDx("ガード\n");

#endif // _DEBUG
}

void EnemyInput::JustGuard()
{
	//敵の攻撃をジャストガードする
	if (m_pPlayerState->GetKind() == CharacterStateBase::CharacterStateKind::kNormalAttack)
	{
		auto attackState = std::dynamic_pointer_cast<CharacterStateNormalAttack>(m_pPlayerState);
		//攻撃までの時間を取得
		int attackTime = attackState->GetTimeToAttack();

		//ジャストガードの猶予時間内なら
		if (attackTime < kJustGuardTime)
		{
			m_pInputData->PushButton("B");
		}
	}
	//敵のステートが攻撃状態でないなら
	else
	{
		//ジャストガードをやめる
		m_situationAction = SituationAction::kNone;
	}
}

void EnemyInput::Return()
{
	//前のステートが違うなら
	if (m_lastStateKind != static_cast<int>(m_pEnemyState->GetKind()))
	{
		//復帰アクションをやめる
		m_situationAction = SituationAction::kNone;
	}
	//復帰アクションを行う
	m_pInputData->BashButton("B");
	m_lastStateKind = static_cast<int>(m_pEnemyState->GetKind());
}

void EnemyInput::Falls()
{
	//前のステートが違うなら
	if (m_lastStateKind != static_cast<int>(m_pEnemyState->GetKind()))
	{
		//復帰アクションをやめる
		m_situationAction = SituationAction::kNone;
	}
	//受け身アクションを行う
	m_pInputData->BashButton("B");

	m_lastStateKind = static_cast<int>(m_pEnemyState->GetKind());
}

void EnemyInput::UpChargeAttack()
{
	//時間計測
	m_stateTime++;

	m_pInputData->TiltStick(MyEngine::Vector2(0, -1000), true);

	m_pInputData->PushButton("X");

	//アッパー攻撃を行っていたら
	if (m_pEnemyState->GetKind() == CharacterStateBase::CharacterStateKind::kNormalAttack)
	{
		auto attackState = std::dynamic_pointer_cast<CharacterStateNormalAttack>(m_pEnemyState);
		if (attackState->GetNowAttackName() == "UpCharge" || attackState->GetEndAttack())
		{
			//m_isCountActionTime = true;
		}
	}

	//一定時間以上攻撃を行っていたら
	if (m_stateTime > kMaxAttackTime)
	{
		m_isCountActionTime = true;
	}

#ifdef _DEBUG

	printfDx("上ため攻撃\n");

#endif // _DEBUG
}

void EnemyInput::MiddleChargeAttack()
{
	m_stateTime++;

	m_pInputData->TiltStick(MyEngine::Vector2(0, 0), true);
	m_pInputData->PushButton("X");
	//ミドル攻撃を行っていたら
	if (m_pEnemyState->GetKind() == CharacterStateBase::CharacterStateKind::kNormalAttack)
	{
		auto attackState = std::dynamic_pointer_cast<CharacterStateNormalAttack>(m_pEnemyState);
		if (attackState->GetNowAttackName() == "MiddleCharge" || attackState->GetEndAttack())
		{
			m_isCountActionTime = true;
		}
	}

	//一定時間以上攻撃を行っていたら
	if (m_stateTime > kMaxAttackTime)
	{
		m_isCountActionTime = true;
	}

#ifdef _DEBUG

	printfDx("中ため攻撃\n");

#endif // _DEBUG
}

void EnemyInput::DownChargeAttack()
{
	m_stateTime++;

	m_pInputData->TiltStick(MyEngine::Vector2(0, 1000), true);
	m_pInputData->PushButton("X");
	//ダウン攻撃を行っていたら
	if (m_pEnemyState->GetKind() == CharacterStateBase::CharacterStateKind::kNormalAttack)
	{
		auto attackState = std::dynamic_pointer_cast<CharacterStateNormalAttack>(m_pEnemyState);
		if (attackState->GetNowAttackName() == "DownCharge" || attackState->GetEndAttack())
		{
			m_isCountActionTime = true;
		}
	}

	//一定時間以上攻撃を行っていたら
	if (m_stateTime > kMaxAttackTime)
	{
		m_isCountActionTime = true;
	}

#ifdef _DEBUG

	printfDx("下ため攻撃\n");

#endif // _DEBUG
}

void EnemyInput::None()
{
	m_isCountActionTime = true;

#ifdef _DEBUG
	printfDx("何もしない\n");
#endif // _DEBUG
}


void EnemyInput::SetAction(Action action)
{
	switch (action)
	{
		//格闘攻撃
	case EnemyInput::Action::kPhysicalAttack:
		m_actionFunc = &EnemyInput::PhysicalAttack;
		break;

		//エネルギー攻撃
	case EnemyInput::Action::kEnergyAttack:
		m_actionFunc = &EnemyInput::EnergyAttack;
		break;

		//ダッシュ
	case EnemyInput::Action::kDash:
		m_actionFunc = &EnemyInput::Dash;
		break;

		//ラッシュ
	case EnemyInput::Action::kSuperDash:
		//スーパーダッシュの時間設定
		m_superDashTime = GetRand(kMaxSuperDashTime - kMinSuperDashTime) + kMinSuperDashTime;

		//スーパーダッシュを始めるまでの時間を加算
		m_superDashTime += kSuperDashActionTime;

		m_actionFunc = &EnemyInput::SuperDash;
		break;

		//必殺技
	case EnemyInput::Action::kSpecialAttack:
		m_actionFunc = &EnemyInput::SpecialAttack;
		break;

		//エネルギーチャージ
	case EnemyInput::Action::kEnergyCharge:
		m_actionFunc = &EnemyInput::EnergyCharge;
		break;

		//ガード
	case EnemyInput::Action::kGuard:
		m_actionFunc = &EnemyInput::Guard;
		//ガードを行う時間を設定
		m_guardTime = GetRand(kMaxGuardTime - kMinGuardTime) + kMinGuardTime;
		break;

		//何もしない
	case EnemyInput::Action::kNone:
		m_actionFunc = &EnemyInput::None;
		break;

		//中段チャージ攻撃
	case EnemyInput::Action::kMiddleChargeAttack:
		m_actionFunc = &EnemyInput::MiddleChargeAttack;
		break;

		//下チャージ攻撃
	case EnemyInput::Action::kDownChargeAttack:
		m_actionFunc = &EnemyInput::DownChargeAttack;
		break;

		//上チャージ攻撃
	case EnemyInput::Action::kUpChargeAttack:
		m_actionFunc = &EnemyInput::UpChargeAttack;
		break;
		//ロケットダッシュ
	case EnemyInput::Action::kRocketDash:
		m_actionFunc = &EnemyInput::RocketDash;
		break;
		//それ以外
	default:
		m_actionFunc = &EnemyInput::None;
		break;
	}
}