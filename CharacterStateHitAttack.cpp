#include "CharacterStateHitAttack.h"
#include "CharacterStateIdle.h"
#include "LocalPos.h"
#include "DxLib.h"
#include "CharacterBase.h"
#include "Attack.h"

namespace
{
	const std::map<CharacterBase::HitReactionKind, int> kDownTimeMap =
	{
		{CharacterBase::HitReactionKind::kLow,40},
		{CharacterBase::HitReactionKind::kMiddle,40},
		{CharacterBase::HitReactionKind::kWeakUpBurst,60},
		{CharacterBase::HitReactionKind::kUpBurst,120},
		{CharacterBase::HitReactionKind::kFarBurst,120},
		{CharacterBase::HitReactionKind::kDownBurst,120},
		{CharacterBase::HitReactionKind::kMiddleStan,60},
		{CharacterBase::HitReactionKind::kBottomStan,60}
	};

	const std::map<CharacterBase::HitReactionKind, float> kMoveSpeedMap =
	{
		{CharacterBase::HitReactionKind::kLow,0.4f},
		{CharacterBase::HitReactionKind::kMiddle,0.8f},
		{CharacterBase::HitReactionKind::kWeakUpBurst,1.2f},
		{CharacterBase::HitReactionKind::kUpBurst,2.0f},
		{CharacterBase::HitReactionKind::kFarBurst,2.0f},
		{CharacterBase::HitReactionKind::kDownBurst,2.0f},
		{CharacterBase::HitReactionKind::kMiddleStan,0.1f},
		{CharacterBase::HitReactionKind::kBottomStan,0.1f}
	};

	//前から攻撃を受けた時の反応
	const std::map<CharacterBase::HitReactionKind, CharacterBase::AnimKind> kFrontHitReactionMap =
	{
		{CharacterBase::HitReactionKind::kLow,CharacterBase::AnimKind::kLowHit1},
		{CharacterBase::HitReactionKind::kMiddle,CharacterBase::AnimKind::kMiddleHit},
		{CharacterBase::HitReactionKind::kWeakUpBurst,CharacterBase::AnimKind::kFrontBurst},
		{CharacterBase::HitReactionKind::kUpBurst,CharacterBase::AnimKind::kFrontBurst},
		{CharacterBase::HitReactionKind::kDownBurst,CharacterBase::AnimKind::kFrontBurst},
		{CharacterBase::HitReactionKind::kFarBurst,CharacterBase::AnimKind::kFrontBurst},
		{CharacterBase::HitReactionKind::kBottomStan,CharacterBase::AnimKind::kBottomStan},
		{CharacterBase::HitReactionKind::kMiddleStan,CharacterBase::AnimKind::kFrontMiddleStan}
	};

	//後ろから攻撃を受けた時の反応
	const std::map<CharacterBase::HitReactionKind, CharacterBase::AnimKind> kBackHitReactionMap =
	{
		{CharacterBase::HitReactionKind::kLow,CharacterBase::AnimKind::kBackLowHit1},
		{CharacterBase::HitReactionKind::kMiddle,CharacterBase::AnimKind::kBackMiddleHit},
		{CharacterBase::HitReactionKind::kWeakUpBurst,CharacterBase::AnimKind::kBackBurst},
		{CharacterBase::HitReactionKind::kUpBurst,CharacterBase::AnimKind::kBackBurst},
		{CharacterBase::HitReactionKind::kDownBurst,CharacterBase::AnimKind::kBackBurst},
		{CharacterBase::HitReactionKind::kFarBurst,CharacterBase::AnimKind::kBackBurst},
		{CharacterBase::HitReactionKind::kBottomStan,CharacterBase::AnimKind::kBottomStan},
		{CharacterBase::HitReactionKind::kMiddleStan,CharacterBase::AnimKind::kBackMiddleStan}
	};

	//移動する時間の割合
	constexpr float kMoveTimeRate = 0.7f;

	//スタン時のアニメーションをゆっくり再生する時間の割合
	constexpr float kSlowAnimTimeRate = 0.4f;
	//スタン時のアニメーションをゆっくり再生するときの再生速度
	constexpr float kSlowAnimPlaySpeed = 0.3f;
}

CharacterStateHitAttack::CharacterStateHitAttack(std::shared_ptr<CharacterBase> character) :
	CharacterStateBase(character),
	m_downTime(0),
	m_moveTime(0),
	m_isFrontHit(false)
{
}

void CharacterStateHitAttack::Enter()
{
	m_kind = CharacterStateKind::kHitAttack;
	m_pNextState = shared_from_this();
}

void CharacterStateHitAttack::Update()
{

	m_time++;
	printfDx("ヒットアタック%.0f\n",m_time);

	//今受けている攻撃がスタン攻撃だったら
	if (m_pCharacter->GetHitReaction() == CharacterBase::HitReactionKind::kBottomStan ||
		m_pCharacter->GetHitReaction() == CharacterBase::HitReactionKind::kMiddleStan)
	{
		int slowAnimTime = static_cast<int>(m_downTime * kSlowAnimTimeRate);

		if (m_time < slowAnimTime)
		{
			//再生速度をゆっくりにする
			m_pCharacter->SetAnimPlaySpeed(kSlowAnimPlaySpeed);
		}
		else
		{
			//再生速度を初期値に戻す
			m_pCharacter->SetAnimPlaySpeed();
		}
	}

	//設定した時間たったら
	if (m_downTime <= m_time)
	{

		std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);

		//アイドル状態に戻る
		ChangeState(next);
	}

	//移動する時間が終わったら
	if (m_time > static_cast<int>(m_downTime * kMoveTimeRate))
	{
		m_moveVec = MyEngine::Vector3(0, 0, 0);
		//TODO : 吹っ飛び状態であれば吹っ飛び状態から通常状態に戻るアニメーションを再生する
	}

	SetCharacterVelo(m_moveVec);


