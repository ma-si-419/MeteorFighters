#include "TutorialManager.h"
#include "TutorialUi.h"
#include "GameUi.h"
#include "LoadCsv.h"
#include "GameCamera.h"
#include "Attack.h"
#include "EffectManager.h"
#include "Stage.h"
#include "Input.h"
#include "EnemyInput.h"

namespace
{
	//�`���[�g���A�����N���A�������
	const std::vector<TutorialManager::TutorialSuccessKind> kTutorialSuccessTerms[static_cast<int>(TutorialManager::TutorialKind::kTutorialNum)] =
	{
		{TutorialManager::TutorialSuccessKind::kMove},
		{TutorialManager::TutorialSuccessKind::kStep},
		{TutorialManager::TutorialSuccessKind::kJump},
		{TutorialManager::TutorialSuccessKind::kUp,TutorialManager::TutorialSuccessKind::kDown },
		{TutorialManager::TutorialSuccessKind::kPhysicalAttack},
		{TutorialManager::TutorialSuccessKind::kChargePhysicalAttack},
		{TutorialManager::TutorialSuccessKind::kEnergyCharge},
		{TutorialManager::TutorialSuccessKind::kEnergyAttack},
		{TutorialManager::TutorialSuccessKind::kChargeEnergyAttack},
		{TutorialManager::TutorialSuccessKind::kGuard},
		{TutorialManager::TutorialSuccessKind::kSpecialAttack}
	};
}

TutorialManager::TutorialManager(std::shared_ptr<GameCamera> camera) :
	GameManagerBase(camera, GameManagerBase::GameKind::kTutorial),
	m_nowTutorial(TutorialKind::kMove),
	m_drawSituationFunc(&TutorialManager::DrawMenu),
	m_updateSituationFunc(&TutorialManager::UpdateMenu),
	m_tutorialSituation(TutorialSituation::kMenu)
{
	m_pTutorialUi = std::make_shared<TutorialUi>();

	m_pGameUi = std::make_shared<GameUi>();
}

TutorialManager::~TutorialManager()
{
}

void TutorialManager::Init()
{
	//�J�����̏�����
	m_pCamera->SetBattleCamera();
	
	//�`���[�g���A��UI�̏�����
	m_pTutorialUi->Init();

	//situation�̕ύX
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

void TutorialManager::UpdateMenu()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	TutorialUi::MenuItem selectItem = m_pTutorialUi->GetSelectItem();

	if (input->IsTrigger("A"))
	{
		bool isEnd = false;

		//�󋵂����Z�b�g����������ꂽ��
		if (selectItem == TutorialUi::MenuItem::kReset)
		{
			RetryInit();
			m_nowTutorial = static_cast<TutorialKind>(m_pTutorialUi->GetTutorialNumber());
			isEnd = true;
		}
		//���j���[����邪�����ꂽ��
		else if (selectItem == TutorialUi::MenuItem::kMenuEnd)
		{
			isEnd = true;
			//���s���Ă���`���[�g���A���ƑI�����Ă���`���[�g���A�����قȂ��Ă�����
			if (m_nowTutorial != static_cast<TutorialKind>(m_pTutorialUi->GetTutorialNumber()))
			{
				m_nowTutorial = static_cast<TutorialKind>(m_pTutorialUi->GetTutorialNumber());
				RetryInit();
			}
		}
		//�`���[�g���A�����I�����邪�����ꂽ��
		else if (selectItem == TutorialUi::MenuItem::kTutorialEnd)
		{
			//���C�����j���[�ɖ߂�
			m_nextScene = Game::Scene::kMenu;
		}

		//���j���[��ʂ����I�������I�΂�Ă�����
		if (isEnd)
		{
			ChangeSituation(TutorialSituation::kStart);
		}
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

	if (input->IsTrigger("Pause"))
	{
		ChangeSituation(TutorialSituation::kMenu);
	}

	//�`���[�g���A���������������ǂ���
	bool isSuccess = true;

	auto& terms = kTutorialSuccessTerms[static_cast<int>(m_nowTutorial)];

	for (auto item : terms)
	{
		//�����ŃN���A���Ă��邩���m�F
		if (!m_successTutorialKinds[item])
		{
			//��ł��N���A���Ă��Ȃ����ڂ������false�ɂ���
			isSuccess = false;
		}
	}

	//�N���A�������ǂ�����Ui�ɑ���
	m_pTutorialUi->SetSuccessTutorial(isSuccess);

	//�N���A���Ă�����`�揈����ύX����
	if (isSuccess)
	{
		ChangeSituation(TutorialSituation::kSuccess);
	}

}

void TutorialManager::UpdateSuccess()
{
	//�N���A���o���I����Ă����玟�̃��j���[�Ɉڂ�
	if (m_pTutorialUi->IsSuccessEnd())
	{
		m_nowTutorial = static_cast<TutorialKind>(static_cast<int>(m_nowTutorial) + 1);

		for (auto& item : m_successTutorialKinds)
		{
			item.second = false;
		}

		ChangeSituation(TutorialSituation::kStart);
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

void TutorialManager::DrawSuccess()
{
	m_pTutorialUi->DrawSuccess(static_cast<int>(m_nowTutorial));
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
		m_pTutorialUi->SetNowTutorialNumber(static_cast<int>(m_nowTutorial));
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
		for (auto& player : m_pCharacters)
		{
			player->LookTarget();
			player->ChangeSituationUpdate(static_cast<int>(BattleSituation::kBattle));
		}
		//���������s��
		m_pTutorialUi->InitPlaying(static_cast<int>(m_nowTutorial));
		//�X�V�����̕ύX
		m_updateSituationFunc = &TutorialManager::UpdatePlaying;
		//�`�揈���̕ύX
		m_drawSituationFunc = &TutorialManager::DrawPlaying;
	}
	else if (next == TutorialSituation::kSuccess)
	{
		//�v���C���[�̏󋵂�ύX
		for (auto& player : m_pCharacters) player->ChangeSituationUpdate(static_cast<int>(BattleSituation::kBattle));

		//���������s��
		m_pTutorialUi->InitSuccess();
		//�X�V�����̕ύX
		m_updateSituationFunc = &TutorialManager::UpdateSuccess;
		//�`�揈����ύX
		m_drawSituationFunc = &TutorialManager::DrawSuccess;

	}

}
