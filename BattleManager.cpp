#include "BattleManager.h"
#include "Attack.h"
#include "LoadCsv.h"
#include "Stage.h"
#include "GameCamera.h"
#include "Character.h"
#include "Effectmanager.h"
#include "Effect.h"
#include "GameUi.h"
#include "Game.h"
#include <cmath>

namespace
{
	//バトル開始時の演出のカメラスタート座標(ローカル)
	const MyEngine::Vector3 kStartCameraStartPos(45.0f, 20.0f, 80.0f);

	//バトル開始時の演出のカメラエンド座標(ローカル)
	const MyEngine::Vector3 kStartCameraEndPos(15.0f, 10.0f, 15.0f);

	//開始時のカメラの移動速度
	constexpr float kStartCameraMoveSpeed = 2.0f;

	//キャラクターのアニメーションが終わってから何フレームカメラをそのままにするか
	constexpr int kStartPoseTime = 60;

	//キャラクターの演出が終わってから何フレームでバトルに移行するか
	constexpr int kBattleStartTime = 120;

	//ノックアウトした時のカメラの座標(ローカル)
	const MyEngine::Vector3 kKnockOutCameraPos(-15.0f, -5.0f, -20.0f);

	//ノックアウト時のカメラターゲットを中心からどれだけ離すか
	constexpr float kKnockOutCameraTargetDistance = 10.0f;

	//ノックアウトの演出の時間
	constexpr int kKnockOutTime = 60;

	//フェードの速度
	constexpr int kKnockOutFadeSpeed = 5;

	//リトライ時のフェードの速度
	constexpr int kRetryFadeSpeed = 25;

	//バトル終了時の演出のカメラスタート座標
	const MyEngine::Vector3 kResultCameraStartPos(0.0f, 50.0f, 50.0f);
	const MyEngine::Vector3 kResultCameraEndPos(0.0f, 8.0f, 18.0f);

	//バトル終了時のカメラの移動速度
	constexpr float kResultCameraMoveSpeed = 1.0f;
}

BattleManager::BattleManager(std::shared_ptr<GameCamera> camera) :
	GameManagerBase(camera)
{
	m_pStage = std::make_shared<Stage>();
	m_pStage->Init();
	m_pCamera = camera;

	m_pEffectManager = std::make_shared<EffectManager>();
	m_pGameUi = std::make_shared<GameUi>();

	m_updateSituationFunc = &BattleManager::UpdateStart;
}

BattleManager::~BattleManager()
{
}

void BattleManager::Init()
{
	m_pCamera->SetPoseCamera();
	m_poseCameraPos = kStartCameraStartPos;
	ChangeSituation(BattleSituation::kStart1P);
#ifdef _DEBUG
	//	m_situation = Situation::kBattle;
	//	m_updateSituationFunc = &BattleManager::UpdateBattle;
#endif // _DEBUG
}

void BattleManager::Update()
{
#ifdef _DEBUG

	MyEngine::Vector3 pos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetPos();

	DrawFormatString(0, 80, GetColor(255, 255, 255), "プレイヤーの座標(%0.1f,%0.1f,%0.1f)", pos.x, pos.y, pos.z);

	pos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetPos();

	DrawFormatString(0, 96, GetColor(255, 255, 255), "エネミーの座標(%0.1f,%0.1f,%0.1f)", pos.x, pos.y, pos.z);

#endif // _DEBUG

	//状況によって変わるUpdate
	(this->*m_updateSituationFunc)();

	//ゲームシーン共通の更新
	UpdateCommon();

}

