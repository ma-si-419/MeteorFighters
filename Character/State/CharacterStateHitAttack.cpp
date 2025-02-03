#include "CharacterStateHitAttack.h"
#include "CharacterStateIdle.h"
#include "CharacterStateDown.h"
#include "LocalPos.h"
#include "DxLib.h"
#include "Character.h"
#include "Attack.h"
#include "GameManagerBase.h"
#include "Effect.h"
#include "Rock.h"
#include "TutorialManager.h"
#include "SoundManager.h"

namespace
{
	const std::map<Character::HitReactionKind, float> kMoveSpeedMap =
	{
		{Character::HitReactionKind::kGuard,0.2f},
		{Character::HitReactionKind::kLow,0.2f},
		{Character::HitReactionKind::kMiddle,0.8f},
		{Character::HitReactionKind::kWeakUpBurst,1.2f},
		{Character::HitReactionKind::kUpBurst,2.0f},
		{Character::HitReactionKind::kFarBurst,2.0f},
		{Character::HitReactionKind::kDownBurst,2.0f},
		{Character::HitReactionKind::kWeakBurst,1.8f},
		{Character::HitReactionKind::kMiddleStan,0.1f},
		{Character::HitReactionKind::kBottomStan,0.1f}
	};

	//前から攻撃を受けた時の反応
	const std::map<Character::HitReactionKind, Character::AnimKind> kFrontHitReactionMap =
	{
		{Character::HitReactionKind::kGuard,Character::AnimKind::kGuardMiddle},
		{Character::HitReactionKind::kLow,Character::AnimKind::kLowHit1},
		{Character::HitReactionKind::kMiddle,Character::AnimKind::kMiddleHit},
		{Character::HitReactionKind::kWeakUpBurst,Character::AnimKind::kFrontBurst},
		{Character::HitReactionKind::kUpBurst,Character::AnimKind::kFrontBurst},
		{Character::HitReactionKind::kDownBurst,Character::AnimKind::kFrontBurst},
		{Character::HitReactionKind::kFarBurst,Character::AnimKind::kFrontBurst},
		{Character::HitReactionKind::kWeakBurst,Character::AnimKind::kFrontBurst},
		{Character::HitReactionKind::kBottomStan,Character::AnimKind::kBottomStan},
		{Character::HitReactionKind::kMiddleStan,Character::AnimKind::kFrontMiddleStan}
	};

	//後ろから攻撃を受けた時の反応
	const std::map<Character::HitReactionKind, Character::AnimKind> kBackHitReactionMap =
	{
		{Character::HitReactionKind::kGuard,Character::AnimKind::kBackLowHit1},
		{Character::HitReactionKind::kLow,Character::AnimKind::kBackLowHit1},
		{Character::HitReactionKind::kMiddle,Character::AnimKind::kBackMiddleHit},
		{Character::HitReactionKind::kWeakUpBurst,Character::AnimKind::kBackBurst},
		{Character::HitReactionKind::kUpBurst,Character::AnimKind::kBackBurst},
		{Character::HitReactionKind::kDownBurst,Character::AnimKind::kBackBurst},
		{Character::HitReactionKind::kFarBurst,Character::AnimKind::kBackBurst},
		{Character::HitReactionKind::kWeakBurst,Character::AnimKind::kBackBurst},
		{Character::HitReactionKind::kBottomStan,Character::AnimKind::kBottomStan},
		{Character::HitReactionKind::kMiddleStan,Character::AnimKind::kBackMiddleStan}
	};

	//スタン時のアニメーションをゆっくり再生する時間の割合
	constexpr float kSlowAnimTimeRate = 0.4f;
	//スタン時のアニメーションをゆっくり再生するときの再生速度
	constexpr float kSlowAnimPlaySpeed = 0.3f;

	//移動速度を減速させていくタイミング
	constexpr float kMoveSpeedDecelerationTime = 0.6f;

	//受け身がとれるようになる時間(動けない時間の割合)
	constexpr float kStopTimeRate = 0.7f;

	//吹っ飛び中にかける重力
	constexpr float kBurstGravity = 0.1f;

	//地面とぶつかると判定する移動ベクトルのyの値
	constexpr float kGroundHitY = 1.8f;

