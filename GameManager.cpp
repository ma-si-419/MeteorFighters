#include "GameManager.h"
#include "Attack.h"
#include "LoadCsv.h"
#include "Stage.h"
#include "GameCamera.h"
#include "CharacterBase.h"
#include "Effectmanager.h"
#include "Effect.h"
#include "GameUi.h"
#include "Game.h"
#include <cmath>

namespace
{
	//�o�g���J�n���̉��o�̃J�����X�^�[�g���W(���[�J��)
	const MyEngine::Vector3 kStartCameraStartPos(45.0f, 20.0f, 80.0f);

	//�o�g���J�n���̉��o�̃J�����G���h���W(���[�J��)
	const MyEngine::Vector3 kStartCameraEndPos(15.0f, 10.0f, 15.0f);

	//�J�n���̃J�����̈ړ����x
	constexpr float kStartCameraMoveSpeed = 2.0f;

	//�L�����N�^�[�̃A�j���[�V�������I����Ă��牽�t���[���J���������̂܂܂ɂ��邩
	constexpr int kStartPoseTime = 60;

	//�L�����N�^�[�̉��o���I����Ă��牽�t���[���Ńo�g���Ɉڍs���邩
	constexpr int kBattleStartTime = 120;

	//�m�b�N�A�E�g�������̃J�����̍��W(���[�J��)
	const MyEngine::Vector3 kKnockOutCameraPos(-15.0f, -5.0f, -20.0f);

	//�m�b�N�A�E�g���̃J�����^�[�Q�b�g�𒆐S����ǂꂾ��������
	constexpr float kKnockOutCameraTargetDistance = 10.0f;

	//�m�b�N�A�E�g�̉��o�̎���
	constexpr int kKnockOutTime = 60;

	//�t�F�[�h�̑��x
	constexpr int kFadeSpeed = 5;

	//�o�g���I�����̉��o�̃J�����X�^�[�g���W
	const MyEngine::Vector3 kResultCameraStartPos(0.0f,50.0f,50.0f);
	const MyEngine::Vector3 kResultCameraEndPos(0.0f,8.0f,18.0f);

	//�o�g���I�����̃J�����̈ړ����x
	constexpr float kResultCameraMoveSpeed = 1.0f;
}

GameManager::GameManager(std::shared_ptr<GameCamera> camera) :
	m_time(0),
	m_alpha(0)
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
	m_pCamera->SetPoseCamera();
	m_situation = Situation::kStart1P;
	m_updateSituationFunc = &GameManager::UpdateStart;
#ifdef _DEBUG
//	m_situation = Situation::kBattle;
//	m_updateSituationFunc = &GameManager::UpdateBattle;
#endif // _DEBUG


	m_poseCameraPos = kStartCameraStartPos;
}

void GameManager::Update()
{
#ifdef _DEBUG

	MyEngine::Vector3 pos = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetPos();

	DrawFormatString(0, 80, GetColor(255, 255, 255), "�v���C���[�̍��W(%0.1f,%0.1f,%0.1f)", pos.x, pos.y, pos.z);

	pos = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->GetPos();

	DrawFormatString(0, 96, GetColor(255, 255, 255), "�G�l�~�[�̍��W(%0.1f,%0.1f,%0.1f)", pos.x, pos.y, pos.z);

#endif // _DEBUG

	//�󋵂ɂ���ĕς��Update
	(this->*m_updateSituationFunc)();


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

void GameManager::Draw()
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
	m_pGameUi->DrawHpBar(m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetHp(), true);
	//2P�̗̑͂�`�悷��
	m_pGameUi->DrawHpBar(m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->GetHp(), false);
	//1P�̋C�͂�`�悷��
	m_pGameUi->DrawMpBar(m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetMp(), true);
	//2P�̋C�͂�`�悷��
	m_pGameUi->DrawMpBar(m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->GetMp(), false);

	//�t�F�[�h�̃A���t�@�l��0���������̏ꍇ�\������
	if (m_alpha > 0)
	{
		m_pGameUi->DrawFade(GetColor(255, 255, 255), m_alpha);
	}

	if (m_situation == Situation::kResult)
	{
		m_pGameUi->DrawResult(true);
	}


}

