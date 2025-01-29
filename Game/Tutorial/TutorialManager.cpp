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
	//string����SuccessKind�ɕϊ����邽�߂�map
	const std::map<std::string, TutorialManager::TutorialSuccessKind> kSuccessKindMap =
	{
		{"�ړ�",TutorialManager::TutorialSuccessKind::kMove},
		{"�W�����v",TutorialManager::TutorialSuccessKind::kJump},
		{"�X�e�b�v",TutorialManager::TutorialSuccessKind::kStep},
		{"�_�b�V��",TutorialManager::TutorialSuccessKind::kDash},
		{"�󒆑ҋ@",TutorialManager::TutorialSuccessKind::kSkyIdle},
		{"�㏸",TutorialManager::TutorialSuccessKind::kUp},
		{"���~",TutorialManager::TutorialSuccessKind::kDown},
		{"�i���U��1",TutorialManager::TutorialSuccessKind::kPhysicalAttack1},
		{"�i���U��2",TutorialManager::TutorialSuccessKind::kPhysicalAttack2},
		{"�i���U��3",TutorialManager::TutorialSuccessKind::kPhysicalAttack3},
		{"�i���U��4",TutorialManager::TutorialSuccessKind::kPhysicalAttack4},
		{"�i���U��5",TutorialManager::TutorialSuccessKind::kPhysicalAttack5},
		{"�i���U��6",TutorialManager::TutorialSuccessKind::kPhysicalAttack6},
		{"�i���U��7",TutorialManager::TutorialSuccessKind::kPhysicalAttack7},
		{"�i���U��8",TutorialManager::TutorialSuccessKind::kPhysicalAttack8},
		{"�X�}�b�V���i��",TutorialManager::TutorialSuccessKind::kChargePhysicalAttack},
		{"�ǂ��ł�",TutorialManager::TutorialSuccessKind::kChaseAttack},
		{"�C�̓`���[�W",TutorialManager::TutorialSuccessKind::kEnergyCharge},
		{"�C�e�U��",TutorialManager::TutorialSuccessKind::kEnergyAttack},
		{"�X�}�b�V���C�e",TutorialManager::TutorialSuccessKind::kChargeEnergyAttack},
		{"�K�E�Z",TutorialManager::TutorialSuccessKind::kSpecialAttack},
		{"�K�[�h",TutorialManager::TutorialSuccessKind::kGuard},
		{"�W���X�g�K�[�h",TutorialManager::TutorialSuccessKind::kJustGuard},
		{"�󂯐g",TutorialManager::TutorialSuccessKind::kFalls},
		{"���A",TutorialManager::TutorialSuccessKind::kReturn},
		{"�X�[�p�[�_�b�V��",TutorialManager::TutorialSuccessKind::kSuperDash},
		{"���P�b�g�_�b�V��",TutorialManager::TutorialSuccessKind::kRocketDash},
		{"���b�V���Ό�",TutorialManager::TutorialSuccessKind::kButtonBashing},
		{"�h���U����",TutorialManager::TutorialSuccessKind::kUpperAttack},
		{"�h���U����",TutorialManager::TutorialSuccessKind::kDekaKick},
		{"�h���U����",TutorialManager::TutorialSuccessKind::kCycloneKick},
		{"��K�[�h",TutorialManager::TutorialSuccessKind::kUpGuard},
		{"���K�[�h",TutorialManager::TutorialSuccessKind::kDownGuard}
	};

	//string����TutorialKind�ɕϊ����邽�߂�map
	const std::map<std::string, TutorialManager::TutorialKind> kTutorialKindMap =
	{
		{"�ړ�",TutorialManager::TutorialKind::kMove},
		{"�X�e�b�v",TutorialManager::TutorialKind::kStep},
		{"�_�b�V��",TutorialManager::TutorialKind::kDash},
		{"�󒆈ړ�",TutorialManager::TutorialKind::kSkyMove},
		{"���b�V���i��",TutorialManager::TutorialKind::kPhysicalAttack},
		{"�X�}�b�V���i��",TutorialManager::TutorialKind::kChargePhysicalAttack},
		{"�ǂ��ł�",TutorialManager::TutorialKind::kChaseAttack},
		{"�C�̓`���[�W",TutorialManager::TutorialKind::kEnergyCharge},
		{"���b�V���C�e",TutorialManager::TutorialKind::kEnergyAttack},
		{"�X�}�b�V���C�e",TutorialManager::TutorialKind::kChargeEnergyAttack},
		{"�K�[�h",TutorialManager::TutorialKind::kGuard},
		{"�K�E�Z",TutorialManager::TutorialKind::kSpecialAttack},
		{"�������",TutorialManager::TutorialKind::kJustGuard},
		{"�󂯐g",TutorialManager::TutorialKind::kFalls},
		{"���A",TutorialManager::TutorialKind::kReturn},
		{"�X�[�p�[�_�b�V��",TutorialManager::TutorialKind::kSuperDash},
		{"���P�b�g�_�b�V��",TutorialManager::TutorialKind::kRocketDash},
		{"���b�V���Ό�",TutorialManager::TutorialKind::kButtonBashing},
		{"�A�b�p�[�A�^�b�N",TutorialManager::TutorialKind::kUpperAttack},
		{"�f�J�L�b�N",TutorialManager::TutorialKind::kMiddleAttack},
		{"�T�C�N�����L�b�N",TutorialManager::TutorialKind::kDownAttack},
		{"��K�[�h",TutorialManager::TutorialKind::kGuardUp},
		{"���K�[�h",TutorialManager::TutorialKind::kGuardDown}
	};

	//string����EnemyInput::Action�ɕϊ����邽�߂�map
	const std::map<std::string, EnemyInput::Action> kActionMap =
	{
		{"Idle",EnemyInput::Action::kNone},
		{"EnergyAttack",EnemyInput::Action::kEnergyAttack},
		{"PhysicalAttack",EnemyInput::Action::kPhysicalAttack},
		{"Dash",EnemyInput::Action::kDash},
		{"SuperDash",EnemyInput::Action::kSuperDash},
		{"SpecialAttack",EnemyInput::Action::kSpecialAttack},
		{"EnergyCharge",EnemyInput::Action::kEnergyCharge},
		{"Guard",EnemyInput::Action::kGuard},
		{"MiddleChargeAttack",EnemyInput::Action::kMiddleChargeAttack},
		{"DownChargeAttack",EnemyInput::Action::kDownChargeAttack},
		{"UpChargeAttack",EnemyInput::Action::kUpChargeAttack},
		{"RocketDash",EnemyInput::Action::kRocketDash},
		{"None",EnemyInput::Action::kNone}
	};
}