	//壁とぶつかると判定する移動ベクトルの長さ
	constexpr float kWallHitLength = 1.8f;

	//ぶつかった時のエフェクトの再生時間
	constexpr int kBumpEffectLifeTime = 90;

	//ステージとぶつかった時にエフェクトをたくようにする距離(一定距離移動しないとエフェクトをたかないようにする)
	constexpr float kStageBumpEffectPopLength = 20.0f;

	//石の種類数
	constexpr int kRockKindNum = 2;

	//ぶつかったときに出す石の数
	constexpr int kRockNum = 15;

	//受け身を取るときのアニメーションブレンドの速さ
	constexpr float kFallsBlendSpeed = 0.2f;

	//受け身を取るときのアニメーションの再生速度
	constexpr float kFallsPlaySpeed = 2.5f;

	//受け身にかかる時間
	constexpr int kFallsStopTime = 10;

	//受け身をとった時のエフェクトの再生時間
	constexpr int kFallsEffectLifeTime = 30;

	//ステージにぶつかった時のカメラを揺らす時間
	constexpr int kStageBumpShakeCameraTime = 13;

	//ステージにぶつかった時のカメラを揺らす大きさ
	constexpr int kStageBumpShakeCameraPower = 10;

	//ステージにぶつかった時受けるダメージ
	constexpr int kStageBumpDamage = 2000;
}

CharacterStateHitAttack::CharacterStateHitAttack(std::shared_ptr<Character> character) :
	CharacterStateBase(character),
	m_moveTime(0),
	m_isFrontHit(false),
	m_moveLength(0.0f),
	m_isStageBump(false),
	m_isFalls(false)
{
}

void CharacterStateHitAttack::Enter()
{
	m_kind = CharacterStateKind::kHitAttack;
	m_pNextState = shared_from_this();
}