void BattleManager::ChangeSituation(BattleSituation situation)
{
	//状況を変更
	m_situation = situation;

	//プレイヤーの状況を変更
	for (auto& player : m_pCharacters) player->ChangeSituationUpdate(static_cast<int>(situation));

	//リトライ時
	if (situation == BattleSituation::kRetry)
	{
		m_updateSituationFunc = &BattleManager::UpdateRetry;
	}
	//開始時
	else if (situation == BattleSituation::kStart1P || situation == BattleSituation::kStart2P)
	{
		m_updateSituationFunc = &BattleManager::UpdateStart;

		m_pCamera->SetPoseCamera();
		m_poseCameraPos = kStartCameraStartPos;
	}
	//バトル時
	else if (situation == BattleSituation::kBattle)
	{
		m_updateSituationFunc = &BattleManager::UpdateBattle;
	}
	//ノックアウト時
	else if (situation == BattleSituation::kKnockOut)
	{
		m_updateSituationFunc = &BattleManager::UpdateKnockOut;
	}
	//リザルト時
	else if (situation == BattleSituation::kResult)
	{
		m_updateSituationFunc = &BattleManager::UpdateResult;
	}
	else if (situation == BattleSituation::kMenu)
	{
		m_updateSituationFunc = &BattleManager::UpdateMenu;
	}

}

void BattleManager::Draw()
{
	//共通の描画処理
	DrawCommon();

	//フェードのアルファ値が0よりも高い場合表示する
	if (m_alpha > 0)
	{
		m_pGameUi->DrawFade(GetColor(255, 255, 255), m_alpha);
	}

	if (m_situation == BattleSituation::kResult)
	{
		if (m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetHp() > 0)
		{
			m_pGameUi->DrawResult(true);
		}
		else
		{
			m_pGameUi->DrawResult(false);
		}
	}
}

void BattleManager::Final()
{
	for (auto& character : m_pCharacters)
	{
		character->Final();
	}
	for (auto& attack : m_pAttacks)
	{
		attack->Final();
	}
	m_pStage->Final();
	m_pCamera->Final();
	m_pEffectManager->Final();
}

void BattleManager::UpdateStart()
{
	m_alpha -= kRetryFadeSpeed;

	m_alpha = max(m_alpha, 0);

	//フェードインが終わってアップデートを行わない
	if (m_alpha > 0) return;

	//カメラの座標設定
	MyEngine::Vector3 onePlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetPos();
	MyEngine::Vector3 twoPlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetPos();

	//1Pの演出中
	if (m_situation == BattleSituation::kStart1P)
	{
		//カメラを遠目から少しずつ寄せていく
		MyEngine::Vector3 cameraVelo = kStartCameraEndPos - m_poseCameraPos;

		//カメラの現在の座標とカメラの終点の座標の距離が一定以上あれば
		if ((kStartCameraEndPos - m_poseCameraPos).Length() > kStartCameraMoveSpeed)
		{
			cameraVelo = cameraVelo.Normalize() * kStartCameraMoveSpeed;
		}
		//一定距離よりも近ければ
		else
		{
			cameraVelo = cameraVelo.Normalize() * (kStartCameraEndPos - m_poseCameraPos).Length();
		}

		//座標を更新する
		m_poseCameraPos += cameraVelo;

		//1Pを中心に1Pをターゲットにする
		m_pCamera->SetCenterAndTarget(onePlayerPos, onePlayerPos);

		m_pCamera->SetLocalPos(m_poseCameraPos);

		m_pCamera->SetFrontPos(twoPlayerPos);
		//アニメーションが終了したか
		if (m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->IsEndAnim())
		{
			//アニメーションが終了していたらタイムを数える
			m_time++;
		}
		if (m_time > kStartPoseTime)
		{
			m_time = 0;
			//X座標だけ反転したものをカメラの開始位置にする
			MyEngine::Vector3 pos = kStartCameraStartPos;
			pos.x *= -1;
			m_poseCameraPos = pos;

			ChangeSituation(BattleSituation::kStart2P);
		}
	}
	//2Pの演出中
	else if (m_situation == BattleSituation::kStart2P)
	{
		MyEngine::Vector3 startPos = kStartCameraEndPos;
		startPos.x *= -1;

		//カメラを遠目から少しずつ寄せていく
		MyEngine::Vector3 cameraVelo = startPos - m_poseCameraPos;

		//カメラの現在の座標とカメラの終点の座標の距離が一定以上あれば
		if ((startPos - m_poseCameraPos).Length() > kStartCameraMoveSpeed)
		{
			cameraVelo = cameraVelo.Normalize() * kStartCameraMoveSpeed;
		}
		//一定距離よりも近ければ
		else
		{
			cameraVelo = cameraVelo.Normalize() * (startPos - m_poseCameraPos).Length();
		}

		//座標を更新する
		m_poseCameraPos += cameraVelo;


		//1Pを中心に1Pをターゲットにする
		m_pCamera->SetCenterAndTarget(twoPlayerPos, twoPlayerPos);

		m_pCamera->SetLocalPos(m_poseCameraPos);

		m_pCamera->SetFrontPos(onePlayerPos);
		//アニメーションが終了したか
		if (m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->IsEndAnim())
		{
			//アニメーションが終了していたらタイムを数える
			m_time++;
		}
		if (m_time > kStartPoseTime)
		{
			m_time = 0;
			ChangeSituation(BattleSituation::kBattle);
			m_pCamera->SetCenterAndTarget(onePlayerPos, twoPlayerPos);
			m_pCamera->SetFrontPos(twoPlayerPos);
			m_pCamera->SetBattleCamera();
		}
	}

}

