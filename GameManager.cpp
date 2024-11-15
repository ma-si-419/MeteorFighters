#include "GameManager.h"
#include "Attack.h"
#include "LoadCsv.h"
#include "Stage.h"
#include "GameCamera.h"
#include "CharacterBase.h"

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
	m_pCamera->Init(m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kOnePlayer)]->GetPos());
}

void GameManager::Update()
{
#ifdef _DEBUG

	MyEngine::Vector3 pos = m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kOnePlayer)]->GetPos();

	DrawFormatString(0, 80, GetColor(255, 255, 255), "�v���C���[�̍��W(%0.1f,%0.1f,%0.1f)", pos.x, pos.y, pos.z);

	pos = m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kTwoPlayer)]->GetPos();

	DrawFormatString(0, 96, GetColor(255, 255, 255), "�G�l�~�[�̍��W(%0.1f,%0.1f,%0.1f)", pos.x, pos.y, pos.z);

#endif // _DEBUG

	//�J�����̍X�V
	MyEngine::Vector3 onePlayerPos = m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kOnePlayer)]->GetPos();
	MyEngine::Vector3 twoPlayerPos = m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kTwoPlayer)]->GetPos();

	MyEngine::Vector3 cameraTargetPos = twoPlayerPos;

	cameraTargetPos.y = twoPlayerPos.y;

	m_pCamera->SetPlayerPosAndTarget(onePlayerPos, cameraTargetPos);

	//1P����2P�ւ̃x�N�g��
	MyEngine::Vector3 playerToTarget = (twoPlayerPos - onePlayerPos).Normalize();

	//�J�����̐��ʕ�����ݒ�
	m_pCamera->SetPlayerFrontPos(onePlayerPos + playerToTarget);

	//�J�����Ƀv���C���[�̃x���V�e�B��ݒ肷��
	m_pCamera->SetPlayerVelo(m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kOnePlayer)]->GetVelo());

	//�J�����̍X�V
	m_pCamera->Update();

	for (auto& item : m_pAttacks)
	{
		//�U���̍X�V
		item->Update();
	}

	//�U���̍폜
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
		//�U���̕`��
		item->Draw();
	}

	//�X�e�[�W�̕`��
	m_pStage->Draw();

}

void GameManager::SetPlayerStatus(int number, std::vector<std::string> statusData)
{
	//�v���C���[�쐬
	m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kOnePlayer)] = std::make_shared<CharacterBase>(ObjectTag::kOnePlayer,static_cast<CharacterBase::CharacterKind>(number));
	//�v���C���[�Ɏ����̃|�C���^�[��n���Ă���
	m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kOnePlayer)]->SetGameManager(shared_from_this());

	CharacterBase::CharacterStatus status;

	status.name = statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kName)];
	status.hp = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kHp)]);
	status.startMp = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kStartMp)]);
	status.atk = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kAtk)]);
	status.def = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kDef)]);
	status.spd = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSpd)]);
	status.chargeSpd = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kChargeSpd)]);
	status.chaseAttackNum = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kChaseNum)]);

	//��ڂ̕K�E�Z�̏��ݒ�
	status.firstSpecialAttackData.name = statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialName)];
	status.firstSpecialAttackData.cost = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialCost)]);
	status.firstSpecialAttackData.damageRate = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialDamageRate)]);
	status.firstSpecialAttackData.startFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialStartFrame)]);
	status.firstSpecialAttackData.endFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialEndFrame)]);
	status.firstSpecialAttackData.kind = m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kOnePlayer)]->GetSpecialAttackKind(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialKind)]);

	//��ڂ̕K�E�Z�̏��ݒ�
	status.secondSpecialAttackData.name = statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialCost)];
	status.secondSpecialAttackData.cost = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialCost)]);
	status.secondSpecialAttackData.damageRate = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialDamageRate)]);
	status.secondSpecialAttackData.startFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialStartFrame)]);
	status.secondSpecialAttackData.endFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialEndFrame)]);
	status.secondSpecialAttackData.kind = m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kOnePlayer)]->GetSpecialAttackKind(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialKind)]);

	//�v���C���[�̃X�e�[�^�X��ݒ肷��
	m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kOnePlayer)]->SetStatus(status);
	m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kOnePlayer)]->SetCharacterNumber(CharacterBase::CharacterNumber::kOnePlayer);

}

