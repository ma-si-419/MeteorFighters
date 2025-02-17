#include "BattleManager.h"
#include "BattleUi.h"
#include "Attack.h"
#include "LoadCsv.h"
#include "Stage.h"
#include "GameCamera.h"
#include "Character.h"
#include "CharacterStateBase.h"
#include "Effectmanager.h"
#include "Effect.h"
#include "GameUi.h"
#include "Game.h"
#include <cmath>
#include "Physics.h"
#include "Input.h"
#include "SoundManager.h"

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
	constexpr int kKnockOutFadeSpeed = 5;

	//���g���C���̃t�F�[�h�̑��x
	constexpr int kRetryFadeSpeed = 25;

	//�o�g���I�����̉��o�̃J�����X�^�[�g���W
	const MyEngine::Vector3 kResultCameraStartPos(0.0f, 50.0f, 50.0f);
	const MyEngine::Vector3 kResultCameraEndPos(0.0f, 8.0f, 18.0f);

	//�o�g���I�����̃J�����̈ړ����x
	constexpr float kResultCameraMoveSpeed = 1.0f;

	//Bgm�̉���
	constexpr int kBgmVolume = 220;

	//KnockOut�̉���
	constexpr int kKnockOutVolume = 255;
}

BattleManager::BattleManager(std::shared_ptr<GameCamera> camera) :
	GameManagerBase(camera, GameManagerBase::GameKind::kBattle),
	m_bgmName("empty"),
	m_menuSelectNumber(0)
{
	m_pCamera = camera;

	m_pGameUi = std::make_shared<GameUi>();

	m_pBattleUi = std::make_shared<BattleUi>();

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

	//BGM���Đ�����
	SoundManager::GetInstance().PlayLoopSound(m_bgmName);

#ifdef _DEBUG
	//	m_situation = Situation::kBattle;
	//	m_updateSituationFunc = &BattleManager::UpdateBattle;
#endif // _DEBUG
}

void BattleManager::Update()
{

	//�󋵂ɂ���ĕς��Update
	(this->*m_updateSituationFunc)();

	//Ui�̍X�V
	m_pBattleUi->Update();

	//�Q�[���V�[�����ʂ̍X�V
	UpdateCommon();

}

void BattleManager::ChangeSituation(BattleSituation situation)
{
	//�󋵂�ύX
	m_situation = situation;

	//�v���C���[�̏󋵂�ύX
	for (auto& player : m_pCharacters) player->ChangeSituationUpdate(static_cast<int>(situation));

	//���g���C��
	if (situation == BattleSituation::kRetry)
	{
		m_updateSituationFunc = &BattleManager::UpdateRetry;
	}
	//�J�n��
	else if (situation == BattleSituation::kStart1P || situation == BattleSituation::kStart2P)
	{
		m_updateSituationFunc = &BattleManager::UpdateStart;

		m_pCamera->SetPoseCamera();
		m_poseCameraPos = kStartCameraStartPos;
	}
	//�o�g����
	else if (situation == BattleSituation::kBattle)
	{
		m_updateSituationFunc = &BattleManager::UpdateBattle;
		m_pBattleUi->ChangeSituation(BattleUi::UiSituation::kNone);
		Physics::GetInstance().StartUpdate();
	}
	//�m�b�N�A�E�g��
	else if (situation == BattleSituation::kKnockOut)
	{
		m_updateSituationFunc = &BattleManager::UpdateKnockOut;
		m_pBattleUi->ChangeSituation(BattleUi::UiSituation::kNone);
	}
	//���U���g��
	else if (situation == BattleSituation::kResult)
	{
		m_updateSituationFunc = &BattleManager::UpdateResult;
		m_pBattleUi->ChangeSituation(BattleUi::UiSituation::kResult);
	}
	//���j���[��
	else if (situation == BattleSituation::kMenu)
	{
		m_updateSituationFunc = &BattleManager::UpdateMenu;
		m_pBattleUi->ChangeSituation(BattleUi::UiSituation::kMenu);
		Physics::GetInstance().StopUpdate();
	}
}

void BattleManager::Draw()
{
	//���ʂ̕`�揈��
	DrawCommon();

	//�t�F�[�h�̃A���t�@�l��0���������ꍇ�\������
	if (m_alpha > 0)
	{
		m_pGameUi->DrawFade(GetColor(255, 255, 255), m_alpha);
	}

	//�o�g������Ui�̕`��
	m_pBattleUi->Draw();
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

	//BGM���~�߂�
	SoundManager::GetInstance().StopLoopSound(m_bgmName);
}

