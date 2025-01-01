#include "GameManagerBase.h"
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

GameManagerBase::GameManagerBase(std::shared_ptr<GameCamera> camera) :
	m_time(0),
	m_alpha(0),
	m_nextScene(Game::Scene::kGame),
	m_situation(BattleSituation::kStart1P)
{
	m_pStage = std::make_shared<Stage>();
	m_pStage->Init();
	m_pCamera = camera;

	m_pEffectManager = std::make_shared<EffectManager>();
	m_pGameUi = std::make_shared<GameUi>();

}

GameManagerBase::~GameManagerBase()
{
}

void GameManagerBase::RetryInit()
{
	m_pCharacters[0]->RetryInit();
	m_pCharacters[1]->RetryInit();

	//�J�����̍��W�ݒ�
	MyEngine::Vector3 onePlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetPos();
	MyEngine::Vector3 twoPlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetPos();
	//1P�𒆐S��1P���^�[�Q�b�g�ɂ���
	m_pCamera->SetCenterAndTarget(onePlayerPos, onePlayerPos);

	m_pCamera->SetLocalPos(m_poseCameraPos);

	m_pCamera->SetFrontPos(twoPlayerPos);

	m_pGameUi->RetryInit();
}

void GameManagerBase::SetOnePlayerStatus(int number, std::vector<std::string> statusData)
{
	//�v���C���[�쐬
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)] = std::make_shared<Character>(ObjectTag::kOnePlayer, static_cast<Character::CharacterKind>(number));
	//�v���C���[�Ɏ����̃|�C���^�[��n���Ă���
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->SetGameManager(shared_from_this());

	//�X�e�[�^�X���擾
	Character::CharacterStatus status = GetCharacterStatus(statusData);

	//�v���C���[�̃X�e�[�^�X��ݒ肷��
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->SetStatus(status);
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->SetCharacterNumber(Character::PlayerNumber::kOnePlayer);

}

void GameManagerBase::SetTwoPlayerStatus(int number, std::vector<std::string> statusData)
{
	//�G�l�~�[�쐬
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)] = std::make_shared<Character>(ObjectTag::kTwoPlayer, static_cast<Character::CharacterKind>(number));
	//�G�l�~�[�Ɏ����̃|�C���^�[��n���Ă���
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->SetGameManager(shared_from_this());

	//�X�e�[�^�X���擾
	Character::CharacterStatus status = GetCharacterStatus(statusData);

	//�G�l�~�[�̃X�e�[�^�X��ݒ肷��
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

void GameManagerBase::UpdateCommon()
{
	//�J�����̍X�V
	m_pCamera->Update();

	//�U���N���X�̍X�V���s��
	for (auto& item : m_pAttacks)
	{
		//�U����Update
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

	//�G�t�F�N�g�̍X�V
	m_pEffectManager->Update();
}

void GameManagerBase::DrawCommon()
{
	for (auto& item : m_pAttacks)
	{
		//�U���̕`��
		item->Draw();
	}

	//�X�e�[�W�̕`��
	m_pStage->Draw();

	//�G�t�F�N�g�̕`��
	m_pEffectManager->Draw();

	//1P�̗̑͂�`�悷��
	m_pGameUi->DrawHpBar(m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetHp(), true);
	//2P�̗̑͂�`�悷��
	m_pGameUi->DrawHpBar(m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetHp(), false);
	//1P�̋C�͂�`�悷��
	m_pGameUi->DrawMpBar(m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetMp(), true);
	//2P�̋C�͂�`�悷��
	m_pGameUi->DrawMpBar(m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetMp(), false);
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

	//��ڂ̕K�E�Z�̏��ݒ�
	ans.firstSpecialAttackData.name = statusData[static_cast<int>(Character::CharacterStatusDataSort::kFirstSpecialName)];
	ans.firstSpecialAttackData.path = statusData[static_cast<int>(Character::CharacterStatusDataSort::kFirstSpecialEffectName)];
	ans.firstSpecialAttackData.cost = stoi(statusData[static_cast<int>(Character::CharacterStatusDataSort::kFirstSpecialCost)]);
	ans.firstSpecialAttackData.damageRate = stof(statusData[static_cast<int>(Character::CharacterStatusDataSort::kFirstSpecialDamageRate)]);
	ans.firstSpecialAttackData.radius = stof(statusData[static_cast<int>(Character::CharacterStatusDataSort::kFirstSpecialRadius)]);
	ans.firstSpecialAttackData.startFrame = stoi(statusData[static_cast<int>(Character::CharacterStatusDataSort::kFirstSpecialStartFrame)]);
	ans.firstSpecialAttackData.endFrame = stoi(statusData[static_cast<int>(Character::CharacterStatusDataSort::kFirstSpecialEndFrame)]);
	ans.firstSpecialAttackData.kind = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetSpecialAttackKind(statusData[static_cast<int>(Character::CharacterStatusDataSort::kFirstSpecialKind)]);

	//��ڂ̕K�E�Z�̏��ݒ�
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