void CharacterStateHitAttack::Update()
{
	//時間計測
	m_time++;

	//入力マネージャー
	auto input = GetCharacterInput();

	//今受けている攻撃がスタン攻撃だったら
	if (m_pCharacter->GetHitReaction() == Character::HitReactionKind::kBottomStan ||
		m_pCharacter->GetHitReaction() == Character::HitReactionKind::kMiddleStan)
	{
		int slowAnimTime = static_cast<int>(m_stopTime * kSlowAnimTimeRate);

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

	//減速させていく時間になっていたら
	if (m_time > m_stopTime * kMoveSpeedDecelerationTime)
	{
		//減速ベクトルが設定されていなければ
		if (m_decelerationVec.SqLength() < 0.01f)
		{
			//減速ベクトルを計算する
			m_decelerationVec = m_moveVec / static_cast<float>(m_stopTime * kMoveSpeedDecelerationTime);
		}

		//減速ベクトルよりも移動ベクトルの長さが大きければ
		if (m_moveVec.SqLength() > m_decelerationVec.SqLength())
		{
			//減速ベクトルを移動ベクトルに加算する
			m_moveVec += m_decelerationVec;
		}
		//減速ベクトルよりも移動ベクトルの長さが小さければ
		else
		{
			//減速ベクトルを0にする
			m_decelerationVec = MyEngine::Vector3();
		}

		//吹っ飛び中は重力をかける
		if (m_pCharacter->GetHitReaction() == Character::HitReactionKind::kUpBurst ||
			m_pCharacter->GetHitReaction() == Character::HitReactionKind::kDownBurst ||
			m_pCharacter->GetHitReaction() == Character::HitReactionKind::kFarBurst ||
			m_pCharacter->GetHitReaction() == Character::HitReactionKind::kWeakBurst ||
			m_pCharacter->GetHitReaction() == Character::HitReactionKind::kWeakUpBurst)
		{
			m_moveVec.y -= kBurstGravity;
		}
	}


	//下吹っ飛びの状態であれば基本的にエフェクトを再生する
	if (m_pCharacter->GetHitReaction() == Character::HitReactionKind::kDownBurst)
	{
		m_isStageBump = true;
	}

	//一定以上移動していたら
	if (m_moveLength > kStageBumpEffectPopLength)
	{
		//地面にぶつかった時エフェクトを再生するフラグを立てる
		m_isStageBump = true;
	}
	//それ以外であれば
	else
	{
		//地面にぶつかった時エフェクトを再生するフラグを下げる
		m_isStageBump = false;
	}

	//減速している場合
	if (m_time > m_stopTime * kMoveSpeedDecelerationTime)
	{
		//下吹っ飛び以外はエフェクトを再生しない
		if (m_pCharacter->GetHitReaction() == Character::HitReactionKind::kFarBurst ||
			m_pCharacter->GetHitReaction() == Character::HitReactionKind::kUpBurst)
		{
			m_isStageBump = false;
		}
	}

	//設定した時間たったら
	if (m_time >= static_cast<float>(m_stopTime * kStopTimeRate))
	{
		//受け身をとれるようにする
		m_isFalls = true;
	}

	//吹っ飛び状態であれば
	if (m_pCharacter->GetHitReaction() == Character::HitReactionKind::kUpBurst ||
		m_pCharacter->GetHitReaction() == Character::HitReactionKind::kDownBurst ||
		m_pCharacter->GetHitReaction() == Character::HitReactionKind::kFarBurst)
	{
		//移動ベクトルが一定以上下を向いていて地面とぶつかっていたら
		if (m_pCharacter->IsGround() &&
			m_moveVec.y < -kGroundHitY)
		{
			//次の状態をダウン状態に設定する
			std::shared_ptr<CharacterStateDown> next = std::make_shared<CharacterStateDown>(m_pCharacter);
			//ダウンの方向を設定する
			next->SetFrontHit(m_isFrontHit);
			//向いている方向を飛んでいる方向と逆方向にする
			m_pCharacter->SetFrontPos(m_pCharacter->GetPos() - m_moveVec);
			//ダウン状態に遷移する
			ChangeState(next);

			//ステージとぶつかるフラグが立っていたら
			if (m_isStageBump)
			{
				//地面にぶつかったエフェクトを再生する
				auto effect = std::make_shared<Effect>(Effect::EffectKind::kStageHit);
				//エフェクトの位置を設定する
				effect->SetPos(m_pCharacter->GetPos() + m_moveVec);
				effect->SetLifeTime(kBumpEffectLifeTime);
				m_pManager->EntryEffect(effect);

				//石の数だけループ
				for (int i = 0; i < kRockNum; i++)
				{
					//石のオブジェクト作成する
					auto rock = std::make_shared<Rock>();
					//石のモデルハンドルを設定する
					rock->SetHandle(m_pManager->GetRockModelHandle(GetRand(kRockKindNum)));
					//石の移動ベクトルを設定する
					rock->SetMoveVec(m_pCharacter->GetPos() + m_moveVec, m_pCharacter->GetPos());
					//石の初期化
					rock->Init();
					//石を登録する
					m_pManager->EntryObject(rock);
				}
				//ぶつかった時のサウンドを再生
				SoundManager::GetInstance().PlayOnceSound("StageHit");

				//ステージにぶつかった時にカメラを揺らす
				m_pManager->ShakeCamera(kStageBumpShakeCameraTime, kStageBumpShakeCameraPower);

				//ステージにぶつかった時にダメージを受ける
				m_pCharacter->SubHp(kStageBumpDamage);
			}


			return;
		}

		//移動ベクトルのXZ成分の長さ
		float moveXZLength = MyEngine::Vector3(m_moveVec.x, 0.0f, m_moveVec.z).Length();

		//壁とぶつかっていたら
		if (m_pCharacter->IsWall() &&
			moveXZLength > kWallHitLength)
		{
			//次の状態をダウン状態に設定する
			std::shared_ptr<CharacterStateDown> next = std::make_shared<CharacterStateDown>(m_pCharacter);
			//ダウンの方向を設定する
			next->SetFrontHit(m_isFrontHit);
			//向いている方向を飛んでいる方向と逆方向にする
			m_pCharacter->SetFrontPos(m_pCharacter->GetPos() - m_moveVec);
			//ダウン状態に遷移する
			ChangeState(next);

			//ステージとぶつかるフラグが立っていたら
			if (m_isStageBump)
			{
				//壁にぶつかったエフェクトを再生する
				auto effect = std::make_shared<Effect>(Effect::EffectKind::kStageHit);
				//エフェクトの位置を設定する
				effect->SetPos(m_pCharacter->GetPos());
				effect->SetLifeTime(kBumpEffectLifeTime);
				m_pManager->EntryEffect(effect);

				//石の数だけループ
				for (int i = 0; i < kRockNum; i++)
				{
					//石のオブジェクト作成する
					auto rock = std::make_shared<Rock>();
					//石のモデルハンドルを設定する
					rock->SetHandle(m_pManager->GetRockModelHandle(GetRand(kRockKindNum)));
					//石の移動ベクトルを設定する
					rock->SetMoveVec(m_pCharacter->GetPos() + m_moveVec, m_pCharacter->GetPos());
					//石の初期化
					rock->Init();
					//石を登録する
					m_pManager->EntryObject(rock);
				}

				//ぶつかった時のサウンドを再生
				SoundManager::GetInstance().PlayOnceSound("StageHit");

				//ステージにぶつかった時にカメラを揺らす
				m_pManager->ShakeCamera(kStageBumpShakeCameraTime, kStageBumpShakeCameraPower);

				//ステージにぶつかった時にダメージを受ける
				m_pCharacter->SubHp(kStageBumpDamage);
			}

			return;
		}

		//受け身がとれる状態で受け身ボタンが押されたら
		if (input->IsTrigger("B") && m_isFalls)
		{
			//次の状態をアイドル状態に設定する
			std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);

			//受け身アニメを再生する
			next->SetEndAnim(static_cast<int>(Character::AnimKind::kBottomStan), kFallsStopTime, kFallsBlendSpeed);

			//受け身エフェクトを再生する
			auto effect = std::make_shared<Effect>(Effect::EffectKind::kFalls);

			//エフェクトの位置を設定する
			effect->SetPos(m_pCharacter->GetPos() + m_moveVec * kFallsStopTime);

			//エフェクトの寿命を設定する
			effect->SetLifeTime(kFallsEffectLifeTime);

			//エフェクトを登録する
			m_pManager->EntryEffect(effect);

			//受け身チュートリアルをクリアさせる
			SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kFalls));

			m_pCharacter->SetAnimPlaySpeed(kFallsPlaySpeed);

			//アイドル状態に遷移する
			ChangeState(next);

			return;
		}
	}
	//弱吹っ飛びであれば
	else if (m_pCharacter->GetHitReaction() == Character::HitReactionKind::kWeakUpBurst ||
		m_pCharacter->GetHitReaction() == Character::HitReactionKind::kWeakBurst)
	{

		//受け身がとれる状態で受け身ボタンが押されたら
		if (input->IsTrigger("B") && m_isFalls)
		{
			//次の状態をアイドル状態に設定する
			std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);
			//受け身アニメを再生する
			next->SetEndAnim(static_cast<int>(Character::AnimKind::kFrontBurst), kFallsStopTime, kFallsBlendSpeed);
			m_pCharacter->SetAnimPlaySpeed(kFallsPlaySpeed);
			//受け身チュートリアルをクリアさせる
			SuccessTutorial(static_cast<int>(TutorialManager::TutorialSuccessKind::kFalls));
			//アイドル状態に遷移する
			ChangeState(next);
			return;
		}

		//設定した時間たったら
		if (m_time >= m_stopTime)
		{
			//次の状態をアイドル状態に設定する
			std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);
			//アイドル状態に遷移する
			ChangeState(next);

			return;
		}
	}
	//それ以外の状態であれば
	else
	{
		//設定した時間たったら
		if (m_time >= m_stopTime)
		{
			//次の状態をアイドル状態に設定する
			std::shared_ptr<CharacterStateIdle> next = std::make_shared<CharacterStateIdle>(m_pCharacter);
			//アイドル状態に遷移する
			ChangeState(next);

			return;
		}
	}

	//移動した距離を保存する
	m_moveLength += m_moveVec.Length();

	//キャラクターの移動ベクトルを設定する
	SetCharacterVelo(m_moveVec);


}

