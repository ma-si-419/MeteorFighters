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
	m_time(0),
	m_alpha(0),
	m_nextScene(Game::Scene::kGame)
{
	m_pStage = std::make_shared<Stage>();
	m_pStage->Init();
	m_pCamera = camera;

	m_pEffectManager = std::make_shared<EffectManager>();
	m_pGameUi = std::make_shared<GameUi>();
}

BattleManager::~BattleManager()
{
}

void BattleManager::Init()
{
	m_pCamera->SetPoseCamera();
	m_poseCameraPos = kStartCameraStartPos;
	ChangeSituation(Situation::kStart1P);
#ifdef _DEBUG
	//	m_situation = Situation::kBattle;
	//	m_updateSituationFunc = &BattleManager::UpdateBattle;
#endif // _DEBUG
}

void BattleManager::RetryInit()
{
	m_pCharacters[0]->RetryInit();
	m_pCharacters[1]->RetryInit();

	m_pCamera->SetPoseCamera();
	m_poseCameraPos = kStartCameraStartPos;

	//カメラの座標設定
	MyEngine::Vector3 onePlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetPos();
	MyEngine::Vector3 twoPlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetPos();
	//1Pを中心に1Pをターゲットにする
	m_pCamera->SetCenterAndTarget(onePlayerPos, onePlayerPos);

	m_pCamera->SetLocalPos(m_poseCameraPos);

	m_pCamera->SetFrontPos(twoPlayerPos);

	m_pGameUi->RetryInit();
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


	//カメラの更新
	m_pCamera->Update();

	//攻撃クラスの更新を行う
	for (auto& item : m_pAttacks)
	{
		//攻撃のUpdate
		item->Update();
	}

	//攻撃の削除
	auto iterator = std::remove_if(m_pAttacks.begin(), m_pAttacks.end(),
		[](const auto& item)
		{
			if (!item->IsExist())
			{
				item->Final();

				return true;
			}

			return false;
		});
	m_pAttacks.erase(iterator, m_pAttacks.end());

	//エフェクトの更新
	m_pEffectManager->Update();

}

void BattleManager::ChangeSituation(Situation situation)
{
	//状況を変更
	m_situation = situation;

	//プレイヤーの状況を変更
	for (auto& player : m_pCharacters) player->ChangeSituationUpdate(static_cast<int>(situation));

	//リトライ時
	if (situation == Situation::kRetry)
	{
		m_updateSituationFunc = &BattleManager::UpdateRetry;
	}
	//開始時
	else if (situation == Situation::kStart1P || situation == Situation::kStart2P)
	{
		m_updateSituationFunc = &BattleManager::UpdateStart;
	}
	//バトル時
	else if (situation == Situation::kBattle)
	{
		m_updateSituationFunc = &BattleManager::UpdateBattle;
	}
	//ノックアウト時
	else if (situation == Situation::kKnockOut)
	{
		m_updateSituationFunc = &BattleManager::UpdateKnockOut;
	}
	//リザルト時
	else if (situation == Situation::kResult)
	{
		m_updateSituationFunc = &BattleManager::UpdateResult;
	}

}

void BattleManager::Draw()
{
	for (auto& item : m_pAttacks)
	{
		//攻撃の描画
		item->Draw();
	}

	//ステージの描画
	m_pStage->Draw();

	//エフェクトの描画
	m_pEffectManager->Draw();

	//1Pの体力を描画する
	m_pGameUi->DrawHpBar(m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetHp(), true);
	//2Pの体力を描画する
	m_pGameUi->DrawHpBar(m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetHp(), false);
	//1Pの気力を描画する
	m_pGameUi->DrawMpBar(m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetMp(), true);
	//2Pの気力を描画する
	m_pGameUi->DrawMpBar(m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetMp(), false);

	//フェードのアルファ値が0よりも高い場合表示する
	if (m_alpha > 0)
	{
		m_pGameUi->DrawFade(GetColor(255, 255, 255), m_alpha);
	}

	if (m_situation == Situation::kResult)
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

void BattleManager::SetOnePlayerStatus(int number, std::vector<std::string> statusData)
{
	//プレイヤー作成
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)] = std::make_shared<Character>(ObjectTag::kOnePlayer, static_cast<Character::CharacterKind>(number));
	//プレイヤーに自分のポインターを渡しておく
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->SetGameManager(shared_from_this());

	//ステータスを取得
	Character::CharacterStatus status = GetCharacterStatus(statusData);

	//プレイヤーのステータスを設定する
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->SetStatus(status);
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->SetCharacterNumber(Character::PlayerNumber::kOnePlayer);

}

