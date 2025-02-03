#include "GameManagerBase.h"
#include "Attack.h"
#include "LoadCsv.h"
#include "LocalPos.h"
#include "Stage.h"
#include "GameCamera.h"
#include "Character.h"
#include "Effectmanager.h"
#include "Loadmanager.h"
#include "Effect.h"
#include "GameUi.h"
#include "Game.h"
#include "ObjectBase.h"
#include <cmath>
#include "SoundManager.h"

namespace
{
	//何フレームボタン連打対決を行うのか
	constexpr int kButtonBashingTime = 300;

	//ボタン連打対決の時のカメラのターゲット座標
	const MyEngine::Vector3 kButtonBashingCameraTargetPos(0, 200, 0);

	const MyEngine::Vector3 kButtonBashingCameraPos[static_cast<int>(GameManagerBase::ButtonBashingSituation::kSituationNum)] =
	{
		MyEngine::Vector3(-50,190,-100),
		MyEngine::Vector3(-30,190,70),
		MyEngine::Vector3(10,205,10)
	};

	//カメラ間を移動する時間
	constexpr float kButtonBashingMoveTime = 20.0f;

	//カメラの移動速度
	const float kButtonBashingCameraMoveSpeed[static_cast<int>(GameManagerBase::ButtonBashingSituation::kSituationNum)] =
	{
		0.0f,
		((kButtonBashingCameraPos[static_cast<int>(GameManagerBase::ButtonBashingSituation::kSecondHit)] - kButtonBashingCameraPos[static_cast<int>(GameManagerBase::ButtonBashingSituation::kFirstHit)]) / kButtonBashingMoveTime).Length(),
		((kButtonBashingCameraPos[static_cast<int>(GameManagerBase::ButtonBashingSituation::kFighting)] - kButtonBashingCameraPos[static_cast<int>(GameManagerBase::ButtonBashingSituation::kSecondHit)]) / kButtonBashingMoveTime).Length()
	};

	//カメラを回転させる速度
	constexpr float kCameraRotaSpeed = 0.08f;

	//回転中のカメラの距離
	constexpr float kRotaCameraDistance = 30.0f;

	//連打するボタンの種類数
	constexpr int kBashingButtonNum = 4;

	//連打するボタンの種類
	const std::string kBashingButtonKind[kBashingButtonNum] =
	{
		"A",
		"B",
		"X",
		"Y"
	};

	//ボタン連打時のカメラを揺らす感覚
	constexpr int kButtonBashingCameraShakeInterval = 6;

	//ボタン連打時のカメラの揺れの大きさ
	constexpr int kButtonBashingCameraShakePower = 1;

	//ボタン連打終了時のサウンドの音量
	constexpr int kWinButtonBashingSoundVolume = 255;

}

GameManagerBase::GameManagerBase(std::shared_ptr<GameCamera> camera, GameManagerBase::GameKind kind) :
	m_time(0),
	m_alpha(0),
	m_nextScene(Game::Scene::kGame),
	m_situation(BattleSituation::kStart1P),
	m_gameKind(kind),
	m_buttonBashingTime(0),
	m_buttonBashNum(),
	m_isButtonBashing(false),
	m_buttonBashingCameraRota(0.0f),
	m_bashingButton("empty"),
	m_buttonBashingSituation(ButtonBashingSituation::kFirstHit),
	m_isDrawHpBar(true)
{
	m_pCamera = camera;

	m_pEffectManager = std::make_shared<EffectManager>();
	m_pGameUi = std::make_shared<GameUi>();
	m_pStage = std::make_shared<Stage>(m_pEffectManager);
	m_pStage->Init();

}

GameManagerBase::~GameManagerBase()
{
}

void GameManagerBase::RetryInit()
{
	m_pCharacters[0]->RetryInit();
	m_pCharacters[1]->RetryInit();

	//カメラの座標設定
	MyEngine::Vector3 onePlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetPos();
	MyEngine::Vector3 twoPlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetPos();
	//1Pを中心に1Pをターゲットにする
	m_pCamera->SetCenterAndTarget(onePlayerPos, onePlayerPos);

	m_pCamera->SetLocalPos(m_poseCameraPos);

	m_pCamera->SetFrontPos(twoPlayerPos);

	m_pGameUi->RetryInit();
}