TutorialManager::TutorialManager(std::shared_ptr<GameCamera> camera) :
	GameManagerBase(camera, GameManagerBase::GameKind::kTutorial),
	m_nowTutorial(TutorialKind::kMove),
	m_drawSituationFunc(&TutorialManager::DrawPlayMenu),
	m_updateSituationFunc(&TutorialManager::UpdatePlayMenu),
	m_tutorialSituation(TutorialSituation::kPlayMenu)
{
	m_pTutorialUi = std::make_shared<TutorialUi>();

	m_pGameUi = std::make_shared<GameUi>();

	LoadCsv load;

	m_tutorialPlayData = load.LoadFile("data/csv/tutorialPlayData.csv");

	m_pTutorialUi->SetTutorialPlayData(m_tutorialPlayData);
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
	ChangeSituation(TutorialSituation::kStart);

	//�G�l�~�[�̃C���v�b�g�f�[�^�������Ă���
	m_pEnemyInput = m_pCharacters[static_cast<int>(Character::PlayerNumber::kTwoPlayer)]->GetEnemyInput();
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

void TutorialManager::UpdateStartMenu()
{
}

void TutorialManager::UpdatePlayMenu()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	TutorialUi::MenuItem selectItem = m_pTutorialUi->GetSelectItem();

	bool isEnd = false;

	if (input->IsTrigger("A"))
	{

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
	}
	else if (input->IsTrigger("B"))
	{
		isEnd = true;
		//���s���Ă���`���[�g���A���ƑI�����Ă���`���[�g���A�����قȂ��Ă�����
		if (m_nowTutorial != static_cast<TutorialKind>(m_pTutorialUi->GetTutorialNumber()))
		{
			m_nowTutorial = static_cast<TutorialKind>(m_pTutorialUi->GetTutorialNumber());
			RetryInit();
		}
	}

	//���j���[��ʂ����I�������I�΂�Ă�����
	if (isEnd)
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

	//�J�����̍X�V���s��
	m_pCamera->Update();

}

