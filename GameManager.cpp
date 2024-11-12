#include "GameManager.h"
#include "Player.h"
#include "Enemy.h"
#include "Attack.h"
#include "LoadCsv.h"
#include "Stage.h"
#include "GameCamera.h"

GameManager::GameManager(std::shared_ptr<GameCamera> camera)
{
	m_pStage = std::make_shared<Stage>();
	m_pStage->Init();
	m_pCamera = camera;
}

GameManager::~GameManager()
{
}

void GameManager::Init()
{
	m_pCamera->Init(GetPlayerPos());
}

void GameManager::Update()
{
#ifdef _DEBUG

	MyEngine::Vector3 pos = m_pPlayer->GetPos();

	DrawFormatString(0,80,GetColor(255,255,255),"プレイヤーの座標(%0.1f,%0.1f,%0.1f)",pos.x,pos.y,pos.z);
	
	pos = m_pEnemy->GetPos();

	DrawFormatString(0,96,GetColor(255,255,255),"エネミーの座標(%0.1f,%0.1f,%0.1f)",pos.x,pos.y,pos.z);

#endif // _DEBUG

	//カメラの更新
	MyEngine::Vector3 cameraTargetPos = GetEnemyPos();

	cameraTargetPos.y = GetPlayerPos().y;

	m_pCamera->SetPlayerPosAndTarget(GetPlayerPos(), cameraTargetPos);

	//プレイヤーからエネミーへのベクトル
	MyEngine::Vector3 playerToTarget = (GetEnemyPos() - GetPlayerPos()).Normalize();

	//カメラの正面方向を設定
	m_pCamera->SetPlayerFrontPos(GetPlayerPos() + playerToTarget);

	//カメラにプレイヤーのベロシティを設定する
	m_pCamera->SetPlayerVelo(m_pPlayer->GetVelo());

	//カメラの更新
	m_pCamera->Update();

	for (auto& item : m_pAttacks)
	{
		//攻撃の更新
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

}

void GameManager::SetPlayerStatus(int number,std::vector<std::string> statusData)
{
	//プレイヤー作成
	m_pPlayer = std::make_shared<Player>(static_cast<CharacterBase::CharacterKind>(number));
	//プレイヤーに自分のポインターを渡しておく
	m_pPlayer->SetGameManager(shared_from_this());

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
	status.firstSpecialAttackData.kind = m_pPlayer->GetSpecialAttackKind(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialKind)]);

	//二つ目の必殺技の情報設定
	status.secondSpecialAttackData.name = statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialCost)];
	status.secondSpecialAttackData.cost = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialCost)]);
	status.secondSpecialAttackData.damageRate = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialDamageRate)]);
	status.secondSpecialAttackData.startFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialStartFrame)]);
	status.secondSpecialAttackData.endFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialEndFrame)]);
	status.secondSpecialAttackData.kind = m_pPlayer->GetSpecialAttackKind(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialKind)]);

	//プレイヤーのステータスを設定する
	m_pPlayer->SetStatus(status);
}

void GameManager::SetEnemyStatus(int number,std::vector<std::string> statusData)
{
	//エネミー作成
	m_pEnemy = std::make_shared<Enemy>(static_cast<CharacterBase::CharacterKind>(number));
	//エネミーに自分のポインターを渡しておく
	m_pEnemy->SetGameManager(shared_from_this());
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
	status.firstSpecialAttackData.kind = m_pEnemy->GetSpecialAttackKind(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialKind)]);

	//二つ目の必殺技の情報設定
	status.secondSpecialAttackData.name = statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialCost)];
	status.secondSpecialAttackData.cost = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialCost)]);
	status.secondSpecialAttackData.damageRate = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialDamageRate)]);
	status.secondSpecialAttackData.startFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialStartFrame)]);
	status.secondSpecialAttackData.endFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialEndFrame)]);
	status.secondSpecialAttackData.kind = m_pEnemy->GetSpecialAttackKind(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialKind)]);

	//エネミーのステータスを設定する
	m_pEnemy->SetStatus(status);
}

MyEngine::Vector3 GameManager::GetPlayerPos()
{
	return m_pPlayer->GetPos();
}

MyEngine::Vector3 GameManager::GetEnemyPos()
{
	return m_pEnemy->GetPos();
}

MyEngine::Vector3 GameManager::GetPlayerVelo()
{
	return m_pPlayer->GetVelo();
}

MyEngine::Vector3 GameManager::GetEnemyVelo()
{
	return m_pEnemy->GetVelo();
}

int GameManager::GetPlayerHitReaction()
{
	return static_cast<int>(m_pPlayer->GetHitReaction());
}

int GameManager::GetEnemyHitReaction()
{
	return static_cast<int>(m_pEnemy->GetHitReaction());
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

MyEngine::Vector3 GameManager::GetPlayerBackPos(float distance)
{
	return m_pPlayer->GetBackPos(distance);
}

MyEngine::Vector3 GameManager::GetEnemyBackPos(float distance)
{
	return m_pEnemy->GetBackPos(distance);
}