void BattleManager::UpdateBattle()
{
	//キャラクターどちらかの体力が0になっていたらゲームを終了するフラグを立てる
	for (auto item : m_pCharacters)
	{
		if (item->GetHp() <= 0)
		{

			//カメラの設定をここで一度だけ行う

			//ローカル座標を設定する
			m_pCamera->SetLocalPos(kKnockOutCameraPos);

			//中心座標
			MyEngine::Vector3 centerPos;
			//負けたキャラクターから勝ったキャラクターへのベクトル
			MyEngine::Vector3 loserToWinnerVec;
			//カメラの正面方向とする座標
			MyEngine::Vector3 frontPos;

			int onePlayer = static_cast<int>(Character::PlayerNumber::kOnePlayer);
			int twoPlayer = static_cast<int>(Character::PlayerNumber::kTwoPlayer);

			//負けている方を中心とする
			if (m_pCharacters[onePlayer]->GetHp() <= 0)
			{
				centerPos = m_pCharacters[onePlayer]->GetPos();
				loserToWinnerVec = m_pCharacters[twoPlayer]->GetPos() - m_pCharacters[onePlayer]->GetPos();
				frontPos = m_pCharacters[twoPlayer]->GetPos();
			}
			else
			{
				centerPos = m_pCharacters[twoPlayer]->GetPos();
				loserToWinnerVec = m_pCharacters[onePlayer]->GetPos() - m_pCharacters[twoPlayer]->GetPos();
				frontPos = m_pCharacters[onePlayer]->GetPos();
			}

			//ターゲット座標
			MyEngine::Vector3 targetPos = (loserToWinnerVec.Normalize() * kKnockOutCameraTargetDistance) + centerPos;

			//中心座標とカメラのターゲット座標を設定
			m_pCamera->SetCenterAndTarget(centerPos, targetPos);

			//正面座標を設定
			m_pCamera->SetFrontPos(frontPos);

			ChangeSituation(BattleSituation::kKnockOut);
			return;
		}
	}

	//カメラの更新
	MyEngine::Vector3 onePlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetPos();
	MyEngine::Vector3 twoPlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetPos();

	MyEngine::Vector3 cameraTargetPos = twoPlayerPos;

	cameraTargetPos.y = twoPlayerPos.y;

	m_pCamera->SetCenterAndTarget(onePlayerPos, cameraTargetPos);

	//1Pから2Pへのベクトル
	MyEngine::Vector3 playerToTarget = (twoPlayerPos - onePlayerPos).Normalize();

	//カメラの正面方向を設定
	m_pCamera->SetFrontPos(onePlayerPos + playerToTarget);

	//カメラにプレイヤーのベロシティを設定する
	m_pCamera->SetPlayerVelo(m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetVelo());
}