void BattleManager::UpdateStart()
{
	m_alpha -= kRetryFadeSpeed;

	m_alpha = max(m_alpha, 0);

	//�t�F�[�h�C�����I����ăA�b�v�f�[�g���s��Ȃ�
	if (m_alpha > 0) return;

	//�J�����̍��W�ݒ�
	MyEngine::Vector3 onePlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetPos();
	MyEngine::Vector3 twoPlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetPos();

	//1P�̉��o��
	if (m_situation == BattleSituation::kStart1P)
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
		if (m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->IsEndAnim())
		{
			//�A�j���[�V�������I�����Ă�����^�C���𐔂���
			m_time++;
		}
		if (m_time > kStartPoseTime)
		{
			m_time = 0;
			//X���W�������]�������̂��J�����̊J�n�ʒu�ɂ���
			MyEngine::Vector3 pos = kStartCameraStartPos;
			pos.x *= -1;
			m_poseCameraPos = pos;

			ChangeSituation(BattleSituation::kStart2P);
		}
	}
	//2P�̉��o��
	else if (m_situation == BattleSituation::kStart2P)
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
		if (m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->IsEndAnim())
		{
			//�A�j���[�V�������I�����Ă�����^�C���𐔂���
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

	//�J�����̍X�V
	m_pCamera->Update();
}

void BattleManager::UpdateBattle()
{
	for (auto item : m_pCharacters)
	{
		//�L�����N�^�[�ǂ��炩�̗̑͂�0�ɂȂ��Ă�����Q�[�����I������t���O�𗧂Ă�
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

			int onePlayer = static_cast<int>(Character::PlayerNumber::kOnePlayer);
			int twoPlayer = static_cast<int>(Character::PlayerNumber::kTwoPlayer);

			//�����Ă�����𒆐S�Ƃ���

			//1P���������ꍇ
			if (m_pCharacters[onePlayer]->GetHp() <= 0)
			{
				centerPos = m_pCharacters[onePlayer]->GetPos();
				loserToWinnerVec = m_pCharacters[twoPlayer]->GetPos() - m_pCharacters[onePlayer]->GetPos();
				frontPos = m_pCharacters[twoPlayer]->GetPos();

				//1P���������Ɛݒ肷��
				m_pBattleUi->SetResult(false);
			}
			//1P���������ꍇ
			else
			{
				centerPos = m_pCharacters[twoPlayer]->GetPos();
				loserToWinnerVec = m_pCharacters[onePlayer]->GetPos() - m_pCharacters[twoPlayer]->GetPos();
				frontPos = m_pCharacters[onePlayer]->GetPos();

				//1P���������Ɛݒ肷��
				m_pBattleUi->SetResult(true);
			}

			//�^�[�Q�b�g���W
			MyEngine::Vector3 targetPos = (loserToWinnerVec.Normalize() * kKnockOutCameraTargetDistance) + centerPos;

			//���S���W�ƃJ�����̃^�[�Q�b�g���W��ݒ�
			m_pCamera->SetCenterAndTarget(centerPos, targetPos);

			//���ʍ��W��ݒ�
			m_pCamera->SetFrontPos(frontPos);

			m_pCamera->SetPoseCamera();

			//�m�b�N�A�E�g���̃T�E���h���Đ�
			SoundManager::GetInstance().PlayOnceSound("KnockOut");

			ChangeSituation(BattleSituation::kKnockOut);
			return;
		}
	}

	//�J�����̍X�V
	MyEngine::Vector3 onePlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetPos();
	MyEngine::Vector3 twoPlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetPos();

	MyEngine::Vector3 cameraTargetPos = twoPlayerPos;

	cameraTargetPos.y = twoPlayerPos.y;

	m_pCamera->SetCenterAndTarget(onePlayerPos, cameraTargetPos);

	//1P����2P�ւ̃x�N�g��
	MyEngine::Vector3 playerToTarget = (twoPlayerPos - onePlayerPos).Normalize();

	//�J�����̐��ʕ�����ݒ�
	m_pCamera->SetFrontPos(onePlayerPos + playerToTarget);

	//�J�����Ƀv���C���[�̃x���V�e�B��ݒ肷��
	m_pCamera->SetPlayerVelo(m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetVelo());

	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//�{�^���A�ŏ�Ԃ���Ȃ����Ƀ{�^���������ꂽ��
	if (input->IsTrigger("Pause") && !m_isButtonBashing)
	{
		ChangeSituation(BattleSituation::kMenu);
	}

	//�J�����̍X�V
	m_pCamera->Update();
}

void BattleManager::UpdateKnockOut()
{
	//�G�t�F�N�g�̍Đ����x��x������
	m_pEffectManager->SetEffectPlaySpeed(0.1f);
	//���o���s���Ă��鎞�Ԃ��v��
	m_time++;

	//��莞�ԉ��o��������t�F�[�h�C������
	if (m_time > kKnockOutTime)
	{
		m_alpha += kKnockOutFadeSpeed;
		m_alpha = min(m_alpha, 255);

	}
	//�t�F�[�h�C�����I�������
	if (m_alpha >= 255)
	{
		//�󋵂�ύX
		ChangeSituation(BattleSituation::kResult);
		//���Ԍv�������Z�b�g
		m_time = 0;
		//�J�����̃A�b�v�f�[�g��ύX
		m_pCamera->SetPoseCamera();
	}

	//�J�����̍X�V
	m_pCamera->Update();
}