void BattleManager::SetTwoPlayerStatus(int number, std::vector<std::string> statusData)
{
	//エネミー作成
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)] = std::make_shared<Character>(ObjectTag::kTwoPlayer, static_cast<Character::CharacterKind>(number));
	//エネミーに自分のポインターを渡しておく
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->SetGameManager(shared_from_this());

	//ステータスを取得
	Character::CharacterStatus status = GetCharacterStatus(statusData);

	//エネミーのステータスを設定する
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->SetStatus(status);
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->SetCharacterNumber(Character::PlayerNumber::kTwoPlayer);
}

MyEngine::Vector3 BattleManager::GetTargetPos(std::shared_ptr<Character> character)
{
	if (character == m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)])
	{
		return m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetPos();
	}
	else if (character == m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)])
	{
		return m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetPos();
	}
	else
	{
		return MyEngine::Vector3(0, 0, 0);
	}
}

MyEngine::Vector3 BattleManager::GetTargetVelo(std::shared_ptr<Character> character)
{
	if (character == m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)])
	{
		return m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetVelo();
	}
	else if (character == m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)])
	{
		return m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetVelo();
	}
	else
	{
		return MyEngine::Vector3(0, 0, 0);
	}
}

Character::HitReactionKind BattleManager::GetTargetHitReaction(std::shared_ptr<Character> character)
{
	if (character == m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)])
	{
		return m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetHitReaction();
	}
	else if (character == m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)])
	{
		return m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetHitReaction();
	}
	else
	{
		return Character::HitReactionKind::kNone;
	}
}

void BattleManager::AddAttack(std::shared_ptr<Attack> attack)
{
	m_pAttacks.push_back(attack);
}

void BattleManager::StartFastCameraMove()
{
	m_pCamera->StartFastCameraMove();
}

void BattleManager::StopCameraCorrection()
{
	m_pCamera->StopCamera();
}

void BattleManager::StopCamera(int time)
{
	m_pCamera->StopCamera(time);
}

void BattleManager::StartCameraCorrection()
{
	m_pCamera->StartMoveCamera();
}

void BattleManager::ShakeCamera(int time)
{
	m_pCamera->ShakeCamera(time);
}

void BattleManager::SwayCamera()
{
	m_pCamera->SwayCamera();
}

void BattleManager::EntryEffect(std::shared_ptr<Effect> effect)
{
	m_pEffectManager->Entry(effect, effect->GetPos());
}

void BattleManager::ExitEffect(std::shared_ptr<Effect> effect)
{
	m_pEffectManager->Exit(effect);
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
	if (m_situation == Situation::kStart1P)
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

			ChangeSituation(Situation::kStart2P);
		}
	}
	//2Pの演出中
	else if (m_situation == Situation::kStart2P)
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
			ChangeSituation(Situation::kBattle);
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

			ChangeSituation(Situation::kKnockOut);
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
		ChangeSituation(Situation::kResult);
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
		ChangeSituation(Situation::kRetry);
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
		ChangeSituation(Situation::kStart1P);
		//初期化を行う
		RetryInit();
	}
}

MyEngine::Vector3 BattleManager::GetTargetBackPos(float distance, std::shared_ptr<Character> character)
{
	if (character == m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)])
	{
		return m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetBackPos(distance);
	}
	else if (character == m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)])
	{
		return m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetBackPos(distance);
	}
	else
	{
		return MyEngine::Vector3(0, 0, 0);
	}
}

