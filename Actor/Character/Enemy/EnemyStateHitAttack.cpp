#include "EnemyStateHitAttack.h"
#include "EnemyStateIdle.h"
#include "LocalPos.h"
#include "DxLib.h"
#include "Enemy.h"
#include "Attack.h"

namespace
{
	constexpr int kDownTimes[static_cast<int>(EnemyStateHitAttack::HitKind::kKindNum)] =
	{
		40,
		40,
		90,
		90,
		90,
		60,
		60
	};

	constexpr float kMoveSpeed[static_cast<int>(EnemyStateHitAttack::HitKind::kKindNum)] =
	{
		0.3f,
		1.0f,
		3.0f,
		3.0f,
		3.0f,
		0.01f,
		0.01f
	};

	//前から攻撃を受けた時の反応
	const std::map<EnemyStateHitAttack::HitKind, CharacterBase::AnimKind> kFrontHitReactionMap =
	{
		{EnemyStateHitAttack::HitKind::kLow,CharacterBase::AnimKind::kLowHit1},
		{EnemyStateHitAttack::HitKind::kMiddle,CharacterBase::AnimKind::kMiddleHit},
		{EnemyStateHitAttack::HitKind::kUpBurst,CharacterBase::AnimKind::kFrontBurst},
		{EnemyStateHitAttack::HitKind::kDownBurst,CharacterBase::AnimKind::kFrontBurst},
		{EnemyStateHitAttack::HitKind::kFarBurst,CharacterBase::AnimKind::kFrontBurst},
		{EnemyStateHitAttack::HitKind::kBottomStan,CharacterBase::AnimKind::kBottomStan},
		{EnemyStateHitAttack::HitKind::kMiddleStan,CharacterBase::AnimKind::kFrontMiddleStan}
	};

	//後ろから攻撃を受けた時の反応
	const std::map<EnemyStateHitAttack::HitKind, CharacterBase::AnimKind> kBackHitReactionMap =
	{
		{EnemyStateHitAttack::HitKind::kLow,CharacterBase::AnimKind::kBackLowHit1},
		{EnemyStateHitAttack::HitKind::kMiddle,CharacterBase::AnimKind::kBackMiddleHit},
		{EnemyStateHitAttack::HitKind::kUpBurst,CharacterBase::AnimKind::kBackBurst},
		{EnemyStateHitAttack::HitKind::kDownBurst,CharacterBase::AnimKind::kBackBurst},
		{EnemyStateHitAttack::HitKind::kFarBurst,CharacterBase::AnimKind::kBackBurst},
		{EnemyStateHitAttack::HitKind::kBottomStan,CharacterBase::AnimKind::kBottomStan},
		{EnemyStateHitAttack::HitKind::kMiddleStan,CharacterBase::AnimKind::kBackMiddleStan}
	};

	//移動する時間の割合
	constexpr float kMoveTimeRate = 0.4f;

	//スタン時のアニメーションをゆっくり再生する時間の割合
	constexpr float kSlowAnimTimeRate = 0.4f;
	//スタン時のアニメーションをゆっくり再生するときの再生速度
	constexpr float kSlowAnimPlaySpeed = 0.3f;
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

	//今受けている攻撃がスタン攻撃だったら
	if (m_hitReaction == HitKind::kBottomStan ||
		m_hitReaction == HitKind::kMiddleStan)
	{
		int slowAnimTime = static_cast<int>(m_downTime * kSlowAnimTimeRate);

		if (m_time < slowAnimTime)
		{
			//再生速度をゆっくりにする
			m_pEnemy->SetAnimPlaySpeed(kSlowAnimPlaySpeed);
		}
		else
		{
			//再生速度を初期値に戻す
			m_pEnemy->SetAnimPlaySpeed();
		}
	}

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

	//前方から殴られていたらプレイヤーの方向を向く
	if (m_isFrontHit)
	{
		m_pEnemy->SetFrontPos(GetPlayerPos());
	}
	//後方から殴られていたらプレイヤーとは逆の方向を向く
	else
	{
		MyEngine::Vector3 frontPos;

		frontPos = (m_pEnemy->GetPos() - GetPlayerPos()).Normalize() + m_pEnemy->GetPos();

		m_pEnemy->SetFrontPos(frontPos);
	}

	//アニメーションの変更
	m_pEnemy->ChangeAnim(static_cast<CharacterBase::AnimKind>(GetNextAnimKind(kind)), false);

	//今まで受けた攻撃を保存しておく
	m_hitReactions.push_back(kind);
}

void EnemyStateHitAttack::OnCollide(std::shared_ptr<Collidable> collider)
{
	if (collider->GetTag() == ObjectTag::kPlayerAttack)
	{
		auto attack = std::static_pointer_cast<Attack>(collider);

		auto status = attack->GetStatus();

		EnemyStateBase::HitAttack(attack, GetKind());
		printfDx("Hit\n");
	}
}

int EnemyStateHitAttack::GetNextAnimKind(HitKind kind)
{

	CharacterBase::AnimKind ans = CharacterBase::AnimKind::kLowHit1;

	CharacterBase::AnimKind animKind = m_pEnemy->GetPlayAnimKind();

	//正面から攻撃を受けている場合
	if (m_isFrontHit)
	{
		ans = kFrontHitReactionMap.at(kind);
	}
	//後ろから攻撃を受けている場合
	else
	{
		ans = kBackHitReactionMap.at(kind);
	}

	//弱攻撃を受けた場合、順番にアニメーションをさせる
	if (ans == CharacterBase::AnimKind::kLowHit1)
	{
		//弱攻撃1を受けている状態だったら
		if (animKind == CharacterBase::AnimKind::kLowHit1)
		{
			ans = CharacterBase::AnimKind::kLowHit2;
		}
		//弱攻撃2を受けている状態だったら
		else if (animKind == CharacterBase::AnimKind::kLowHit2)
		{
			ans = CharacterBase::AnimKind::kLowHit3;
		}
	}
	else if (ans == CharacterBase::AnimKind::kBackLowHit1)
	{
		//弱攻撃1を受けている状態だったら
		if (animKind == CharacterBase::AnimKind::kLowHit1)
		{
			ans = CharacterBase::AnimKind::kLowHit2;
		}
		//弱攻撃2を受けている状態だったら
		else if (animKind == CharacterBase::AnimKind::kLowHit2)
		{
			ans = CharacterBase::AnimKind::kLowHit3;
		}
	}

	return static_cast<int>(ans);
}