void GameManager::SetEnemyStatus(int number, std::vector<std::string> statusData)
{
	//�G�l�~�[�쐬
	m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kTwoPlayer)] = std::make_shared<CharacterBase>(ObjectTag::kTwoPlayer,static_cast<CharacterBase::CharacterKind>(number));
	//�G�l�~�[�Ɏ����̃|�C���^�[��n���Ă���
	m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kTwoPlayer)]->SetGameManager(shared_from_this());
	CharacterBase::CharacterStatus status;

	status.name = statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kName)];
	status.hp = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kHp)]);
	status.startMp = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kStartMp)]);
	status.atk = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kAtk)]);
	status.def = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kDef)]);
	status.spd = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSpd)]);
	status.chargeSpd = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kChargeSpd)]);
	status.chaseAttackNum = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kChaseNum)]);

	//��ڂ̕K�E�Z�̏��ݒ�
	status.firstSpecialAttackData.name = statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialName)];
	status.firstSpecialAttackData.cost = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialCost)]);
	status.firstSpecialAttackData.damageRate = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialDamageRate)]);
	status.firstSpecialAttackData.startFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialStartFrame)]);
	status.firstSpecialAttackData.endFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialEndFrame)]);
	status.firstSpecialAttackData.kind = m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kTwoPlayer)]->GetSpecialAttackKind(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialKind)]);

	//��ڂ̕K�E�Z�̏��ݒ�
	status.secondSpecialAttackData.name = statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialCost)];
	status.secondSpecialAttackData.cost = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialCost)]);
	status.secondSpecialAttackData.damageRate = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialDamageRate)]);
	status.secondSpecialAttackData.startFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialStartFrame)]);
	status.secondSpecialAttackData.endFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialEndFrame)]);
	status.secondSpecialAttackData.kind = m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kTwoPlayer)]->GetSpecialAttackKind(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialKind)]);

	//�G�l�~�[�̃X�e�[�^�X��ݒ肷��
	m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kTwoPlayer)]->SetStatus(status);
	m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kTwoPlayer)]->SetCharacterNumber(CharacterBase::CharacterNumber::kTwoPlayer);
}

MyEngine::Vector3 GameManager::GetTargetPos(std::shared_ptr<CharacterBase> character)
{
	if (character == m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kOnePlayer)])
	{
		return m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kTwoPlayer)]->GetPos();
	}
	else if(character == m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kTwoPlayer)])
	{
		return m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kOnePlayer)]->GetPos();
	}
	else
	{
		return MyEngine::Vector3(0,0,0);
	}
}

MyEngine::Vector3 GameManager::GetTargetVelo(std::shared_ptr<CharacterBase> character)
{
	if (character == m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kOnePlayer)])
	{
		return m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kTwoPlayer)]->GetVelo();
	}
	else if (character == m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kTwoPlayer)])
	{
		return m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kOnePlayer)]->GetVelo();
	}
	else
	{
		return MyEngine::Vector3(0, 0, 0);
	}
}

CharacterBase::HitReactionKind GameManager::GetTargetHitReaction(std::shared_ptr<CharacterBase> character)
{
	if (character == m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kOnePlayer)])
	{
		return m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kTwoPlayer)]->GetHitReaction();
	}
	else if (character == m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kTwoPlayer)])
	{
		return m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kOnePlayer)]->GetHitReaction();
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

MyEngine::Vector3 GameManager::GetTargetBackPos(float distance, std::shared_ptr<CharacterBase> character)
{
	if (character == m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kOnePlayer)])
	{
		return m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kTwoPlayer)]->GetBackPos(distance);
	}
	else if (character == m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kTwoPlayer)])
	{
		return m_pCharacters[static_cast<int>(CharacterBase::CharacterNumber::kOnePlayer)]->GetBackPos(distance);
	}
	else
	{
		return MyEngine::Vector3(0,0,0);
	}
}