void BattleManager::UpdateKnockOut()
{
	//エフェクトの再生速度を遅くする
	m_pEffectManager->SetEffectPlaySpeed(0.1f);
	//演出を行っている時間を計る
	m_time++;

	//一定時間演出をしたらフェードインする
	if (m_time > kKnockOutTime)
	{
		m_alpha += kKnockOutFadeSpeed;
		m_alpha = min(m_alpha, 255);

	}
	//フェードインが終わったら
	if (m_alpha >= 255)
	{
		//状況を変更
		ChangeSituation(BattleSituation::kResult);
		//時間計測をリセット
		m_time = 0;
		//カメラのアップデートを変更
		m_pCamera->SetPoseCamera();
	}
}

void BattleManager::UpdateResult()
{

	//フェードインしていく
	if (m_alpha > 0)
	{
		m_alpha -= kKnockOutFadeSpeed;
		m_pEffectManager->DeletePlayEffect();

		//カメラの座標をリザルトの初期座標に変更
		m_poseCameraPos = kResultCameraStartPos;
		//カメラに座標設定
		m_pCamera->SetLocalPos(m_poseCameraPos);
		//1Pの座標
		MyEngine::Vector3 onePlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetPos();
		//カメラの中心座標とターゲット座標を設定
		m_pCamera->SetCenterAndTarget(onePlayerPos, onePlayerPos);
		//正面座標
		MyEngine::Vector3 frontPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetFrontPos();
		//正面座標の高さをプレイヤーの高さに合わせる
		frontPos.y = onePlayerPos.y;
		//カメラの正面座標を設定
		m_pCamera->SetFrontPos(frontPos);
	}
	//フェードアウトが終わったら
	else
	{
		//カメラを寄せていく
		MyEngine::Vector3 cameraVelo = kResultCameraEndPos - m_poseCameraPos;

		//カメラの現在の座標とカメラの終点の座標の距離が一定以上あれば
		if ((kResultCameraEndPos - m_poseCameraPos).Length() > kResultCameraMoveSpeed)
		{
			cameraVelo = cameraVelo.Normalize() * kResultCameraMoveSpeed;
		}
		//一定距離よりも近ければ
		else
		{
			cameraVelo = cameraVelo.Normalize() * (kResultCameraEndPos - m_poseCameraPos).Length();
		}
		//1Pの座標
		MyEngine::Vector3 onePlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetPos();
		//正面座標
		MyEngine::Vector3 frontPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetFrontPos();
		//正面座標の高さをプレイヤーの高さに合わせる
		frontPos.y = onePlayerPos.y;
		//カメラの正面座標を設定
		m_pCamera->SetFrontPos(frontPos);
		//座標を更新する
		m_poseCameraPos += cameraVelo;

		m_pCamera->SetLocalPos(m_poseCameraPos);
	}

	int selectNum = m_pGameUi->UpdateResult();

	if (selectNum == static_cast<int>(GameUi::SelectItem::kRetry))
	{
		//リトライする
		ChangeSituation(BattleSituation::kRetry);
	}
	else if (selectNum == static_cast<int>(GameUi::SelectItem::kCharacterSelect))
	{
		m_nextScene = Game::Scene::kSelect;
	}
	else if (selectNum == static_cast<int>(GameUi::SelectItem::kMenu))
	{
		m_nextScene = Game::Scene::kMenu;
	}


}

void BattleManager::UpdateRetry()
{
	//白くフェードアウトしてすぐにもう一度フェードインする
	m_alpha += kRetryFadeSpeed;

	m_alpha = min(m_alpha, 255);

	//フェードアウトが終わったら
	if (m_alpha == 255)
	{
		//状況を変更
		ChangeSituation(BattleSituation::kStart1P);
		//初期化を行う
		RetryInit();
	}
}

void BattleManager::UpdateMenu()
{
}
