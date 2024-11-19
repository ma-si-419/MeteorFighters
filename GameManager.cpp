#include "GameManager.h"
#include "Attack.h"
#include "LoadCsv.h"
#include "Stage.h"
#include "GameCamera.h"
#include "CharacterBase.h"
#include "Effectmanager.h"
#include "Effect.h"
#include "GameUi.h"

GameManager::GameManager(std::shared_ptr<GameCamera> camera)
{
	m_pStage = std::make_shared<Stage>();
	m_pStage->Init();
	m_pCamera = camera;

	m_pEffectManager = std::make_shared<EffectManager>();
	m_pGameUi = std::make_shared<GameUi>();
}

GameManager::~GameManager()
{
}

void GameManager::Init()
{
	m_pCamera->Init(m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetPos());
}

void GameManager::Update()
{
#ifdef _DEBUG

	MyEngine::Vector3 pos = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetPos();

	DrawFormatString(0, 80, GetColor(255, 255, 255), "プレイヤーの座標(%0.1f,%0.1f,%0.1f)", pos.x, pos.y, pos.z);

	pos = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->GetPos();

	DrawFormatString(0, 96, GetColor(255, 255, 255), "エネミーの座標(%0.1f,%0.1f,%0.1f)", pos.x, pos.y, pos.z);

#endif // _DEBUG

	//カメラの更新
	MyEngine::Vector3 onePlayerPos = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetPos();
	MyEngine::Vector3 twoPlayerPos = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->GetPos();

	MyEngine::Vector3 cameraTargetPos = twoPlayerPos;

	cameraTargetPos.y = twoPlayerPos.y;

	m_pCamera->SetPlayerPosAndTarget(onePlayerPos, cameraTargetPos);

	//1Pから2Pへのベクトル
	MyEngine::Vector3 playerToTarget = (twoPlayerPos - onePlayerPos).Normalize();

	//カメラの正面方向を設定
	m_pCamera->SetPlayerFrontPos(onePlayerPos + playerToTarget);

	//カメラにプレイヤーのベロシティを設定する
	m_pCamera->SetPlayerVelo(m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetVelo());

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

void GameManager::Draw()
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
	m_pGameUi->DrawHpBar(m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetHp(),true);
	//2Pの体力を描画する
	m_pGameUi->DrawHpBar(m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->GetHp(),false);


}

void GameManager::SetPlayerStatus(int number, std::vector<std::string> statusData)
{
	//プレイヤー作成
	m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)] = std::make_shared<CharacterBase>(ObjectTag::kOnePlayer,static_cast<CharacterBase::CharacterKind>(number));
	//プレイヤーに自分のポインターを渡しておく
	m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->SetGameManager(shared_from_this());

	CharacterBase::CharacterStatus status;

	status.name = statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kName)];
	status.hp = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kHp)]);
	status.startMp = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kStartMp)]);
	status.atk = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kAtk)]);
	status.def = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kDef)]);
	status.spd = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSpd)]);
	status.chargeSpd = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kChargeSpd)]);
	status.chaseAttackNum = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kChaseNum)]);

	//一つ目の必殺技の情報設定
	status.firstSpecialAttackData.name = statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialName)];
	status.firstSpecialAttackData.cost = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialCost)]);
	status.firstSpecialAttackData.damageRate = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialDamageRate)]);
	status.firstSpecialAttackData.startFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialStartFrame)]);
	status.firstSpecialAttackData.endFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialEndFrame)]);
	status.firstSpecialAttackData.kind = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetSpecialAttackKind(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialKind)]);

	//二つ目の必殺技の情報設定
	status.secondSpecialAttackData.name = statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialCost)];
	status.secondSpecialAttackData.cost = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialCost)]);
	status.secondSpecialAttackData.damageRate = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialDamageRate)]);
	status.secondSpecialAttackData.startFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialStartFrame)]);
	status.secondSpecialAttackData.endFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialEndFrame)]);
	status.secondSpecialAttackData.kind = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetSpecialAttackKind(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialKind)]);

	//プレイヤーのステータスを設定する
	m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->SetStatus(status);
	m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->SetCharacterNumber(CharacterBase::PlayerNumber::kOnePlayer);

}

