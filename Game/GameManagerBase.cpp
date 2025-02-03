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
	//���t���[���{�^���A�őΌ����s���̂�
	constexpr int kButtonBashingTime = 300;

	//�{�^���A�őΌ��̎��̃J�����̃^�[�Q�b�g���W
	const MyEngine::Vector3 kButtonBashingCameraTargetPos(0, 200, 0);

	const MyEngine::Vector3 kButtonBashingCameraPos[static_cast<int>(GameManagerBase::ButtonBashingSituation::kSituationNum)] =
	{
		MyEngine::Vector3(-50,190,-100),
		MyEngine::Vector3(-30,190,70),
		MyEngine::Vector3(10,205,10)
	};

	//�J�����Ԃ��ړ����鎞��
	constexpr float kButtonBashingMoveTime = 20.0f;

	//�J�����̈ړ����x
	const float kButtonBashingCameraMoveSpeed[static_cast<int>(GameManagerBase::ButtonBashingSituation::kSituationNum)] =
	{
		0.0f,
		((kButtonBashingCameraPos[static_cast<int>(GameManagerBase::ButtonBashingSituation::kSecondHit)] - kButtonBashingCameraPos[static_cast<int>(GameManagerBase::ButtonBashingSituation::kFirstHit)]) / kButtonBashingMoveTime).Length(),
		((kButtonBashingCameraPos[static_cast<int>(GameManagerBase::ButtonBashingSituation::kFighting)] - kButtonBashingCameraPos[static_cast<int>(GameManagerBase::ButtonBashingSituation::kSecondHit)]) / kButtonBashingMoveTime).Length()
	};

	//�J��������]�����鑬�x
	constexpr float kCameraRotaSpeed = 0.08f;

	//��]���̃J�����̋���
	constexpr float kRotaCameraDistance = 30.0f;

	//�A�ł���{�^���̎�ސ�
	constexpr int kBashingButtonNum = 4;

	//�A�ł���{�^���̎��
	const std::string kBashingButtonKind[kBashingButtonNum] =
	{
		"A",
		"B",
		"X",
		"Y"
	};

	//�{�^���A�Ŏ��̃J������h�炷���o
	constexpr int kButtonBashingCameraShakeInterval = 6;

	//�{�^���A�Ŏ��̃J�����̗h��̑傫��
	constexpr int kButtonBashingCameraShakePower = 1;

	//�{�^���A�ŏI�����̃T�E���h�̉���
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

	//�J�����̍��W�ݒ�
	MyEngine::Vector3 onePlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetPos();
	MyEngine::Vector3 twoPlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetPos();
	//1P�𒆐S��1P���^�[�Q�b�g�ɂ���
	m_pCamera->SetCenterAndTarget(onePlayerPos, onePlayerPos);

	m_pCamera->SetLocalPos(m_poseCameraPos);

	m_pCamera->SetFrontPos(twoPlayerPos);

	m_pGameUi->RetryInit();
}

void GameManagerBase::UpdateAsyncLoad()
{
	//�񓯊����[�h���I�����邩�ǂ���
	bool isEnd = false;
	auto& manager = LoadManager::GetInstance();

	//�n���h�����擾����
	for (auto& item : m_modelHandles)
	{
		item.second = manager.GetHandle(item.first);
	}

	if (GetASyncLoadNum() == 0)
	{
		isEnd = true;
	}


	//�n���h�������ׂĎ擾�ł��Ă�����
	if (isEnd)
	{

		//���f���n���h����n��
		m_pCharacters[0]->SetModelHandle(m_modelHandles.at("Player1"));
		m_pCharacters[1]->SetModelHandle(m_modelHandles.at("Player2"));

		m_pStage->SetStageModelHandle(m_modelHandles.at("Stage"));
		m_pStage->SetSkyDomeModelHandle(m_modelHandles.at("SkyDome"));

		m_rockModelHandle[0] = m_modelHandles.at("Rock1");
		m_rockModelHandle[1] = m_modelHandles.at("Rock2");
		m_rockModelHandle[2] = m_modelHandles.at("Rock3");

		//�񓯊����[�h���I�������^�C�~���O��Init���Ă�
		Init();

		//�񓯊����[�h���I������
		manager.EndAsyncLoad();
	}
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

void GameManagerBase::SetTwoPlayerStatus(int number, std::vector<std::string> statusData, int enemyLevel)
{
	//�G�l�~�[�쐬
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)] = std::make_shared<Character>(ObjectTag::kTwoPlayer, static_cast<Character::CharacterKind>(number));
	//�G�l�~�[�Ɏ����̃|�C���^�[��n���Ă���
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->SetGameManager(shared_from_this());

	//�G�l�~�[�̃��x����ݒ肷��
	m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->SetEnemyInput(enemyLevel);

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
	//�ΐ푊��̔w���̍��W���擾����
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

	//�Ԃ������Ƃ��̃T�E���h���Đ�����
	SoundManager::GetInstance().PlayOnceSound("HighHit");

	m_buttonBashingCameraPos = kButtonBashingCameraPos[static_cast<int>(m_buttonBashingSituation)];

	//�A�ł���{�^���������_���Ō��߂�
	m_bashingButton = kBashingButtonKind[GetRand(kBashingButtonNum - 1)];
}

void GameManagerBase::EntryObject(std::shared_ptr<ObjectBase> object)
{
	//�I�u�W�F�N�g��o�^����
	m_pObjects.push_back(object);
}