void GameManager::SetPlayerStatus(int number, std::vector<std::string> statusData)
{
	//�v���C���[�쐬
	m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)] = std::make_shared<CharacterBase>(ObjectTag::kOnePlayer, static_cast<CharacterBase::CharacterKind>(number));
	//�v���C���[�Ɏ����̃|�C���^�[��n���Ă���
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

	//��ڂ̕K�E�Z�̏��ݒ�
	status.firstSpecialAttackData.name = statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialName)];
	status.firstSpecialAttackData.path = statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialEffectName)];
	status.firstSpecialAttackData.cost = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialCost)]);
	status.firstSpecialAttackData.damageRate = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialDamageRate)]);
	status.firstSpecialAttackData.radius = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialRadius)]);
	status.firstSpecialAttackData.startFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialStartFrame)]);
	status.firstSpecialAttackData.endFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialEndFrame)]);
	status.firstSpecialAttackData.kind = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetSpecialAttackKind(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialKind)]);

	//��ڂ̕K�E�Z�̏��ݒ�
	status.secondSpecialAttackData.name = statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialName)];
	status.secondSpecialAttackData.path = statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialEffectName)];
	status.secondSpecialAttackData.cost = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialCost)]);
	status.secondSpecialAttackData.damageRate = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialDamageRate)]);
	status.secondSpecialAttackData.radius = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialRadius)]);
	status.secondSpecialAttackData.startFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialStartFrame)]);
	status.secondSpecialAttackData.endFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialEndFrame)]);
	status.secondSpecialAttackData.kind = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetSpecialAttackKind(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialKind)]);

	//�v���C���[�̃X�e�[�^�X��ݒ肷��
	m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->SetStatus(status);
	m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->SetCharacterNumber(CharacterBase::PlayerNumber::kOnePlayer);

}

void GameManager::SetEnemyStatus(int number, std::vector<std::string> statusData)
{
	//�G�l�~�[�쐬
	m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)] = std::make_shared<CharacterBase>(ObjectTag::kTwoPlayer, static_cast<CharacterBase::CharacterKind>(number));
	//�G�l�~�[�Ɏ����̃|�C���^�[��n���Ă���
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

	//��ڂ̕K�E�Z�̏��ݒ�
	status.firstSpecialAttackData.name = statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialName)];
	status.firstSpecialAttackData.cost = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialCost)]);
	status.firstSpecialAttackData.damageRate = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialDamageRate)]);
	status.firstSpecialAttackData.startFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialStartFrame)]);
	status.firstSpecialAttackData.endFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialEndFrame)]);
	status.firstSpecialAttackData.kind = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->GetSpecialAttackKind(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kFirstSpecialKind)]);

	//��ڂ̕K�E�Z�̏��ݒ�
	status.secondSpecialAttackData.name = statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialCost)];
	status.secondSpecialAttackData.cost = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialCost)]);
	status.secondSpecialAttackData.damageRate = stof(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialDamageRate)]);
	status.secondSpecialAttackData.startFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialStartFrame)]);
	status.secondSpecialAttackData.endFrame = stoi(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialEndFrame)]);
	status.secondSpecialAttackData.kind = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->GetSpecialAttackKind(statusData[static_cast<int>(CharacterBase::CharacterStatusDataSort::kSecondSpecialKind)]);

	//�G�l�~�[�̃X�e�[�^�X��ݒ肷��
	m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->SetStatus(status);
	m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->SetCharacterNumber(CharacterBase::PlayerNumber::kTwoPlayer);
}