void GameManagerBase::UpdateAsyncLoad()
{
	//非同期ロードを終了するかどうか
	bool isEnd = false;
	auto& manager = LoadManager::GetInstance();

	//ハンドルを取得する
	for (auto& item : m_modelHandles)
	{
		item.second = manager.GetHandle(item.first);
	}

	if (GetASyncLoadNum() == 0)
	{
		isEnd = true;
	}


	//ハンドルがすべて取得できていたら
	if (isEnd)
	{

		//モデルハンドルを渡す
		m_pCharacters[0]->SetModelHandle(m_modelHandles.at("Player1"));
		m_pCharacters[1]->SetModelHandle(m_modelHandles.at("Player2"));

		m_pStage->SetStageModelHandle(m_modelHandles.at("Stage"));
		m_pStage->SetSkyDomeModelHandle(m_modelHandles.at("SkyDome"));

		m_rockModelHandle[0] = m_modelHandles.at("Rock1");
		m_rockModelHandle[1] = m_modelHandles.at("Rock2");
		m_rockModelHandle[2] = m_modelHandles.at("Rock3");

		//非同期ロードが終了したタイミングでInitを呼ぶ
		Init();

		//非同期ロードを終了する
		manager.EndAsyncLoad();
	}
}

void GameManagerBase::SetOnePlayerStatus(int number, std::vector<std::string> statusData)
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

void GameManagerBase::SetTwoPlayerStatus(int number, std::vector<std::string> statusData, int enemyLevel)
{
	//エネミー作成
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)] = std::make_shared<Character>(ObjectTag::kTwoPlayer, static_cast<Character::CharacterKind>(number));
	//エネミーに自分のポインターを渡しておく
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->SetGameManager(shared_from_this());

	//エネミーのレベルを設定する
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->SetEnemyInput(enemyLevel);

	//ステータスを取得
	Character::CharacterStatus status = GetCharacterStatus(statusData);

	//エネミーのステータスを設定する
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->SetStatus(status);
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->SetCharacterNumber(Character::PlayerNumber::kTwoPlayer);
}

MyEngine::Vector3 GameManagerBase::GetTargetPos(std::shared_ptr<Character> character)
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

MyEngine::Vector3 GameManagerBase::GetTargetVelo(std::shared_ptr<Character> character)
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

Character::HitReactionKind GameManagerBase::GetTargetHitReaction(std::shared_ptr<Character> character)
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

int GameManagerBase::GetTargetState(std::shared_ptr<Character> character)
{
	if (character == m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)])
	{
		return m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetStateKind();
	}
	else if (character == m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)])
	{
		return m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetStateKind();
	}

	return 0;
}

void GameManagerBase::AddAttack(std::shared_ptr<Attack> attack)
{
	m_pAttacks.push_back(attack);
}

void GameManagerBase::StartFastCameraMove()
{
	m_pCamera->StartFastCameraMove();
}

void GameManagerBase::StopCameraCorrection()
{
	m_pCamera->StopCamera();
}

void GameManagerBase::StopCamera(int time)
{
	m_pCamera->StopCamera(time);
}

void GameManagerBase::StartCameraCorrection()
{
	m_pCamera->StartMoveCamera();
}

void GameManagerBase::ShakeCamera(int time)
{
	m_pCamera->ShakeCamera(time);
}

void GameManagerBase::ShakeCamera(int time, int power)
{
	m_pCamera->ShakeCamera(time, power);
}

void GameManagerBase::SwayCamera()
{
	m_pCamera->SwayCamera();
}

void GameManagerBase::EntryEffect(std::shared_ptr<Effect> effect)
{
	m_pEffectManager->Entry(effect, effect->GetPos());
}

void GameManagerBase::ExitEffect(std::shared_ptr<Effect> effect)
{
	m_pEffectManager->Exit(effect);
}