void GameManager::SetEnemyStatus(int number, std::vector<std::string> statusData)
{
	//エネミー作成
	m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)] = std::make_shared<CharacterBase>(ObjectTag::kTwoPlayer,static_cast<CharacterBase::CharacterKind>(number));
	//エネミーに自分のポインターを渡しておく
	m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->SetGameManager(shared_from_this());
	CharacterBase::CharacterStatus status;

	status.name = statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kName)];
	status.hp = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kHp)]);
	status.startMp = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kStartMp)]);
	status.atk = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kAtk)]);
	status.def = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kDef)]);
	status.spd = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSpd)]);
	status.chargeSpd = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kChargeSpd)]);
	status.chaseAttackNum = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kChaseNum)]);

	//一つ目の必殺技の情報設定
	status.firstSpecialAttackData.name = statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialName)];
	status.firstSpecialAttackData.cost = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialCost)]);
	status.firstSpecialAttackData.damageRate = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialDamageRate)]);
	status.firstSpecialAttackData.startFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialStartFrame)]);
	status.firstSpecialAttackData.endFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialEndFrame)]);
	status.firstSpecialAttackData.kind = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->GetSpecialAttackKind(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialKind)]);

	//二つ目の必殺技の情報設定
	status.secondSpecialAttackData.name = statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialCost)];
	status.secondSpecialAttackData.cost = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialCost)]);
	status.secondSpecialAttackData.damageRate = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialDamageRate)]);
	status.secondSpecialAttackData.startFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialStartFrame)]);
	status.secondSpecialAttackData.endFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialEndFrame)]);
	status.secondSpecialAttackData.kind = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->GetSpecialAttackKind(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialKind)]);

	//エネミーのステータスを設定する
	m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->SetStatus(status);
	m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->SetCharacterNumber(CharacterBase::PlayerNumber::kTwoPlayer);
}

MyEngine::Vector3 GameManager::GetTargetPos(std::shared_ptr<CharacterBase> character)
{
	if (character == m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)])
	{
		return m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->GetPos();
	}
	else if(character == m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)])
	{
		return m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetPos();
	}
	else
	{
		return MyEngine::Vector3(0,0,0);
	}
}

MyEngine::Vector3 GameManager::GetTargetVelo(std::shared_ptr<CharacterBase> character)
{
	if (character == m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)])
	{
		return m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->GetVelo();
	}
	else if (character == m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)])
	{
		return m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetVelo();
	}
	else
	{
		return MyEngine::Vector3(0, 0, 0);
	}
}

CharacterBase::HitReactionKind GameManager::GetTargetHitReaction(std::shared_ptr<CharacterBase> character)
{
	if (character == m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)])
	{
		return m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->GetHitReaction();
	}
	else if (character == m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)])
	{
		return m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetHitReaction();
	}
	else
	{
		return CharacterBase::HitReactionKind::kNone;
	}
}

void GameManager::AddAttack(std::shared_ptr<Attack> attack)
{
	m_pAttacks.push_back(attack);
}

void GameManager::StartFastCameraMove()
{
	m_pCamera->StartFastCameraMove();
}

void GameManager::StopMoveCamera()
{
	m_pCamera->StopCamera();
}

void GameManager::StartMoveCamera()
{
	m_pCamera->StartMoveCamera();
}

void GameManager::ShakeCamera(int time)
{
	m_pCamera->ShakeCamera(time);
}

void GameManager::EntryEffect(std::shared_ptr<Effect> effect)
{
	m_pEffectManager->Entry(effect,effect->GetPos());
}

void GameManager::ExitEffect(std::shared_ptr<Effect> effect)
{
	m_pEffectManager->Exit(effect);
}

int GameManager::GetGraphHandle(std::string graphName)
{
	return 0;
}

MyEngine::Vector3 GameManager::GetTargetBackPos(float distance, std::shared_ptr<CharacterBase> character)
{
	if (character == m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)])
	{
		return m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->GetBackPos(distance);
	}
	else if (character == m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)])
	{
		return m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetBackPos(distance);
	}
	else
	{
		return MyEngine::Vector3(0,0,0);
	}
}