MyEngine::Vector3 GameManager::GetTargetPos(std::shared_ptr<CharacterBase> character)
{
	if (character == m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)])
	{
		return m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->GetPos();
	}
	else if (character == m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)])
	{
		return m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetPos();
	}
	else
	{
		return MyEngine::Vector3(0, 0, 0);
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

void GameManager::StopCameraCorrection()
{
	m_pCamera->StopCamera();
}

void GameManager::StopCamera(int time)
{
	m_pCamera->StopCamera(time);
}

void GameManager::StartCameraCorrection()
{
	m_pCamera->StartMoveCamera();
}

void GameManager::ShakeCamera(int time)
{
	m_pCamera->ShakeCamera(time);
}

void GameManager::SwayCamera()
{
	m_pCamera->SwayCamera();
}

void GameManager::EntryEffect(std::shared_ptr<Effect> effect)
{
	m_pEffectManager->Entry(effect, effect->GetPos());
}

void GameManager::ExitEffect(std::shared_ptr<Effect> effect)
{
	m_pEffectManager->Exit(effect);
}

void GameManager::UpdateStart()
{
	//�J�����̍��W�ݒ�
	MyEngine::Vector3 onePlayerPos = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetPos();
	MyEngine::Vector3 twoPlayerPos = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->GetPos();

	//1P�̉��o��
	if (m_situation == Situation::kStart1P)
	{
		//�J���������ڂ��班�����񂹂Ă���
		MyEngine::Vector3 cameraVelo = kStartCameraEndPos - m_poseCameraPos;

		//�J�����̌��݂̍��W�ƃJ�����̏I�_�̍��W�̋��������ȏ゠���
		if ((kStartCameraEndPos - m_poseCameraPos).Length() > kStartCameraMoveSpeed)
		{
			cameraVelo = cameraVelo.Normalize() * kStartCameraMoveSpeed;
		}
		//��苗�������߂����
		else
		{
			cameraVelo = cameraVelo.Normalize() * (kStartCameraEndPos - m_poseCameraPos).Length();
		}

		//���W���X�V����
		m_poseCameraPos += cameraVelo;

		//1P�𒆐S��1P���^�[�Q�b�g�ɂ���
		m_pCamera->SetCenterAndTarget(onePlayerPos, onePlayerPos);

		m_pCamera->SetLocalPos(m_poseCameraPos);

		m_pCamera->SetFrontPos(twoPlayerPos);
		//�A�j���[�V�������I��������
		if (m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->IsEndAnim())
		{
			//�A�j���[�V�������I�����Ă�����^�C���𐔂���
			m_time++;
		}
		if (m_time > kStartPoseTime)
		{
			m_time = 0;
			m_situation = Situation::kStart2P;
			//X���W�������]�������̂��J�����̊J�n�ʒu�ɂ���
			MyEngine::Vector3 pos = kStartCameraStartPos;
			pos.x *= -1;
			m_poseCameraPos = pos;
		}
	}
	//2P�̉��o��
	else if (m_situation == Situation::kStart2P)
	{
		MyEngine::Vector3 startPos = kStartCameraEndPos;
		startPos.x *= -1;

		//�J���������ڂ��班�����񂹂Ă���
		MyEngine::Vector3 cameraVelo = startPos - m_poseCameraPos;

		//�J�����̌��݂̍��W�ƃJ�����̏I�_�̍��W�̋��������ȏ゠���
		if ((startPos - m_poseCameraPos).Length() > kStartCameraMoveSpeed)
		{
			cameraVelo = cameraVelo.Normalize() * kStartCameraMoveSpeed;
		}
		//��苗�������߂����
		else
		{
			cameraVelo = cameraVelo.Normalize() * (startPos - m_poseCameraPos).Length();
		}

		//���W���X�V����
		m_poseCameraPos += cameraVelo;


		//1P�𒆐S��1P���^�[�Q�b�g�ɂ���
		m_pCamera->SetCenterAndTarget(twoPlayerPos, twoPlayerPos);

		m_pCamera->SetLocalPos(m_poseCameraPos);

		m_pCamera->SetFrontPos(onePlayerPos);
		//�A�j���[�V�������I��������
		if (m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->IsEndAnim())
		{
			//�A�j���[�V�������I�����Ă�����^�C���𐔂���
			m_time++;
		}
		if (m_time > kStartPoseTime)
		{
			m_time = 0;
			m_situation = Situation::kBattle;
			m_updateSituationFunc = &GameManager::UpdateBattle;
			m_pCamera->SetCenterAndTarget(onePlayerPos, twoPlayerPos);
			m_pCamera->SetFrontPos(twoPlayerPos);
			m_pCamera->SetBattleCamera();
		}
	}

}

void GameManager::UpdateBattle()
{
	//�L�����N�^�[�ǂ��炩�̗̑͂�0�ɂȂ��Ă�����Q�[�����I������t���O�𗧂Ă�
	for (auto item : m_pCharacters)
	{
		if (item->GetHp() <= 0)
		{

			//�J�����̐ݒ�������ň�x�����s��

			//���[�J�����W��ݒ肷��
			m_pCamera->SetLocalPos(kKnockOutCameraPos);

			//���S���W
			MyEngine::Vector3 centerPos;
			//�������L�����N�^�[���珟�����L�����N�^�[�ւ̃x�N�g��
			MyEngine::Vector3 loserToWinnerVec;
			//�J�����̐��ʕ����Ƃ�����W
			MyEngine::Vector3 frontPos;

			int onePlayer = static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer);
			int twoPlayer = static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer);

			//�����Ă�����𒆐S�Ƃ���
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

			//�^�[�Q�b�g���W
			MyEngine::Vector3 targetPos = (loserToWinnerVec.Normalize() * kKnockOutCameraTargetDistance) + centerPos;

			//���S���W�ƃJ�����̃^�[�Q�b�g���W��ݒ�
			m_pCamera->SetCenterAndTarget(centerPos, targetPos);

			//���ʍ��W��ݒ�
			m_pCamera->SetFrontPos(frontPos);

			m_updateSituationFunc = &GameManager::UpdateKnockOut;
			m_situation = Situation::kKnockOut;
			return;
		}
	}

	//�J�����̍X�V
	MyEngine::Vector3 onePlayerPos = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetPos();
	MyEngine::Vector3 twoPlayerPos = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kTwoPlayer)]->GetPos();

	MyEngine::Vector3 cameraTargetPos = twoPlayerPos;

	cameraTargetPos.y = twoPlayerPos.y;

	m_pCamera->SetCenterAndTarget(onePlayerPos, cameraTargetPos);

	//1P����2P�ւ̃x�N�g��
	MyEngine::Vector3 playerToTarget = (twoPlayerPos - onePlayerPos).Normalize();

	//�J�����̐��ʕ�����ݒ�
	m_pCamera->SetFrontPos(onePlayerPos + playerToTarget);

	//�J�����Ƀv���C���[�̃x���V�e�B��ݒ肷��
	m_pCamera->SetPlayerVelo(m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetVelo());
}