MyEngine::Vector3 GameManagerBase::GetTargetBackPos(float distance, std::shared_ptr<Character> character)
{
	//対戦相手の背中の座標を取得する
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

std::string GameManagerBase::GetStagePath()
{
	return m_pStage->GetStagePath();
}

std::string GameManagerBase::GetSkyDomePath()
{
	return m_pStage->GetSkyDomePath();
}

Character::PlayerNumber GameManagerBase::GetButtonBashWinner()
{
	int one = m_buttonBashNum[static_cast<int>(Character::PlayerNumber::kOnePlayer)];
	int two = m_buttonBashNum[static_cast<int>(Character::PlayerNumber::kTwoPlayer)];

	if (one >= two)
	{
		return Character::PlayerNumber::kOnePlayer;
	}
	else
	{
		return Character::PlayerNumber::kTwoPlayer;
	}
}

void GameManagerBase::StartButtonBashing()
{
	m_isButtonBashing = true;
	m_buttonBashingTime = 0;
	m_buttonBashNum[0] = 0;
	m_buttonBashNum[1] = 0;
	m_buttonBashingSituation = ButtonBashingSituation::kFirstHit;

	//ぶつかったときのサウンドを再生する
	SoundManager::GetInstance().PlayOnceSound("HighHit");

	m_buttonBashingCameraPos = kButtonBashingCameraPos[static_cast<int>(m_buttonBashingSituation)];

	//連打するボタンをランダムで決める
	m_bashingButton = kBashingButtonKind[GetRand(kBashingButtonNum - 1)];
}

void GameManagerBase::EntryObject(std::shared_ptr<ObjectBase> object)
{
	//オブジェクトを登録する
	m_pObjects.push_back(object);
}

void GameManagerBase::UpdateCommon()
{
	//ボタン連打状態だったら
	if (m_isButtonBashing)
	{
		UpdateButtonBashing();
	}

	//ステージの更新を行う
	m_pStage->Update();

	//オブジェクトの更新を行う
	for (auto& item : m_pObjects)
	{
		item->Update();
	}

	//攻撃クラスの更新を行う
	for (auto& item : m_pAttacks)
	{
		//攻撃のUpdate
		item->Update();
	}

	//攻撃の削除
	{
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
	}

	//オブジェクトの削除
	{
		auto iterator = std::remove_if(m_pObjects.begin(), m_pObjects.end(),
			[](const auto& item)
			{
				if (!item->IsExist())
				{
					item->Final();

					return true;
				}

				return false;
			});
		m_pObjects.erase(iterator, m_pObjects.end());
	}
	//エフェクトの更新
	m_pEffectManager->Update();
}

void GameManagerBase::DrawCommon()
{

	//オブジェクトの描画
	for (auto& item : m_pObjects)
	{
		item->Draw();
	}

	//攻撃の描画
	for (auto& item : m_pAttacks)
	{
		item->Draw();
	}

	//ステージの描画
	m_pStage->Draw();

	//エフェクトの描画
	m_pEffectManager->Draw();

	//体力を描画するかどうか
	if (m_isDrawHpBar)
	{
		//1Pの体力を描画する
		m_pGameUi->DrawHpBar(m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetHp(), true);
		//2Pの体力を描画する
		m_pGameUi->DrawHpBar(m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetHp(), false);
		//1Pの気力を描画する
		m_pGameUi->DrawMpBar(m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetMp(), true);
		//2Pの気力を描画する
		m_pGameUi->DrawMpBar(m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetMp(), false);
	}

	//ボタン連打状態だったら
	if (m_isButtonBashing)
	{
		//ボタン連打のUIを描画する
		DrawButtonBashing();
	}
}

void GameManagerBase::UpdateButtonBashing()
{
	//行っている時間を計測する
	m_buttonBashingTime++;

	//カメラを揺らす
	if (m_buttonBashingSituation == ButtonBashingSituation::kFighting)
	{
		if (m_buttonBashingTime % kButtonBashingCameraShakeInterval == 0)
		{
			m_pCamera->ShakeCamera(1, kButtonBashingCameraShakePower);
		}

		//ボタン連打のエフェクトを登録する
		if (!m_pButtonBashingEffect)
		{
			//ぶつかっているときのエフェクトを再生する
			m_pButtonBashingEffect = std::make_shared<Effect>(Effect::EffectKind::kShock);

			m_pButtonBashingEffect->SetPos(kButtonBashingCameraTargetPos);

			m_pButtonBashingEffect->SetLoop(10, 15);

			EntryEffect(m_pButtonBashingEffect);
		}
	}

	//カメラが目指す座標
	MyEngine::Vector3 cameraGoalPos = kButtonBashingCameraPos[static_cast<int>(m_buttonBashingSituation)];

	//カメラの移動速度
	float cameraMoveSpeed = kButtonBashingCameraMoveSpeed[static_cast<int>(m_buttonBashingSituation)];

	//現在のカメラの座標と目的地までのベクトル
	MyEngine::Vector3 cameraToGoalVec = cameraGoalPos - m_buttonBashingCameraPos;


	//カメラを回す時にtrueにする
	bool isRota = false;

	//目的地までのベクトルよりも移動ベクトルの方が大きかったら
	if (cameraMoveSpeed > cameraToGoalVec.Length())
	{
		cameraMoveSpeed = cameraToGoalVec.Length();

		if (m_buttonBashingSituation == ButtonBashingSituation::kFighting)
		{
			isRota = true;
		}
	}

	//回転すると設定されてなければ
	if (!isRota)
	{
		//カメラ座標の更新
		m_buttonBashingCameraPos += cameraToGoalVec.Normalize() * cameraMoveSpeed;
		m_pCamera->SetCenterAndTarget(m_buttonBashingCameraPos, kButtonBashingCameraTargetPos);
	}
	//回転する場合
	else
	{
		m_buttonBashingCameraRota += kCameraRotaSpeed;

		MyEngine::Vector3 rota = MyEngine::Vector3(0.0f, m_buttonBashingCameraRota, 0.0f);

		auto mat = rota.GetRotationMat();

		MyEngine::Vector3 targetToCamera = kButtonBashingCameraPos[static_cast<int>(ButtonBashingSituation::kFighting)] - kButtonBashingCameraTargetPos;

		MyEngine::Vector3 moveVec = targetToCamera.MatTransform(mat).Normalize() * kRotaCameraDistance;

		MyEngine::Vector3 cameraPos = moveVec + kButtonBashingCameraTargetPos;

		if (m_pCamera != nullptr)
		{
			m_pCamera->SetCenterAndTarget(cameraPos, kButtonBashingCameraTargetPos);
		}
	}

	//カメラの設定
	m_pCamera->SetLocalPos(MyEngine::Vector3(0, 0, 0));
	m_pCamera->SetPoseCamera();
	m_pCamera->SetFrontPos(kButtonBashingCameraTargetPos);
	m_pCamera->Update();

	//一定時間行ったらやめる
	if (m_buttonBashingTime > kButtonBashingTime)
	{
		m_isButtonBashing = false;
		m_pCamera->SetBattleCamera();

		//ボタン連打終了時のサウンドの音量を設定する
		SoundManager::GetInstance().SetSoundVolume("WinButtonBashing", kWinButtonBashingSoundVolume);

		//ボタン連打終了のサウンドを再生する
		SoundManager::GetInstance().PlayOnceSound("WinButtonBashing");

		//エフェクトを削除する
		m_pEffectManager->Exit(m_pButtonBashingEffect);

		//エフェクトのポインターを初期化する
		m_pButtonBashingEffect = nullptr;
	}
}

void GameManagerBase::DrawButtonBashing()
{
	//二回ぶつかるまでは表示しない
	if (m_buttonBashingSituation != ButtonBashingSituation::kFighting) return;
	//どのボタンを連打すればいいのかを表示する
	m_pGameUi->DrawBashButton(m_bashingButton);
}

Character::CharacterStatus GameManagerBase::GetCharacterStatus(std::vector<std::string> statusData)
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