void TutorialManager::UpdatePlaying()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	if (input->IsTrigger("Pause"))
	{
		ChangeSituation(TutorialSituation::kPlayMenu);
	}

	//�`���[�g���A���������������ǂ���
	bool isSuccess = true;

	//��������
	std::vector<TutorialSuccessKind> successTerms;

	//�O���f�[�^���琬���������擾
	for (auto& data : m_tutorialPlayData)
	{
		//�`���[�g���A���̖��O���Ⴄ�ꍇ�͎��̃f�[�^�Ɉڂ�
		if (m_nowTutorial != ChangeStringToTutorialKind(data[static_cast<int>(TutorialPlayDataIndex::kTutorialName)]))continue;

		int loopCount = 0;

		//�f�[�^�̒��g���擾
		for (auto item : data)
		{
			//���[�v�񐔂���萔�ȉ���������
			if (loopCount <= static_cast<int>(TutorialManager::TutorialPlayDataIndex::kButton))
			{
				loopCount++;
				continue;
			}

			if (item == "")continue;

			//�����������擾
			successTerms.push_back(ChangeStringToSuccessKind(item));
		}
	}

	for (auto item : successTerms)
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

	//�J�����̍X�V���s��
	m_pCamera->Update();

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

	//�J�����̍X�V���s��
	m_pCamera->Update();
}

void TutorialManager::DrawStartMenu()
{
}

void TutorialManager::DrawPlayMenu()
{
	m_pTutorialUi->DrawPlayMenu();
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
	if (next == TutorialSituation::kPlayMenu)
	{
		//�v���C���[�̏󋵂�ύX
		for (auto& player : m_pCharacters) player->ChangeSituationUpdate(static_cast<int>(BattleSituation::kMenu));

		//���������s��
		m_pTutorialUi->SetNowTutorialNumber(static_cast<int>(m_nowTutorial));
		m_pTutorialUi->InitPlayMenu();
		//�X�V�����̕ύX
		m_updateSituationFunc = &TutorialManager::UpdatePlayMenu;
		//�`�揈���̕ύX
		m_drawSituationFunc = &TutorialManager::DrawPlayMenu;
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

		m_pEnemyInput->SetAction(kActionMap.at(m_tutorialPlayData[m_pTutorialUi->GetTutorialNumber()][static_cast<int>(TutorialPlayDataIndex::kEnemySituation)]));

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

TutorialManager::TutorialSuccessKind TutorialManager::ChangeStringToSuccessKind(std::string kind)
{
	return kSuccessKindMap.at(kind);
}

TutorialManager::TutorialKind TutorialManager::ChangeStringToTutorialKind(std::string kind)
{
	return kTutorialKindMap.at(kind);
}

void TutorialManager::ChangeEnemyAction(std::string action)
{
	//�G�l�~�[�̍s����ύX����
	m_pEnemyInput->SetAction(kActionMap.at(action));
}