void GameManager::UpdateKnockOut()
{
	//�G�t�F�N�g�̍Đ����x��x������
	m_pEffectManager->SetEffectPlaySpeed(0.1f);
	//���o���s���Ă��鎞�Ԃ��v��
	m_time++;

	//��莞�ԉ��o��������t�F�[�h�C������
	if (m_time > kKnockOutTime)
	{
		m_alpha += kFadeSpeed;
		m_alpha = min(m_alpha, 255);

	}
	//�t�F�[�h�C�����I�������
	if (m_alpha >= 255)
	{
		//�A�b�v�f�[�g��ύX
		m_updateSituationFunc = &GameManager::UpdateResult;
		//�󋵂����U���g�ɕύX
		m_situation = Situation::kResult;
		//���Ԍv�������Z�b�g
		m_time = 0;
		//�J�����̃A�b�v�f�[�g��ύX
		m_pCamera->SetPoseCamera();
	}
}

void GameManager::UpdateResult()
{

	//�t�F�[�h�C�����Ă���
	if (m_alpha > 0)
	{
		m_alpha -= kFadeSpeed;
		m_pEffectManager->DeletePlayEffect();

		//�J�����̍��W�����U���g�̏������W�ɕύX
		m_poseCameraPos = kResultCameraStartPos;
		//�J�����ɍ��W�ݒ�
		m_pCamera->SetLocalPos(m_poseCameraPos);
		//1P�̍��W
		MyEngine::Vector3 onePlayerPos = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetPos();
		//�J�����̒��S���W�ƃ^�[�Q�b�g���W��ݒ�
		m_pCamera->SetCenterAndTarget(onePlayerPos, onePlayerPos);
		//���ʍ��W
		MyEngine::Vector3 frontPos = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetFrontPos();
		//���ʍ��W�̍������v���C���[�̍����ɍ��킹��
		frontPos.y = onePlayerPos.y;
		//�J�����̐��ʍ��W��ݒ�
		m_pCamera->SetFrontPos(frontPos);
	}
	//�t�F�[�h�A�E�g���I�������
	else
	{
		//�J�������񂹂Ă���
		MyEngine::Vector3 cameraVelo = kResultCameraEndPos - m_poseCameraPos;

		//�J�����̌��݂̍��W�ƃJ�����̏I�_�̍��W�̋��������ȏ゠���
		if ((kResultCameraEndPos - m_poseCameraPos).Length() > kResultCameraMoveSpeed)
		{
			cameraVelo = cameraVelo.Normalize() * kResultCameraMoveSpeed;
		}
		//��苗�������߂����
		else
		{
			cameraVelo = cameraVelo.Normalize() * (kResultCameraEndPos - m_poseCameraPos).Length();
		}
		//1P�̍��W
		MyEngine::Vector3 onePlayerPos = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetPos();
		//���ʍ��W
		MyEngine::Vector3 frontPos = m_pCharacters[static_cast<int>(CharacterBase::PlayerNumber::kOnePlayer)]->GetFrontPos();
		//���ʍ��W�̍������v���C���[�̍����ɍ��킹��
		frontPos.y = onePlayerPos.y;
		//�J�����̐��ʍ��W��ݒ�
		m_pCamera->SetFrontPos(frontPos);
		//���W���X�V����
		m_poseCameraPos += cameraVelo;

		m_pCamera->SetLocalPos(m_poseCameraPos);
	}

	int selectNum = m_pGameUi->UpdateResult();

	if (selectNum == static_cast<int>(GameUi::SelectItem::kRetry))
	{
		//���g���C�������쐬����
	}
	else if (selectNum == static_cast<int>(GameUi::SelectItem::kCharacterSelect))
	{
		m_isChangeScene = true;
	}
	else if (selectNum == static_cast<int>(GameUi::SelectItem::kTitle))
	{

	}


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
		return MyEngine::Vector3(0, 0, 0);
	}
}