void GameManagerBase::UpdateCommon()
{
	//�{�^���A�ŏ�Ԃ�������
	if (m_isButtonBashing)
	{
		UpdateButtonBashing();
	}

	//�X�e�[�W�̍X�V���s��
	m_pStage->Update();

	//�I�u�W�F�N�g�̍X�V���s��
	for (auto& item : m_pObjects)
	{
		item->Update();
	}

	//�U���N���X�̍X�V���s��
	for (auto& item : m_pAttacks)
	{
		//�U����Update
		item->Update();
	}

	//�U���̍폜
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

	//�I�u�W�F�N�g�̍폜
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
	//�G�t�F�N�g�̍X�V
	m_pEffectManager->Update();
}

void GameManagerBase::DrawCommon()
{

	//�I�u�W�F�N�g�̕`��
	for (auto& item : m_pObjects)
	{
		item->Draw();
	}

	//�U���̕`��
	for (auto& item : m_pAttacks)
	{
		item->Draw();
	}

	//�X�e�[�W�̕`��
	m_pStage->Draw();

	//�G�t�F�N�g�̕`��
	m_pEffectManager->Draw();

	//�̗͂�`�悷�邩�ǂ���
	if (m_isDrawHpBar)
	{
		//1P�̗̑͂�`�悷��
		m_pGameUi->DrawHpBar(m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetHp(), true);
		//2P�̗̑͂�`�悷��
		m_pGameUi->DrawHpBar(m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetHp(), false);
		//1P�̋C�͂�`�悷��
		m_pGameUi->DrawMpBar(m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetMp(), true);
		//2P�̋C�͂�`�悷��
		m_pGameUi->DrawMpBar(m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetMp(), false);
	}

	//�{�^���A�ŏ�Ԃ�������
	if (m_isButtonBashing)
	{
		//�{�^���A�ł�UI��`�悷��
		DrawButtonBashing();
	}
}

void GameManagerBase::UpdateButtonBashing()
{
	//�s���Ă��鎞�Ԃ��v������
	m_buttonBashingTime++;

	//�J������h�炷
	if (m_buttonBashingSituation == ButtonBashingSituation::kFighting)
	{
		if (m_buttonBashingTime % kButtonBashingCameraShakeInterval == 0)
		{
			m_pCamera->ShakeCamera(1, kButtonBashingCameraShakePower);
		}

		//�{�^���A�ł̃G�t�F�N�g��o�^����
		if (!m_pButtonBashingEffect)
		{
			//�Ԃ����Ă���Ƃ��̃G�t�F�N�g���Đ�����
			m_pButtonBashingEffect = std::make_shared<Effect>(Effect::EffectKind::kShock);

			m_pButtonBashingEffect->SetPos(kButtonBashingCameraTargetPos);

			m_pButtonBashingEffect->SetLoop(10, 15);

			EntryEffect(m_pButtonBashingEffect);
		}
	}

	//�J�������ڎw�����W
	MyEngine::Vector3 cameraGoalPos = kButtonBashingCameraPos[static_cast<int>(m_buttonBashingSituation)];

	//�J�����̈ړ����x
	float cameraMoveSpeed = kButtonBashingCameraMoveSpeed[static_cast<int>(m_buttonBashingSituation)];

	//���݂̃J�����̍��W�ƖړI�n�܂ł̃x�N�g��
	MyEngine::Vector3 cameraToGoalVec = cameraGoalPos - m_buttonBashingCameraPos;


	//�J�������񂷎���true�ɂ���
	bool isRota = false;

	//�ړI�n�܂ł̃x�N�g�������ړ��x�N�g���̕����傫��������
	if (cameraMoveSpeed > cameraToGoalVec.Length())
	{
		cameraMoveSpeed = cameraToGoalVec.Length();

		if (m_buttonBashingSituation == ButtonBashingSituation::kFighting)
		{
			isRota = true;
		}
	}

	//��]����Ɛݒ肳��ĂȂ����
	if (!isRota)
	{
		//�J�������W�̍X�V
		m_buttonBashingCameraPos += cameraToGoalVec.Normalize() * cameraMoveSpeed;
		m_pCamera->SetCenterAndTarget(m_buttonBashingCameraPos, kButtonBashingCameraTargetPos);
	}
	//��]����ꍇ
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

	//�J�����̐ݒ�
	m_pCamera->SetLocalPos(MyEngine::Vector3(0, 0, 0));
	m_pCamera->SetPoseCamera();
	m_pCamera->SetFrontPos(kButtonBashingCameraTargetPos);
	m_pCamera->Update();

	//��莞�ԍs�������߂�
	if (m_buttonBashingTime > kButtonBashingTime)
	{
		m_isButtonBashing = false;
		m_pCamera->SetBattleCamera();

		//�{�^���A�ŏI�����̃T�E���h�̉��ʂ�ݒ肷��
		SoundManager::GetInstance().SetSoundVolume("WinButtonBashing", kWinButtonBashingSoundVolume);

		//�{�^���A�ŏI���̃T�E���h���Đ�����
		SoundManager::GetInstance().PlayOnceSound("WinButtonBashing");

		//�G�t�F�N�g���폜����
		m_pEffectManager->Exit(m_pButtonBashingEffect);

		//�G�t�F�N�g�̃|�C���^�[������������
		m_pButtonBashingEffect = nullptr;
	}
}

void GameManagerBase::DrawButtonBashing()
{
	//���Ԃ���܂ł͕\�����Ȃ�
	if (m_buttonBashingSituation != ButtonBashingSituation::kFighting) return;
	//�ǂ̃{�^����A�ł���΂����̂���\������
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
