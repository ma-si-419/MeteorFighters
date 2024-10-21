#include "EnemyStateHitAttack.h"
#include "EnemyStateIdle.h"
#include "LocalPos.h"
#include "DxLib.h"
#include "Enemy.h"

namespace
{
	constexpr int kDownTimes[static_cast<int>(EnemyStateHitAttack::HitReaction::kReactionNum)] =
	{
		40,
		40,
		50,
		60,
		180,
		180,
		180
	};
}

void EnemyStateHitAttack::HitAttack(HitReaction reaction, BurstPower power, HitDirection direction)
{

	int reactions[static_cast<int>(HitReaction::kReactionNum)] = {};

	for (auto item : m_hitReactions)
	{
		reactions[static_cast<int>(item)]++;
	}


	//ダウンタイムを設定する
	m_downTime = kDownTimes[static_cast<int>(reaction)];
	//現在のやられ状態を設定する
	m_hitReaction = reaction;
	//動く方向を設定する
	MyEngine::Vector3 moveDir;
	//攻撃した相手の座標を中心としたローカル座標を作成する
	LocalPos local;
	local.SetCenterPos(GetPlayerPos());
	MyEngine::Vector3 centerFrontPos = m_pEnemy->GetPos() - GetPlayerPos() + GetPlayerPos();
	local.SetFrontPos(centerFrontPos);

	if (direction == HitDirection::kUp)
	{
		

	}
	else if (direction == HitDirection::kDown)
	{

	}
	else if (direction == HitDirection::kFar)
	{

	}

	//コンボとやられ状態の確認
	if (reaction == HitReaction::kLow)
	{

	}
	else if (reaction == HitReaction::kMid)
	{

	}
	else if (reaction == HitReaction::kHigh)
	{

	}
	else if (reaction == HitReaction::kBurst)
	{

	}
	else if (reaction == HitReaction::kBottomStan)
	{
		//同じスタン攻撃を二度受けていたら
		if (reactions[static_cast<int>(reaction)] > 0)
		{
			//やられ状態を変化させる
			
		}
	}
	else if (reaction == HitReaction::kMiddleStan)
	{
		//同じスタン攻撃を二度受けていたら
		if (reactions[static_cast<int>(reaction)] > 0)
		{
			//やられ状態を変化させる

		}
	}
	else if (reaction == HitReaction::kTopStan)
	{
		//同じスタン攻撃を二度受けていたら
		if (reactions[static_cast<int>(reaction)] > 0)
		{
			//やられ状態を変化させる

		}
	}

	//今まで受けた攻撃を保存しておく
	m_hitReactions.push_back(reaction);
}

void EnemyStateHitAttack::Enter()
{
	m_kind = CharacterStateKind::kHitAttack;
	m_pNextState = shared_from_this();
}

void EnemyStateHitAttack::Update()
{

	m_time++;
	//設定した時間たったら
	if (m_downTime = m_time)
	{

		std::shared_ptr<EnemyStateIdle> next = std::make_shared<EnemyStateIdle>();

		//アイドル状態に戻る
		ChangeState(next);
	}


#ifdef _DEBUG

	DrawString(0, 32, "EnemyState:HitAttack", GetColor(255, 255, 255));

#endif // _DEBUG
}

void EnemyStateHitAttack::Exit()
{
}

void EnemyStateHitAttack::OnCollide(std::shared_ptr<Collidable> collider)
{
}