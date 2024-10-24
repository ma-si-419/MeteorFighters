#include "EnemyStateHitAttack.h"
#include "EnemyStateIdle.h"
#include "LocalPos.h"
#include "DxLib.h"
#include "Enemy.h"

namespace
{
	constexpr int kDownTimes[static_cast<int>(EnemyStateHitAttack::HitKind::kKindNum)] =
	{
		15,
		20,
		40,
		40,
		40,
		60,
		60
	};

	constexpr float kMoveSpeed[static_cast<int>(EnemyStateHitAttack::HitKind::kKindNum)] =
	{
		0.3f,
		1.5f,
		3.0f,
		3.0f,
		3.0f,
		0.01f,
		0.01f
	};

	//移動する時間の割合
	constexpr float kMoveTimeRate = 0.7f;
}

EnemyStateHitAttack::EnemyStateHitAttack(std::shared_ptr<Enemy> enemy) :
	EnemyStateBase(enemy),
	m_downTime(0),
	m_moveTime(0),
	m_hitReaction(HitKind::kLow),
	m_isFrontHit(false)
{
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
	if (m_downTime <= m_time)
	{

		std::shared_ptr<EnemyStateIdle> next = std::make_shared<EnemyStateIdle>(m_pEnemy);

		//アイドル状態に戻る
		ChangeState(next);
	}

	SetEnemyVelo(m_moveVec);


#ifdef _DEBUG

	DrawString(0, 32, "EnemyState:HitAttack", GetColor(255, 255, 255));

#endif // _DEBUG
}

void EnemyStateHitAttack::Exit()
{
}

void EnemyStateHitAttack::HitAttack(HitKind kind)
{

	//コンボ中に何の攻撃を受けたかを保存しておく
	int hitAttackKinds[static_cast<int>(HitKind::kKindNum)] = {};

	for (auto item : m_hitReactions)
	{
		hitAttackKinds[static_cast<int>(item)]++;
	}

	//動けない時間を設定する
	m_downTime = kDownTimes[static_cast<int>(kind)];
	//現在のやられ状態を設定する
	m_hitReaction = kind;
	//動く方向を設定する
	MyEngine::Vector3 moveDir;
	//攻撃されたキャラクターの座標を中心としたローカル座標を作成する
	LocalPos local;
	local.SetCenterPos(m_pEnemy->GetPos());
	//ローカル座標の前方向を攻撃したものから攻撃されたものに向ける
	MyEngine::Vector3 centerFrontPos = (m_pEnemy->GetPos() - GetPlayerPos()).Normalize() + m_pEnemy->GetPos();
	local.SetFrontPos(centerFrontPos);


	//上方向に吹っ飛ばすもの
	if (kind == HitKind::kUpBurst)
	{
		//斜め上に吹き飛ばす
		local.SetLocalPos(MyEngine::Vector3(0, 1, 1));
	}
	//下方向に吹っ飛ばすもの
	else if (kind == HitKind::kDownBurst)
	{
		//斜め下に吹き飛ばす
		local.SetLocalPos(MyEngine::Vector3(0, -1, 1));
	}
	//それ以外のもの
	else
	{
		//基本的に奥方向に動かす
		local.SetLocalPos(MyEngine::Vector3(0, 0, 1));
	}

	moveDir = (local.GetWorldPos() - m_pEnemy->GetPos());

	//やられ状態によって移動速度を変更する
	m_moveVec = moveDir * kMoveSpeed[static_cast<int>(kind)];

	//コンボとやられ状態の確認

	//軽い吹き飛ばし攻撃
	if (kind == HitKind::kMiddle)
	{
		//二度受けていたら
		if (hitAttackKinds[static_cast<int>(kind)] > 0)
		{
			//やられ状態を変化させる
		}

	}
	//下段スタン攻撃
	else if (kind == HitKind::kBottomStan)
	{
		//二度受けていたら
		if (hitAttackKinds[static_cast<int>(kind)] > 0)
		{
			//やられ状態を変化させる

		}
	}
	//中段スタン攻撃
	else if (kind == HitKind::kMiddleStan)
	{
		//二度受けていたら
		if (hitAttackKinds[static_cast<int>(kind)] > 0)
		{
			//やられ状態を変化させる

		}
	}

	//前方から殴られたかどうかを取得する
	m_isFrontHit = m_pEnemy->IsFrontTarget(false);

	//アニメーションの変更
	m_pEnemy->ChangeAnim(static_cast<CharacterBase::AnimKind>(GetNextAnimKind(kind)), false);

	//今まで受けた攻撃を保存しておく
	m_hitReactions.push_back(kind);
}

void EnemyStateHitAttack::OnCollide(std::shared_ptr<Collidable> collider)
{

}

int EnemyStateHitAttack::GetNextAnimKind(HitKind kind)
{

	int ans = 0;

	CharacterBase::AnimKind animKind = m_pEnemy->GetPlayAnimKind();

	//弱攻撃を受けたら順番にアニメーションを再生する
	if (kind == HitKind::kLow)
	{
		//前方から攻撃を受けた場合
		if (m_isFrontHit)
		{
			if (animKind == CharacterBase::AnimKind::kLowHit1)
			{
				ans = static_cast<int>(CharacterBase::AnimKind::kLowHit2);
			}
			else if (animKind == CharacterBase::AnimKind::kLowHit2)
			{
				ans = static_cast<int>(CharacterBase::AnimKind::kLowHit3);
			}
			else if (animKind == CharacterBase::AnimKind::kLowHit3)
			{
				ans = static_cast<int>(CharacterBase::AnimKind::kLowHit1);
			}
			else
			{
				ans = static_cast<int>(CharacterBase::AnimKind::kLowHit1);
			}
		}
		//後ろから攻撃を受けた場合
		else
		{
			if (animKind == CharacterBase::AnimKind::kBackLowHit1)
			{
				ans = static_cast<int>(CharacterBase::AnimKind::kBackLowHit2);
			}
			else if (animKind == CharacterBase::AnimKind::kBackLowHit2)
			{
				ans = static_cast<int>(CharacterBase::AnimKind::kBackLowHit3);
			}
			else if (animKind == CharacterBase::AnimKind::kBackLowHit3)
			{
				ans = static_cast<int>(CharacterBase::AnimKind::kBackLowHit1);
			}
			else
			{
				ans = static_cast<int>(CharacterBase::AnimKind::kBackLowHit1);
			}
		}
	}
	//中攻撃を受けた場合
	else if (kind == HitKind::kMiddle)
	{
		//前方から攻撃を受けた場合
		if (m_isFrontHit)
		{
			ans = static_cast<int>(CharacterBase::AnimKind::kMiddleHit);
		}
		//後方から攻撃を受けた場合
		else
		{
			ans = static_cast<int>(CharacterBase::AnimKind::kBackMiddleHit);
		}
	}
	//吹き飛ばし攻撃を受けた場合
	else if (kind == HitKind::kUpBurst ||
		kind == HitKind::kDownBurst ||
		kind == HitKind::kFarBurst)
	{
		//前方から攻撃を受けた場合
		if (m_isFrontHit)
		{
			ans = static_cast<int>(CharacterBase::AnimKind::kFrontBurst);
		}
		//後方から攻撃を受けた場合
		else
		{
			ans = static_cast<int>(CharacterBase::AnimKind::kBackBurst);
		}
	}

	return ans;
}