void CharacterStateHitAttack::Exit()
{
}

void CharacterStateHitAttack::HitAttack(int kind)
{
	//時間を初期化する
	m_time = 0;

	Character::HitReactionKind reaction = static_cast<Character::HitReactionKind>(kind);

	//コンボ中に何の攻撃を受けたかを保存しておく
	int hitAttackKinds[static_cast<int>(Character::HitReactionKind::kKindNum)] = {};

	for (auto item : m_hitReactions)
	{
		hitAttackKinds[item]++;
	}

	//動く方向を設定する
	MyEngine::Vector3 moveDir;
	//攻撃されたキャラクターの座標を中心としたローカル座標を作成する
	MyEngine::LocalPos local;
	local.SetCenterPos(m_pCharacter->GetPos());
	//ローカル座標の前方向を攻撃したものから攻撃されたものに向ける
	MyEngine::Vector3 centerFrontPos = (m_pCharacter->GetPos() - m_pManager->GetTargetPos(m_pCharacter)).Normalize() + m_pCharacter->GetPos();
	local.SetFrontPos(centerFrontPos);

	//コンボとやられ状態の確認

	//下段スタン攻撃
	if (reaction == Character::HitReactionKind::kBottomStan)
	{
		//二度受けていたら
		if (hitAttackKinds[static_cast<int>(kind)] > 0)
		{
			//やられ状態を変化させる
			reaction = Character::HitReactionKind::kWeakBurst;
		}
	}
	//中段スタン攻撃
	else if (reaction == Character::HitReactionKind::kMiddleStan)
	{
		//二度受けていたら
		if (hitAttackKinds[static_cast<int>(kind)] > 0)
		{
			//やられ状態を変化させる
			reaction = Character::HitReactionKind::kWeakBurst;
		}
	}


	//上方向に吹っ飛ばすもの
	if (reaction == Character::HitReactionKind::kUpBurst)
	{
		//斜め上に吹き飛ばす
		local.SetLocalPos(MyEngine::Vector3(0, 1, 1));
	}
	//下方向に吹っ飛ばすもの
	else if (reaction == Character::HitReactionKind::kDownBurst)
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

		frontPos = (m_pCharacter->GetPos() - m_pManager->GetTargetPos(m_pCharacter)) + m_pCharacter->GetPos();

		m_pCharacter->SetFrontPos(frontPos);
	}

	//アニメーションの変更
	m_pCharacter->ChangeAnim(static_cast<Character::AnimKind>(GetNextAnimKind(kind)), false);

	//前からの吹っ飛びアニメーションの場合
	if (m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kFrontBurst)
	{
		//動いている方向と逆の方向を向く
		MyEngine::Vector3 frontPos = m_pCharacter->GetPos() - m_moveVec;

		m_pCharacter->SetFrontPos(frontPos);
	}
	//後ろからの場合
	else if (m_pCharacter->GetPlayAnimKind() == Character::AnimKind::kBackBurst)
	{
		//動いている方向を向く
		MyEngine::Vector3 frontPos = m_pCharacter->GetPos() + m_moveVec;

		m_pCharacter->SetFrontPos(frontPos);
	}

	//現在のやられ状態を設定する
	m_pCharacter->SetHitReaction(reaction);

	//今まで受けた攻撃を保存しておく
	m_hitReactions.push_back(kind);
}

int CharacterStateHitAttack::GetNextAnimKind(int kind)
{

	Character::AnimKind ans = Character::AnimKind::kLowHit1;

	Character::AnimKind animKind = m_pCharacter->GetPlayAnimKind();

	Character::HitReactionKind reaction = static_cast<Character::HitReactionKind>(kind);

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
	if (ans == Character::AnimKind::kLowHit1)
	{
		//弱攻撃1を受けている状態だったら
		if (animKind == Character::AnimKind::kLowHit1)
		{
			ans = Character::AnimKind::kLowHit2;
		}
		//弱攻撃2を受けている状態だったら
		else if (animKind == Character::AnimKind::kLowHit2)
		{
			ans = Character::AnimKind::kLowHit3;
		}
	}
	else if (ans == Character::AnimKind::kBackLowHit1)
	{
		//弱攻撃1を受けている状態だったら
		if (animKind == Character::AnimKind::kLowHit1)
		{
			ans = Character::AnimKind::kLowHit2;
		}
		//弱攻撃2を受けている状態だったら
		else if (animKind == Character::AnimKind::kLowHit2)
		{
			ans = Character::AnimKind::kLowHit3;
		}
	}

	return static_cast<int>(ans);
}