#ifdef _DEBUG

	DrawString(0, 32, "PlayerState:HitAttack", GetColor(255, 255, 255));

#endif // _DEBUG
}

void CharacterStateHitAttack::Exit()
{
}

void CharacterStateHitAttack::HitAttack(int kind)
{

	CharacterBase::HitReactionKind reaction = static_cast<CharacterBase::HitReactionKind>(kind);

	//コンボ中に何の攻撃を受けたかを保存しておく
	int hitAttackKinds[static_cast<int>(CharacterBase::HitReactionKind::kKindNum)] = {};

	for (auto item : m_hitReactions)
	{
		hitAttackKinds[item]++;
	}

	//動けない時間を設定する
	m_downTime = kDownTimeMap.at(reaction);
	//現在のやられ状態を設定する
	m_pCharacter->SetHitReaction(reaction);
	//動く方向を設定する
	MyEngine::Vector3 moveDir;
	//攻撃されたキャラクターの座標を中心としたローカル座標を作成する
	MyEngine::LocalPos local;
	local.SetCenterPos(m_pCharacter->GetPos());
	//ローカル座標の前方向を攻撃したものから攻撃されたものに向ける
	MyEngine::Vector3 centerFrontPos = (m_pCharacter->GetPos() - GetTargetPos()).Normalize() + m_pCharacter->GetPos();
	local.SetFrontPos(centerFrontPos);


	//上方向に吹っ飛ばすもの
	if (reaction == CharacterBase::HitReactionKind::kUpBurst)
	{
		//斜め上に吹き飛ばす
		local.SetLocalPos(MyEngine::Vector3(0, 1, 1));
	}
	//下方向に吹っ飛ばすもの
	else if (reaction == CharacterBase::HitReactionKind::kDownBurst)
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

	moveDir = (local.GetWorldPos() - m_pCharacter->GetPos());

	//やられ状態によって移動速度を変更する
	m_moveVec = moveDir * kMoveSpeedMap.at(reaction);

	//コンボとやられ状態の確認

	//軽い吹き飛ばし攻撃
	if (reaction == CharacterBase::HitReactionKind::kMiddle)
	{
		//二度受けていたら
		if (hitAttackKinds[static_cast<int>(kind)] > 0)
		{
			//やられ状態を変化させる
		}

	}
	//下段スタン攻撃
	else if (reaction == CharacterBase::HitReactionKind::kBottomStan)
	{
		//二度受けていたら
		if (hitAttackKinds[static_cast<int>(kind)] > 0)
		{
			//やられ状態を変化させる

		}
	}
	//中段スタン攻撃
	else if (reaction == CharacterBase::HitReactionKind::kMiddleStan)
	{
		//二度受けていたら
		if (hitAttackKinds[static_cast<int>(kind)] > 0)
		{
			//やられ状態を変化させる

		}
	}

	//前方から殴られたかどうかを取得する
	m_isFrontHit = m_pCharacter->IsFrontTarget();

	//前方から殴られていたらプレイヤーの方向を向く
	if (m_isFrontHit)
	{
		m_pCharacter->LookTarget();
	}
	//後方から殴られていたらプレイヤーとは逆の方向を向く
	else
	{
		MyEngine::Vector3 frontPos;

		frontPos = (m_pCharacter->GetPos() - GetTargetPos()) + m_pCharacter->GetPos();

		m_pCharacter->SetFrontPos(frontPos);
	}

	//アニメーションの変更
	m_pCharacter->ChangeAnim(static_cast<CharacterBase::AnimKind>(GetNextAnimKind(kind)), false);

	//前からの吹っ飛びアニメーションの場合
	if (m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kFrontBurst)
	{
		//動いている方向と逆の方向を向く
		MyEngine::Vector3 frontPos = m_pCharacter->GetPos() - m_moveVec;

		m_pCharacter->SetFrontPos(frontPos);
	}
	//後ろからの場合
	else if (m_pCharacter->GetPlayAnimKind() == CharacterBase::AnimKind::kBackBurst)
	{
		//動いている方向を向く
		MyEngine::Vector3 frontPos = m_pCharacter->GetPos() + m_moveVec;

		m_pCharacter->SetFrontPos(frontPos);
	}

	//今まで受けた攻撃を保存しておく
	m_hitReactions.push_back(kind);
}

int CharacterStateHitAttack::GetNextAnimKind(int kind)
{

	CharacterBase::AnimKind ans = CharacterBase::AnimKind::kLowHit1;

	CharacterBase::AnimKind animKind = m_pCharacter->GetPlayAnimKind();

	CharacterBase::HitReactionKind reaction = static_cast<CharacterBase::HitReactionKind>(kind);

	//正面から攻撃を受けている場合
	if (m_isFrontHit)
	{
		ans = kFrontHitReactionMap.at(reaction);
	}
	//後ろから攻撃を受けている場合
	else
	{
		ans = kBackHitReactionMap.at(reaction);
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