void BattleManager::UpdateResult()
{
	//�t�F�[�h�C�����Ă���
	if (m_alpha > 0)
	{
		m_alpha -= kKnockOutFadeSpeed;
		m_pEffectManager->DeletePlayEffect();

		//�J�����̍��W�����U���g�̏������W�ɕύX
		m_poseCameraPos = kResultCameraStartPos;
		//�J�����ɍ��W�ݒ�
		m_pCamera->SetLocalPos(m_poseCameraPos);
		//1P�̍��W
		MyEngine::Vector3 onePlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetPos();
		//�J�����̒��S���W�ƃ^�[�Q�b�g���W��ݒ�
		m_pCamera->SetCenterAndTarget(onePlayerPos, onePlayerPos);
		//���ʍ��W
		MyEngine::Vector3 frontPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetFrontPos();
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
		
			//�����{�^���������ꂽ��
			if (MyEngine::Input::GetInstance().GetInputData(0)->IsAnyPress())
			{
				//Ui�̏󋵂�ύX
				m_pBattleUi->SetResultMenuDisplay(true);
			}
		}
		//1P�̍��W
		MyEngine::Vector3 onePlayerPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetPos();
		//���ʍ��W
		MyEngine::Vector3 frontPos = m_pCharacters[static_cast<int>(Character::PlayerNumber::kOnePlayer)]->GetFrontPos();
		//���ʍ��W�̍������v���C���[�̍����ɍ��킹��
		frontPos.y = onePlayerPos.y;
		//�J�����̐��ʍ��W��ݒ�
		m_pCamera->SetFrontPos(frontPos);
		//���W���X�V����
		m_poseCameraPos += cameraVelo;

		m_pCamera->SetLocalPos(m_poseCameraPos);
	}

	int selectNum = m_pBattleUi->GetDecisionItem();

	if (selectNum == static_cast<int>(BattleUi::ResultItem::kRetry))
	{
		//���g���C����
		ChangeSituation(BattleSituation::kRetry);
		m_pBattleUi->ChangeSituation(BattleUi::UiSituation::kNone);
	}
	else if (selectNum == static_cast<int>(BattleUi::ResultItem::kCharacterSelect))
	{
		m_nextScene = Game::Scene::kSelect;
	}
	else if (selectNum == static_cast<int>(BattleUi::ResultItem::kMenu))
	{
		m_nextScene = Game::Scene::kMenu;
	}

	//�J�����̍X�V
	m_pCamera->Update();
}

void BattleManager::UpdateRetry()
{
	//�����t�F�[�h�A�E�g���Ă����ɂ�����x�t�F�[�h�C������
	m_alpha += kRetryFadeSpeed;

	m_alpha = min(m_alpha, 255);

	//�t�F�[�h�A�E�g���I�������
	if (m_alpha == 255)
	{
		//�󋵂�ύX
		ChangeSituation(BattleSituation::kStart1P);
		//���������s��
		RetryInit();
	}

	//�J�����̍X�V
	m_pCamera->Update();
}

void BattleManager::UpdateMenu()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//B�{�^���������ꂽ��
	if (input->IsTrigger("B"))
	{
		ChangeSituation(BattleSituation::kBattle);
		m_pBattleUi->ChangeSituation(BattleUi::UiSituation::kNone);
		Physics::GetInstance().StartUpdate();

		//�T�E���h���Đ�
		SoundManager::GetInstance().PlayOnceSound("Cancel");
	}

	auto select = static_cast<BattleUi::MenuItem>(m_pBattleUi->GetDecisionItem());

	//�L�����N�^�[��I�тȂ���
	if (select == BattleUi::MenuItem::kBackCharacterSelect)
	{
		m_nextScene = Game::Scene::kSelect;
	}
	//�o�g���ɖ߂�
	else if (select == BattleUi::MenuItem::kReturnBattle)
	{
		ChangeSituation(BattleSituation::kBattle);
		m_pBattleUi->ChangeSituation(BattleUi::UiSituation::kNone);
	}
	//���j���[�ɖ߂�
	else if (select == BattleUi::MenuItem::kBackMenu)
	{
		m_nextScene = Game::Scene::kMenu;
	}
	//������x�ŏ�����키
	else if (select == BattleUi::MenuItem::kRetry)
	{
		//���g���C����
		ChangeSituation(BattleSituation::kRetry);
		m_pBattleUi->ChangeSituation(BattleUi::UiSituation::kNone);
	}
}
