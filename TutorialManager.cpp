#include "TutorialManager.h"
#include "TutorialUi.h"
#include "LoadCsv.h"
#include "GameCamera.h"
#include "Attack.h"
#include "EffectManager.h"
#include "Stage.h"
#include "Input.h"

TutorialManager::TutorialManager(std::shared_ptr<GameCamera> camera) :
	GameManagerBase(camera),
	m_isSuccessTutorial(false),
	m_nowTutorial(TutorialKind::kMove),
	m_drawSituationFunc(&TutorialManager::DrawMenu),
	m_updateSituationFunc(&TutorialManager::UpdateMenu),
	m_tutorialSituation(TutorialSituation::kMenu)

{
	m_pTutorialUi = std::make_shared<TutorialUi>();
}

TutorialManager::~TutorialManager()
{
}

void TutorialManager::Init()
{
	m_pCamera->SetBattleCamera();
	m_pTutorialUi->Init();

	ChangeSituation(TutorialSituation::kMenu);
}

void TutorialManager::Update()
{
	(this->*m_updateSituationFunc)();

	//UI�̃A�b�v�f�[�g
	m_pTutorialUi->Update();


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

	//�Q�[���V�[�����ʂ̍X�V
	UpdateCommon();
}

void TutorialManager::Draw()
{
	//���ʂ̕`�揈��
	DrawCommon();

	(this->*m_drawSituationFunc)();
}

void TutorialManager::Final()
{
}

void TutorialManager::UpdateMenu()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	if (input->IsTrigger("A"))
	{
		ChangeSituation(TutorialSituation::kStart);
	}
}

void TutorialManager::UpdateStart()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	if (input->IsTrigger("A"))
	{
		ChangeSituation(TutorialSituation::kPlaying);
	}
}

void TutorialManager::UpdatePlaying()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	if (input->IsTrigger("A"))
	{
		ChangeSituation(TutorialSituation::kMenu);
		m_nowTutorial = static_cast<TutorialKind>(static_cast<int>(m_nowTutorial) + 1);
	}
}

void TutorialManager::DrawMenu()
{
	m_pTutorialUi->DrawMenu();
}

void TutorialManager::DrawStart()
{
	m_pTutorialUi->DrawStart(static_cast<int>(m_nowTutorial));
}

void TutorialManager::DrawPlaying()
{
	m_pTutorialUi->DrawPlaying(static_cast<int>(m_nowTutorial));
}

void TutorialManager::ChangeSituation(TutorialSituation next)
{
	//�󋵂�ύX
	m_tutorialSituation = next;

	//���j���[��
	if (next == TutorialSituation::kMenu)
	{
		//�v���C���[�̏󋵂�ύX
		for (auto& player : m_pCharacters) player->ChangeSituationUpdate(static_cast<int>(BattleSituation::kMenu));

		//���������s��
		m_pTutorialUi->InitMenu();
		//�X�V�����̕ύX
		m_updateSituationFunc = &TutorialManager::UpdateMenu;
		//�`�揈���̕ύX
		m_drawSituationFunc = &TutorialManager::DrawMenu;
	}
	//�J�n��
	else if (next == TutorialSituation::kStart)
	{
		//�v���C���[�̏󋵂�ύX
		for (auto& player : m_pCharacters) player->ChangeSituationUpdate(static_cast<int>(BattleSituation::kMenu));

		//���������s��
		m_pTutorialUi->InitStart();
		//�X�V�����̕ύX
		m_updateSituationFunc = &TutorialManager::UpdateStart;
		//�`�揈���̕ύX
		m_drawSituationFunc = &TutorialManager::DrawStart;
	}
	//�v���C��
	else if (next == TutorialSituation::kPlaying)
	{
		//�v���C���[�̏󋵂�ύX
		for (auto& player : m_pCharacters) player->ChangeSituationUpdate(static_cast<int>(BattleSituation::kBattle));

		//���������s��
		m_pTutorialUi->InitPlaying(static_cast<int>(m_nowTutorial));
		//�X�V�����̕ύX
		m_updateSituationFunc = &TutorialManager::UpdatePlaying;
		//�`�揈���̕ύX
		m_drawSituationFunc = &TutorialManager::DrawPlaying;
	}

}