Character::CharacterStatus BattleManager::GetCharacterStatus(std::vector<std::string> statusData)
{
	Character::CharacterStatus ans;

	ans.name = statusData[static_cast<int>(Character::CharacterStatusDataSort::kName)];
	ans.hp = stof(statusData[static_cast<int>(Character::CharacterStatusDataSort::kHp)]);
	ans.startMp = stof(statusData[static_cast<int>(Character::CharacterStatusDataSort::kStartMp)]);
	ans.atk = stof(statusData[static_cast<int>(Character::CharacterStatusDataSort::kAtk)]);
	ans.def = stof(statusData[static_cast<int>(Character::CharacterStatusDataSort::kDef)]);
	ans.spd = stof(statusData[static_cast<int>(Character::CharacterStatusDataSort::kSpd)]);
	ans.chargeSpd = stof(statusData[static_cast<int>(Character::CharacterStatusDataSort::kChargeSpd)]);
	ans.chaseAttackNum = stoi(statusData[static_cast<int>(Character::CharacterStatusDataSort::kChaseNum)]);

	//一つ目の必殺技の情報設定
	ans.firstSpecialAttackData.name = statusData[static_cast<int>(Character::CharacterStatusDataSort::kFirstSpecialName)];
	ans.firstSpecialAttackData.path = statusData[static_cast<int>(Character::CharacterStatusDataSort::kFirstSpecialEffectName)];
	ans.firstSpecialAttackData.cost = stoi(statusData[static_cast<int>(Character::CharacterStatusDataSort::kFirstSpecialCost)]);
	ans.firstSpecialAttackData.damageRate = stof(statusData[static_cast<int>(Character::CharacterStatusDataSort::kFirstSpecialDamageRate)]);
	ans.firstSpecialAttackData.radius = stof(statusData[static_cast<int>(Character::CharacterStatusDataSort::kFirstSpecialRadius)]);
	ans.firstSpecialAttackData.startFrame = stoi(statusData[static_cast<int>(Character::CharacterStatusDataSort::kFirstSpecialStartFrame)]);
	ans.firstSpecialAttackData.endFrame = stoi(statusData[static_cast<int>(Character::CharacterStatusDataSort::kFirstSpecialEndFrame)]);
	ans.firstSpecialAttackData.kind = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetSpecialAttackKind(statusData[static_cast<int>(Character::CharacterStatusDataSort::kFirstSpecialKind)]);

	//二つ目の必殺技の情報設定
	ans.secondSpecialAttackData.name = statusData[static_cast<int>(Character::CharacterStatusDataSort::kSecondSpecialName)];
	ans.secondSpecialAttackData.path = statusData[static_cast<int>(Character::CharacterStatusDataSort::kSecondSpecialEffectName)];
	ans.secondSpecialAttackData.cost = stoi(statusData[static_cast<int>(Character::CharacterStatusDataSort::kSecondSpecialCost)]);
	ans.secondSpecialAttackData.damageRate = stof(statusData[static_cast<int>(Character::CharacterStatusDataSort::kSecondSpecialDamageRate)]);
	ans.secondSpecialAttackData.radius = stof(statusData[static_cast<int>(Character::CharacterStatusDataSort::kSecondSpecialRadius)]);
	ans.secondSpecialAttackData.startFrame = stoi(statusData[static_cast<int>(Character::CharacterStatusDataSort::kSecondSpecialStartFrame)]);
	ans.secondSpecialAttackData.endFrame = stoi(statusData[static_cast<int>(Character::CharacterStatusDataSort::kSecondSpecialEndFrame)]);
	ans.secondSpecialAttackData.kind = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetSpecialAttackKind(statusData[static_cast<int>(Character::CharacterStatusDataSort::kSecondSpecialKind)]);
	return ans;
}
