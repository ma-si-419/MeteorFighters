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

#ifdef _DEBUG

	for (auto item : successTerms)
	{
		printfDx("%d,", static_cast<int>(item));
	}
	printfDx("\n");

#endif // _DEBUG
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
	if (kind == "�ړ�")
	{
		return TutorialSuccessKind::kMove;
	}
	else if (kind == "�X�e�b�v")
	{
		return TutorialSuccessKind::kStep;
	}
	else if (kind == "�_�b�V��")
	{
		return TutorialSuccessKind::kDash;
	}
	else if (kind == "�󒆑ҋ@")
	{
		return TutorialSuccessKind::kSkyIdle;
	}
	else if (kind == "�㏸")
	{
		return TutorialSuccessKind::kUp;
	}
	else if (kind == "���~")
	{
		return TutorialSuccessKind::kDown;
	}
	else if (kind == "�W�����v")
	{
		return TutorialSuccessKind::kJump;
	}
	else if (kind == "�i���U��1")
	{
		return TutorialSuccessKind::kPhysicalAttack1;
	}
	else if (kind == "�i���U��2")
	{
		return TutorialSuccessKind::kPhysicalAttack2;
	}
	else if (kind == "�i���U��3")
	{
		return TutorialSuccessKind::kPhysicalAttack3;
	}
	else if (kind == "�i���U��4")
	{
		return TutorialSuccessKind::kPhysicalAttack4;
	}
	else if (kind == "�i���U��5")
	{
		return TutorialSuccessKind::kPhysicalAttack5;
	}
	else if (kind == "�i���U��6")
	{
		return TutorialSuccessKind::kPhysicalAttack6;
	}
	else if (kind == "�i���U��7")
	{
		return TutorialSuccessKind::kPhysicalAttack7;
	}
	else if (kind == "�i���U��8")
	{
		return TutorialSuccessKind::kPhysicalAttack8;
	}
	else if (kind == "�X�}�b�V���i��")
	{
		return TutorialSuccessKind::kChargePhysicalAttack;
	}
	else if (kind == "�ǂ��ł�")
	{
		return TutorialSuccessKind::kChaseAttack;
	}
	else if (kind == "�C�̓`���[�W")
	{
		return TutorialSuccessKind::kEnergyCharge;
	}
	else if (kind == "�C�e�U��")
	{
		return TutorialSuccessKind::kEnergyAttack;
	}
	else if (kind == "�X�}�b�V���C�e")
	{
		return TutorialSuccessKind::kChargeEnergyAttack;
	}
	else if (kind == "�K�E�Z")
	{
		return TutorialSuccessKind::kSpecialAttack;
	}
	else if (kind == "�K�[�h")
	{
		return TutorialSuccessKind::kGuard;
	}
	else if (kind == "�W���X�g�K�[�h")
	{
		return TutorialSuccessKind::kJustGuard;
	}
	else if (kind == "�󂯐g")
	{
		return TutorialSuccessKind::kFalls;
	}
	else if (kind == "���A")
	{
		return TutorialSuccessKind::kReturn;
	}
	else if (kind == "�X�[�p�[�_�b�V��")
	{
		return TutorialSuccessKind::kSuperDash;
	}
	else if (kind == "���P�b�g�_�b�V��")
	{
		return TutorialSuccessKind::kRocketDash;
	}
	else if (kind == "���b�V���Ό�")
	{
		return TutorialSuccessKind::kButtonBashing;
	}
	else if (kind == "�h���U����")
	{
		return TutorialSuccessKind::kUpperAttack;
	}
	else if (kind == "�h���U����")
	{
		return TutorialSuccessKind::kDekaKick;
	}
	else if (kind == "�h���U����")
	{
		return TutorialSuccessKind::kCycloneKick;
	}
	else if (kind == "��K�[�h")
	{
		return TutorialSuccessKind::kUpGuard;
	}
	else if (kind == "���K�[�h")
	{
		return TutorialSuccessKind::kDownGuard;
	}
	else
	{
		//�����܂ŗ��Ȃ��͂�
		return TutorialSuccessKind::kMove;
	}
}

TutorialManager::TutorialKind TutorialManager::ChangeStringToTutorialKind(std::string kind)
{
	if (kind == "�ړ�")
	{
		return TutorialKind::kMove;
	}
	else if (kind == "�X�e�b�v")
	{
		return TutorialKind::kStep;
	}
	else if (kind == "�_�b�V��")
	{
		return TutorialKind::kDash;
	}
	else if (kind == "�󒆈ړ�")
	{
		return TutorialKind::kSkyMove;
	}
	else if (kind == "���b�V���i��")
	{
		return TutorialKind::kPhysicalAttack;
	}
	else if (kind == "�X�}�b�V���i��")
	{
		return TutorialKind::kChargePhysicalAttack;
	}
	else if (kind == "�ǂ��ł�")
	{
		return TutorialKind::kChaseAttack;
	}
	else if (kind == "�C�̓`���[�W")
	{
		return TutorialKind::kEnergyCharge;
	}
	else if (kind == "���b�V���C�e")
	{
		return TutorialKind::kEnergyAttack;
	}
	else if (kind == "�X�}�b�V���C�e")
	{
		return TutorialKind::kChargeEnergyAttack;
	}
	else if (kind == "�K�[�h")
	{
		return TutorialKind::kGuard;
	}
	else if (kind == "�K�E�Z")
	{
		return TutorialKind::kSpecialAttack;
	}
	else if (kind == "�������")
	{
		return TutorialKind::kJustGuard;
	}
	else if (kind == "�󂯐g")
	{
		return TutorialKind::kFalls;
	}
	else if (kind == "���A")
	{
		return TutorialKind::kReturn;
	}
	else if (kind == "�X�[�p�[�_�b�V��")
	{
		return TutorialKind::kSuperDash;
	}
	else if (kind == "���P�b�g�_�b�V��")
	{
		return TutorialKind::kRocketDash;
	}
	else if (kind == "���b�V���Ό�")
	{
		return TutorialKind::kButtonBasging;
	}
	else if (kind == "�A�b�p�[�A�^�b�N")
	{
		return TutorialKind::kUpperAttack;
	}
	else if (kind == "�f�J�L�b�N")
	{
		return TutorialKind::kMiddleAttack;
	}
	else if (kind == "�T�C�N�����L�b�N")
	{
		return TutorialKind::kDownAttack;
	}
	else if (kind == "��K�[�h")
	{
		return TutorialKind::kGuardUp;
	}
	else if (kind == "���K�[�h")
	{
		return TutorialKind::kGuardDown;
	}
	else
	{
		//�����܂ŗ��Ȃ��͂�
		return TutorialKind::kMove;
	}

	return TutorialKind();
}
