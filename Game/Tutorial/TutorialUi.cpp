#include "TutorialUi.h"
#include "LoadCsv.h"
#include "TutorialManager.h"
#include "GraphManager.h"
#include "SoundManager.h"
#include "Input.h"

namespace
{
	//�t�H���g�̖��O
	const TCHAR* kFontName = "GN-�L���SU����NB";

	//�X�^�[�g���ɕ\������t�H���g�̃T�C�Y
	constexpr int kStartFontSize = 84;
	//�v���C���̐�����\������t�H���g�̃T�C�Y
	constexpr int kPlayingFontSize = 32;
	//�v���C���̃��j���[�̕����̃t�H���g�̃T�C�Y
	constexpr int kPlayMenuFontSize = 32;
	//�X�^�[�g���j���[�̕����̃t�H���g�̃T�C�Y
	constexpr int kStartMenuFontSize = 32;
	//�Z���N�g���j���[�̕����̃t�H���g�̃T�C�Y
	constexpr int kSelectMenuFontSize = 32;
	//�Z���N�g���j���[�̃`���[�g���A���̎�ނ̃t�H���g�̃T�C�Y
	constexpr int kSelectMenuTutorialKindFontSize = 48;


	//�`���[�g���A���̃{�^����\��������W�̃t���[��
	constexpr int kButtonFramePosX = Game::kWindowWidth / 2;
	constexpr int kButtonFramePosY = 750;

	//�v���C���̃`���[�g���A���̐�����\��������WY
	constexpr int kPlayingStringPosX = Game::kWindowWidth / 2;
	constexpr int kPlayingStringPosY = 810;

	//���̃{�^���������΂����̂���\��������W
	constexpr int kButtonPosX = Game::kWindowWidth / 2;
	constexpr int kButtonPosY = 745;

	//�{�^����\������Ԋu
	constexpr int kButtonInterval = 100;

	//�X�^�[�g���j���[�̏�����(��ʊO�ɏo������)
	constexpr int kStartMenuInitDistanceX = 600;

	//�X�^�[�g���j���[��\��������W
	constexpr int kStartMenuPosX = 500;
	constexpr int kStartMenuPosY = 300;

	//�X�^�[�g���j���[�̕�����\��������W
	constexpr int kStartMenuStringPosX = kStartMenuPosX - 20;
	constexpr int kStartMenuStringPosY = kStartMenuPosY - 60;

	//�X�^�[�g���j���[�̕�����\������Ԋu
	constexpr int kStartMenuDistanceY = 90;

	//�X�^�[�g���j���[�̕����𓮂������x
	constexpr int kStartMenuSelectItemMoveSpeed = 10;

	//�X�^�[�g���j���[�̕����𓮂����͈�
	constexpr int kStartMenuItemMoveRange = 60;

	//�X�^�[�g���j���[���͂����Ă���Ƃ��̑���
	constexpr int kStartMenuMoveSpeed = 60;

	//�Z���N�g���j���[�̏�����(��ʊO�ɏo������)
	constexpr int kSelectMenuInitDistanceX = 600;

	//�Z���N�g���j���[��\��������W
	constexpr int kSelectMenuPosX = Game::kWindowWidth - 500;
	constexpr int kSelectMenuPosY = Game::kWindowHeight - 300;

	//�Z���N�g���j���[�̕�����\��������W
	constexpr int kSelectMenuStringPosX = kSelectMenuPosX;
	constexpr int kSelectMenuStringPosY = kSelectMenuPosY - 60;

	//�Z���N�g���j���[�̑I�����Ă���`���[�g���A���̎�ނ�\��������W
	constexpr int kSelectMenuTutorialKindPosX = kSelectMenuPosX - 5;
	constexpr int kSelectMenuTutorialKindPosY = kSelectMenuPosY - 180;

	//�Z���N�g���j���[�̕�����\������Ԋu
	constexpr int kSelectMenuDistanceY = 84;

	//�Z���N�g���j���[��RB��LB�̕����ƃ��j���[�̒��S�Ƃ̋���
	constexpr int kSelectMenuRbLbDistance = 380;

	//�Z���N�g���j���[��RB��LB��\��������W
	constexpr int kSelectMenuRbLbPosY = kSelectMenuTutorialKindPosY;

	//�Z���N�g���j���[���͂����Ă���Ƃ��̑���
	constexpr int kSelectMenuMoveSpeed = 60;

	//���j���[���o�Ă���Ƃ��̃t�F�[�h�C���̑���
	constexpr int kMenuFadeSpeed = 20;

	//���j���[��\�����Ă���Ƃ��̃Q�[���V�[���ɂ����鍕���{�b�N�X�̍ő�A���t�@�l
	constexpr int kBackBoxMaxAlpha = 150;

	//�㉺���͂Ń��s�[�g���͂��󂯕t���鎞��
	constexpr int kRepeatInputTime = 15;

	//�v���C���j���[�̍��E���͂Ń��s�[�g���͂��󂯕t���鎞��
	constexpr int kPlayMenuChangeTutorialRepeatInputTime = 20;

	//���s�[�g���͂̊Ԋu
	constexpr int kRepeatInterval = 5;

	//���j���[�̕�����\��������W
	constexpr int kPlayMenuStringPosX = Game::kWindowWidth / 2;

	//���E�ŕύX�ł��郁�j���[�̕�����\��������W
	constexpr int kMoveMenuStringPosX = 650;

	//���j���[�Ń`���[�g���A���̖��O��\��������W
	constexpr int kPlayMenuTutorialNamePosX = 1005;

	//�`���[�g���A���̍��E�̖��ƃ`���[�g���A���̖��O�̋���
	constexpr float kPlayMenuTutorialNameSideArrowDistance = 140.0f;

	//�`���[�g���A���̍��E�̖���h�炷����
	constexpr float kPlayMenuTutorialNameSideArrowSpeed = 0.2f;

	//�`���[�g���A���̍��E�̖���h�炷�傫��
	constexpr int kPlayMenuTutorialNameSideArrowScale = 4;

	//��ڂ̍���
	constexpr int kPlayMenuStringPosY = 175;

	//���j���[�̕�����\������Ԋu
	constexpr int kPlayMenuStringDistanceY = 110;

	//�v���C���j���[�ŕ\�����镶����
	const std::string kPlayMenuStrings[static_cast<int>(TutorialUi::PlayMenuItem::kItemNum)] =
	{
		"�󋵂����Z�b�g����",
		"�`���[�g���A����ύX",
		"�`���[�g���A���̐i�ߕ�",
		"�`���[�g���A���Z���N�g�ɖ߂�",
		"���j���[�����",
		"�`���[�g���A�����I������"
	};

	//�`���[�g���A���̐i�ߕ��ŕ\�����镶����
	const std::string kPlayMenuTutorialModeStrings[static_cast<int>(TutorialManager::TutorialMode::kModeNum)] =
	{
		"�����i�s",
		"���s�[�g",
		"�i�߂Ȃ�"
	};

	//�X�^�[�g���j���[�ŕ\�����镶����
	const std::string kStartMenuStrings[static_cast<int>(TutorialUi::StartMenuItem::kItemNum)] =
	{
		"���ԂɃ`���[�g���A�����s��",
		"�`���[�g���A����I��",
		"�`���[�g���A�����I��"
	};

	//�I�����Ă��鍀�ڂ𓮂������Ƃ��I�����Ă��鍀�ڂ��ǂ��܂ŏ��������邩
	constexpr double kSelectItemBoxMinScale = 0.7;

	//�I�����Ă��鍀�ڂ��傫���Ȃ��Ă������x
	constexpr double kSelectItemBoxScalingSpeed = 0.05;

	//�N���A���̉摜�̏����g�嗦
	constexpr double kSuccessGraphInitScale = 5.0;

	//�N���A���̉摜�����������Ă������x
	constexpr double kSuccessGraphScalingSpeed = 0.3;

	//�N���A���̉摜�̍ŏI�I�ȑ傫��
	constexpr double kSuccessGraphFinalScale = 1.0;

	//�N���A���̉摜��h�炷����
	constexpr int kSuccessGraphShakeTime = 4;

	//�N���A���̉摜��h�炷�傫��
	constexpr int kSuccessGraphShakeScale = 4;
	constexpr int kSuccessGraphShakeHalfScale = 2;

	//�N���A��ǂ̂��炢�܂�
	constexpr int kSuccessEndTime = 60;

	//�X�^�[�g���j���[�̉摜�̖��O
	const std::string kStartMenuGraphName = "StartMenu";

	//�X�^�[�g���j���[�̑I�����Ă��鍀�ڂ̉摜�̖��O
	const std::string kStartMenuSelectItemGraphName = "StartMenuSelectItem";

	//�X�^�[�g���j���[�̑I���ł��鍀�ڂ̉摜�̖��O
	const std::string kStartMenuItemGraphName = "StartMenuItem";

	//�`���[�g���A���Z���N�g���j���[�̉摜�̖��O
	const std::string kSelectMenuGraphName = "SelectMenu";

	//�`���[�g���A���Z���N�g���j���[�̑I�����Ă��鍀�ڂ̉摜�̖��O
	const std::string kSelectMenuSelectItemGraphName = "SelectMenuSelectItem";

	//�`���[�g���A���Z���N�g���j���[�̑I���ł��鍀�ڂ̉摜�̖��O
	const std::string kSelectMenuItemGraphName = "SelectMenuItem";

	//�`���[�g���A���Z���N�g���j���[��RB�{�^���̉摜�̖��O
	const std::string kSelectMenuRBGraphName = "SelectMenuRB";

	//�`���[�g���A���Z���N�g���j���[��LB�{�^���̉摜�̖��O
	const std::string kSelectMenuLBGraphName = "SelectMenuLB";

	//�v���C���j���[�̉摜�̖��O
	const std::string kPlayMenuGraphName = "PlayMenu";

	//�v���C���j���[�̑I�����Ă��鍀�ڂ̉摜�̖��O
	const std::string kPlayMenuSelectItemGraphName = "PlayMenuSelectItem";

	//�X�^�[�g���̌��̉摜�̖��O
	const std::string kStartBackGraphName = "StartBackBox";

	//�{�^���̃t���[���̉摜�̖��O
	const std::string kButtonFrameGraphName = "ButtonFrame";

	//�{�^���̉摜�̖��O
	const std::string kButtonGraphName = "Button";

	//+�̉摜�̖��O
	const std::string kPlusGraphName = "+";

	//�������̉摜�̖��O
	const std::string kSuccessGraphName = "Success";

}

TutorialUi::TutorialUi() :
	m_drawButtonNum(0),
	m_playMenuSelectItem(static_cast<PlayMenuItem>(0)),
	m_tutorialNumber(0),
	m_selectItemMoveTime(0),
	m_successTime(0),
	m_isSuccessEnd(false),
	m_selectTutorialNumber(0),
	m_selectTutorialStringArrowPos(0),
	m_isSuccessTutorial(false),
	m_isSelectStartMenu(false),
	m_isSelectSelectMenu(false),
	m_selectMenuIndexX(0),
	m_selectMenuIndexY(0),
	m_nowTutorialMode(0)
{
	LoadCsv load;

	m_tutorialSelectMenuData = load.LoadFile("data/csv/tutorialSelectMenuData.csv");

	m_updateFunc = &TutorialUi::UpdateStartMenu;

	m_startFontHandle = CreateFontToHandle(kFontName, kStartFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 3);
	m_playingFontHandle = CreateFontToHandle(kFontName, kPlayingFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 3);
	m_playMenuFontHandle = CreateFontToHandle(kFontName, kPlayMenuFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 2);
	m_startMenuFontHandle = CreateFontToHandle(kFontName, kStartMenuFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 2);
	m_selectMenuFontHandle = CreateFontToHandle(kFontName, kSelectMenuFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 2);
	m_selectTutorialKindFontHandle = CreateFontToHandle(kFontName, kSelectMenuTutorialKindFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 3);
}

TutorialUi::~TutorialUi()
{
}

void TutorialUi::Init()
{
}

void TutorialUi::Update()
{
	(this->*m_updateFunc)();
}

void TutorialUi::Final()
{
	//�t�H���g�̍폜
	DeleteFontToHandle(m_startFontHandle);
	DeleteFontToHandle(m_playingFontHandle);
	DeleteFontToHandle(m_playMenuFontHandle);
	DeleteFontToHandle(m_startMenuFontHandle);
	DeleteFontToHandle(m_selectMenuFontHandle);
	DeleteFontToHandle(m_selectTutorialKindFontHandle);
}

void TutorialUi::InitStartMenu()
{
	//�摜�}�l�[�W���[
	auto& manager = GraphManager::GetInstance();

	//���j���[��ʂ̉摜��o�^����
	GraphData menu;
	menu.handle = manager.GetHandle(kStartMenuGraphName);

	//���j���[��ʂ̍��W��ݒ肷��
	menu.pos = MyEngine::Vector2(-kStartMenuPosX - kStartMenuInitDistanceX, kStartMenuPosY);
	m_drawGraphs[kStartMenuGraphName] = menu;

	//�A�b�v�f�[�g��ύX����
	m_updateFunc = &TutorialUi::UpdateStartMenu;

	//���j���[��ʂőI���ł��鍀�ڂ������摜��o�^����
	for (int i = 0; i < static_cast<int>(TutorialUi::StartMenuItem::kItemNum); i++)
	{
		std::string name = kStartMenuItemGraphName;
		name += std::to_string(i);

		GraphData itemBox;

		itemBox.handle = manager.GetHandle(kStartMenuItemGraphName);
		itemBox.pos = MyEngine::Vector2(-kStartMenuStringPosX - kStartMenuInitDistanceX, kStartMenuStringPosY + (kStartMenuDistanceY * i));
		m_drawGraphs[name] = itemBox;
	}

	//���j���[��ʂőI�����Ă��鍀�ڂ������摜��o�^����
	GraphData selectItemBox;
	selectItemBox.handle = manager.GetHandle(kStartMenuSelectItemGraphName);
	selectItemBox.pos = MyEngine::Vector2(-kStartMenuStringPosX - kStartMenuInitDistanceX, kStartMenuStringPosY);
	m_drawGraphs[kStartMenuSelectItemGraphName] = selectItemBox;

	//�I�����Ă��鍀�ڂ���ԏ�ɂ���
	m_startMenuSelectItem = static_cast<StartMenuItem>(0);

	//���̈ʒu�ɂ���܂őI���ł��Ȃ��悤�ɂ���
	m_isSelectStartMenu = false;
}

void TutorialUi::InitPlayMenu()
{
	//�o�^���Ă���摜�����ׂď���
	m_drawGraphs.clear();

	//�摜�}�l�[�W���[
	auto& manager = GraphManager::GetInstance();

	//���j���[��ʂ̉摜��o�^����
	GraphData menu;
	menu.handle = manager.GetHandle(kPlayMenuGraphName);
	menu.pos = MyEngine::Vector2(Game::kWindowWidth / 2, Game::kWindowHeight / 2);
	menu.alpha = 0;
	m_drawGraphs[kPlayMenuGraphName] = menu;

	//�A�b�v�f�[�g��ύX����
	m_updateFunc = &TutorialUi::UpdatePlayMenu;

	//���j���[��ʂőI�����Ă��鍀�ڂ������摜��o�^����
	GraphData selectItemBox;
	selectItemBox.handle = manager.GetHandle(kPlayMenuSelectItemGraphName);
	selectItemBox.pos = MyEngine::Vector2(kPlayMenuStringPosX, kPlayMenuStringPosY);
	m_drawGraphs[kPlayMenuSelectItemGraphName] = selectItemBox;

	//�I�����Ă��鍀�ڂ���ԏ�ɂ���
	m_playMenuSelectItem = static_cast<PlayMenuItem>(0);

}

void TutorialUi::InitStart()
{
	//�o�^���Ă���摜�����ׂď���
	m_drawGraphs.clear();

	m_updateFunc = &TutorialUi::UpdateStart;

	auto& manager = GraphManager::GetInstance();

	GraphData data;

	data.pos = MyEngine::Vector2(Game::kWindowWidth / 2, Game::kWindowHeight / 2);

	data.handle = manager.GetHandle(kStartBackGraphName);

	m_drawGraphs[kStartBackGraphName] = data;
}

void TutorialUi::InitSelectMenu()
{
	m_updateFunc = &TutorialUi::UpdateSelectMenu;

	//�I�����Ă��鍀�ڂ���ԏ�ɂ���
	m_selectMenuIndexX = 0;

	//�摜�}�l�[�W���[
	auto& manager = GraphManager::GetInstance();

	//���j���[��ʂ̉摜��o�^����
	GraphData menu;
	menu.handle = manager.GetHandle(kSelectMenuGraphName);
	menu.pos = MyEngine::Vector2(Game::kWindowWidth + kSelectMenuPosX + kSelectMenuInitDistanceX, kSelectMenuPosY);
	m_drawGraphs[kSelectMenuGraphName] = menu;

	//�A�b�v�f�[�g��ύX����
	m_updateFunc = &TutorialUi::UpdateSelectMenu;

	//�I�����Ă��鍀�ڂ����Z�b�g����
	m_selectMenuIndexX = 0;
	m_selectMenuIndexY = 0;

	//���j���[��ʂőI���ł��鍀�ڂ������摜��o�^����
	for (int i = 0; i < m_tutorialSelectMenuData[m_selectMenuIndexX].size() + 1; i++)
	{
		std::string name = kSelectMenuItemGraphName;
		name += std::to_string(i);

		GraphData itemBox;

		itemBox.handle = manager.GetHandle(kSelectMenuItemGraphName);
		itemBox.pos = MyEngine::Vector2(Game::kWindowWidth + kSelectMenuStringPosX + kSelectMenuInitDistanceX, kSelectMenuStringPosY + (kSelectMenuDistanceY * i));
		m_drawGraphs[name] = itemBox;
	}

	//RB�{�^����o�^����
	GraphData rbButton;
	rbButton.handle = manager.GetHandle(kSelectMenuRBGraphName);
	rbButton.pos = MyEngine::Vector2(Game::kWindowWidth + kSelectMenuPosX + kSelectMenuRbLbDistance + kSelectMenuInitDistanceX, kSelectMenuRbLbPosY);
	m_drawGraphs[kSelectMenuRBGraphName] = rbButton;

	//LB�{�^����o�^����
	GraphData lbButton;
	lbButton.handle = manager.GetHandle(kSelectMenuLBGraphName);
	lbButton.pos = MyEngine::Vector2(Game::kWindowWidth + kSelectMenuPosX - kSelectMenuRbLbDistance + kSelectMenuInitDistanceX, kSelectMenuRbLbPosY);
	m_drawGraphs[kSelectMenuLBGraphName] = lbButton;

	//���j���[��ʂőI�����Ă��鍀�ڂ������摜��o�^����
	GraphData selectItemBox;
	selectItemBox.handle = manager.GetHandle(kSelectMenuSelectItemGraphName);
	selectItemBox.pos = MyEngine::Vector2(Game::kWindowWidth + kSelectMenuStringPosX + kSelectMenuInitDistanceX, kSelectMenuStringPosY);
	m_drawGraphs[kSelectMenuSelectItemGraphName] = selectItemBox;

	//���̈ʒu�ɂ���܂őI���ł��Ȃ��悤�ɂ���
	m_isSelectSelectMenu = false;
}

void TutorialUi::InitPlaying(int number)
{
	//�o�^���Ă���摜�����ׂď���
	m_drawGraphs.clear();

	m_updateFunc = &TutorialUi::UpdatePlaying;

	//�摜�Ǘ��N���X
	auto& manager = GraphManager::GetInstance();
	LoadCsv stringFunc;

	GraphData frame;

	frame.pos = MyEngine::Vector2(kButtonFramePosX, kButtonFramePosY);

	frame.handle = manager.GetHandle(kButtonFrameGraphName);

	m_drawGraphs[kButtonFrameGraphName] = frame;

	auto buttonString = m_tutorialPlayData[number][static_cast<int>(TutorialManager::TutorialPlayDataIndex::kButton)];

	auto button = stringFunc.Split(buttonString, '/');

	m_drawButtonNum = static_cast<int>(button.size());

	//�{�^���̕`�搔��1�ł����
	if (m_drawButtonNum == 1)
	{
		GraphData buttonData;

		buttonData.handle = manager.GetHandle(button[0]);

		buttonData.pos = MyEngine::Vector2(kButtonPosX, kButtonPosY);

		std::string name = kButtonGraphName + "0";

		m_drawGraphs[name] = buttonData;
	}
	//�{�^���̕`�搔�������ł����
	else if (m_drawButtonNum % 2 == 0)
	{
		//�`��J�n�ʒu���v�Z����
		int startPosX = kButtonPosX - (kButtonInterval * m_drawButtonNum / 2);
		startPosX += kButtonInterval / 2;

		//�`�悷��{�^��������
		for (auto item : button)
		{
			//+�{�^���̏ꍇ�͍��W�����炷
			if (item == kPlusGraphName)
			{
				startPosX += kButtonInterval / 2;
			}
		}


		int buttonNum = 0;

		//�`�搔�������[�v
		for (int i = 0; i < m_drawButtonNum; i++)
		{
			GraphData buttonData;

			buttonData.handle = manager.GetHandle(button[i]);

			int posX = startPosX;

			posX += kButtonInterval * buttonNum;

			//+�{�^���̏ꍇ�͍��W�����炷
			if (button[i] == kPlusGraphName)
			{
				posX -= kButtonInterval / 2;

				buttonNum--;
			}

			buttonData.pos = MyEngine::Vector2(posX, kButtonPosY);

			std::string name = kButtonGraphName + std::to_string(i);

			m_drawGraphs[name] = buttonData;

			buttonNum++;
		}
	}
	//��̏ꍇ
	else
	{
		//�`��J�n�ʒu���v�Z����
		int startPosX = kButtonPosX;

		startPosX -= kButtonInterval * ((m_drawButtonNum - 1) / 2);

		//�`�悷��{�^��������
		for (auto item : button)
		{
			//+�{�^���̏ꍇ�͍��W�����炷
			if (item == kPlusGraphName)
			{
				startPosX += kButtonInterval / 2;
			}
		}

		int buttonNum = 0;

		//�`�搔�������[�v
		for (int i = 0; i < m_drawButtonNum; i++)
		{
			GraphData buttonData;

			buttonData.handle = manager.GetHandle(button[i]);

			int posX = startPosX;

			posX += kButtonInterval * buttonNum;

			//+�{�^���̏ꍇ�͍��W�����炷
			if (button[i] == kPlusGraphName)
			{
				posX -= kButtonInterval / 2;

				buttonNum--;
			}

			buttonData.pos = MyEngine::Vector2(posX, kButtonPosY);

			std::string name = kButtonGraphName + std::to_string(i);

			m_drawGraphs[name] = buttonData;

			buttonNum++;
		}
	}

}

void TutorialUi::InitSuccess()
{
	auto& manager = GraphManager::GetInstance();

	m_updateFunc = &TutorialUi::UpdateSuccess;

	//�N���A���̉摜���擾
	GraphData success;
	success.handle = manager.GetHandle(kSuccessGraphName);
	//���W�̓{�^����\�����Ă�����W
	success.pos = MyEngine::Vector2(kButtonPosX, kButtonPosY);
	//�ŏ��͑傫��
	success.scale = kSuccessGraphInitScale;

	m_drawGraphs[kSuccessGraphName] = success;

	m_isSuccessEnd = false;
	m_successTime = 0;

}

void TutorialUi::DrawStartMenu()
{
	GraphData menu = m_drawGraphs[kStartMenuGraphName];

	//���j���[��`��
	DrawRotaGraph(static_cast<int>(menu.pos.x), static_cast<int>(menu.pos.y), menu.scale, 0.0, menu.handle, true);

	//�A�C�e���̐��������[�v
	for (int i = 0; i < static_cast<int>(TutorialUi::StartMenuItem::kItemNum); i++)
	{
		//������̗��̃{�b�N�X��\������
		std::string name = kStartMenuItemGraphName;
		name += std::to_string(i);

		GraphData itemBox = m_drawGraphs[name];

		DrawRotaGraph(static_cast<int>(itemBox.pos.x), static_cast<int>(itemBox.pos.y), itemBox.scale, 0.0, itemBox.handle, true);
	}

	//�I�����Ă��鍀�ڂ̃{�b�N�X��\������
	GraphData selectItemBox = m_drawGraphs[kStartMenuSelectItemGraphName];

	DrawRotaGraph(static_cast<int>(selectItemBox.pos.x), static_cast<int>(selectItemBox.pos.y), selectItemBox.scale, 0.0, selectItemBox.handle, true);

	//�A�C�e���̐��������[�v
	for (int i = 0; i < static_cast<int>(TutorialUi::StartMenuItem::kItemNum); i++)
	{
		//�{�b�N�X�̍��W�ɍ��킹��
		std::string name = kStartMenuItemGraphName;
		name += std::to_string(i);

		GraphData itemBox = m_drawGraphs[name];

		//������̍��W��ݒ�
		MyEngine::Vector2 pos = itemBox.pos;

		DrawStringCenter(kStartMenuStrings[i], pos, m_startMenuFontHandle, GetColor(0, 0, 0), GetColor(255, 255, 255));
	}

	//�Z���N�g���j���[�̉摜���o�^����Ă�����`�悷��
	if (m_drawGraphs.find(kSelectMenuGraphName) != m_drawGraphs.end())
	{
		//�X�V�֐����X�^�[�g���j���[�Ȃ�
		if (m_updateFunc == &TutorialUi::UpdateStartMenu)
		{
			DrawSelectMenu();
		}
	}

}

void TutorialUi::DrawPlayMenu()
{
	GraphData menu = m_drawGraphs[kPlayMenuGraphName];

	//��ʑS�̂𕢂������{�b�N�X�̃A���t�@�l
	int backBoxAlpha = menu.alpha;

	backBoxAlpha = min(backBoxAlpha, kBackBoxMaxAlpha);

	//�u�����h���[�h�ύX
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, backBoxAlpha);

	//��ʑS�̂𕢂������{�b�N�X�\��
	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, GetColor(0, 0, 0), true);

	//�A���t�@�l�ύX
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, menu.alpha);

	DrawRotaGraph(static_cast<int>(menu.pos.x), static_cast<int>(menu.pos.y), menu.scale, 0.0, menu.handle, true);

	//�u�����h���[�h�����ɖ߂�
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//�I�����Ă��鍀�ڂ̌��̃{�b�N�X��\������
	GraphData itemBox = m_drawGraphs[kPlayMenuSelectItemGraphName];

	DrawRotaGraph(static_cast<int>(itemBox.pos.x), static_cast<int>(itemBox.pos.y), itemBox.scale, 0.0, itemBox.handle, true);

	//�������牺�͕������\�����鏈��

	//�A���t�@�l�����j���[�ƍ��킹��
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, menu.alpha);

	//�������\������
	for (int i = 0; i < static_cast<int>(TutorialUi::PlayMenuItem::kItemNum); i++)
	{
		//��ԏ�̕����̍��W
		MyEngine::Vector2 pos = MyEngine::Vector2(kPlayMenuStringPosX, kPlayMenuStringPosY);

		//������Y���W�����炷
		pos.y += kPlayMenuStringDistanceY * i;

		//���E�ɓ������镶���̍��W�����ɂ��炷(�ʂ̏����ɂ�����)

		if (i == static_cast<int>(TutorialUi::PlayMenuItem::kChangeTutorial) ||
			i == static_cast<int>(TutorialUi::PlayMenuItem::kChangeMode))
		{
			pos.x = kMoveMenuStringPosX;
		}

		//�������\��
		DrawStringCenter(kPlayMenuStrings[i], pos, m_playMenuFontHandle, GetColor(0, 0, 0), GetColor(255, 255, 255));

	}

	//�I�����Ă���`���[�g���A����\������
	MyEngine::Vector2 selectTutorialPos = MyEngine::Vector2(kPlayMenuTutorialNamePosX, kPlayMenuStringPosY + kPlayMenuStringDistanceY * static_cast<int>(PlayMenuItem::kChangeTutorial));
	std::string selectTutorialName = m_tutorialPlayData[m_selectTutorialNumber][static_cast<int>(TutorialManager::TutorialPlayDataIndex::kTutorialName)];
	DrawStringCenter(selectTutorialName, selectTutorialPos, m_playMenuFontHandle, GetColor(0, 0, 0), GetColor(255, 255, 255));

	//�I�����Ă���`���[�g���A���̍��E�̖���\������
	float leftGap = kPlayMenuTutorialNameSideArrowDistance + sinf(m_selectTutorialStringArrowPos) * kPlayMenuTutorialNameSideArrowScale;
	float rightGap = kPlayMenuTutorialNameSideArrowDistance + sinf(m_selectTutorialStringArrowPos) * kPlayMenuTutorialNameSideArrowScale;

	//���E�̖��
	MyEngine::Vector2 leftArrowPos = MyEngine::Vector2(selectTutorialPos.x - leftGap - static_cast<float>(kPlayMenuFontSize), selectTutorialPos.y - static_cast<float>(kPlayMenuFontSize / 2));
	MyEngine::Vector2 rightArrowPos = MyEngine::Vector2(selectTutorialPos.x + rightGap, selectTutorialPos.y - static_cast<float>(kPlayMenuFontSize / 2));

	//���E�̖���`��
	DrawStringToHandle(static_cast<int>(leftArrowPos.x), static_cast<int>(leftArrowPos.y), "��", GetColor(0, 0, 0), m_playMenuFontHandle, GetColor(255, 255, 255));
	DrawStringToHandle(static_cast<int>(rightArrowPos.x), static_cast<int>(rightArrowPos.y), "��", GetColor(0, 0, 0), m_playMenuFontHandle, GetColor(255, 255, 255));
	
	//�`���[�g���A���̐i�ߕ���\������
	MyEngine::Vector2 tutorialModePos = MyEngine::Vector2(kPlayMenuTutorialNamePosX, kPlayMenuStringPosY + kPlayMenuStringDistanceY * static_cast<int>(PlayMenuItem::kChangeMode));
	std::string tutorialModeString = kPlayMenuTutorialModeStrings[static_cast<int>(m_nowTutorialMode)];
	DrawStringCenter(tutorialModeString, tutorialModePos, m_playMenuFontHandle, GetColor(0, 0, 0), GetColor(255, 255, 255));

	//���E�̖����`���[�g���A���̐i�ߕ��̉��ɂ��\������
	leftArrowPos.y += kPlayMenuStringDistanceY;
	rightArrowPos.y += kPlayMenuStringDistanceY;

	//���E�̖���`��
	DrawStringToHandle(static_cast<int>(leftArrowPos.x), static_cast<int>(leftArrowPos.y), "��", GetColor(0, 0, 0), m_playMenuFontHandle, GetColor(255, 255, 255));
	DrawStringToHandle(static_cast<int>(rightArrowPos.x), static_cast<int>(rightArrowPos.y), "��", GetColor(0, 0, 0), m_playMenuFontHandle, GetColor(255, 255, 255));

	//�u�����h���[�h�����ɖ߂�
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void TutorialUi::DrawStart(int number)
{
	GraphData backBox = m_drawGraphs[kStartBackGraphName];

	DrawRotaGraph(static_cast<int>(backBox.pos.x), static_cast<int>(backBox.pos.y), backBox.scale, 0.0, backBox.handle, true);

	std::string startWord = m_tutorialPlayData[number][static_cast<int>(TutorialManager::TutorialPlayDataIndex::kDescription)];

	//TODO : �t�@�C�����[�h�ƕ�����֗̕��֐��𕪂���悤�ɂ���(���t�@�N�^�����O)
	LoadCsv load;

	std::vector<std::string> splitString = load.Split(startWord, '/');

	//��{�I�ɂ͉�ʒ����ɕ��͂�\������
	MyEngine::Vector2 stringPos = MyEngine::Vector2(Game::kWindowWidth / 2, Game::kWindowHeight / 2);

	for (int i = 0; i < splitString.size(); i++)
	{
		//������
		if (splitString.size() == 2)
		{
			MyEngine::Vector2 shift = MyEngine::Vector2(0, static_cast<int>(-GetFontSizeToHandle(m_startFontHandle) * 0.5f));

			shift.y += (GetFontSizeToHandle(m_startFontHandle) * 2) * i;

			stringPos += shift;

			DrawStringCenter(splitString[i], stringPos, m_startFontHandle, GetColor(0, 0, 0), GetColor(255, 255, 255));
		}
		else
		{
			DrawStringCenter(splitString[i], stringPos, m_startFontHandle, GetColor(0, 0, 0), GetColor(255, 255, 255));
		}
	}
}

void TutorialUi::DrawPlaying(int number)
{
	GraphData frameData = m_drawGraphs[kButtonFrameGraphName];

	//�{�^����\������t���[��
	DrawRotaGraph(static_cast<int>(frameData.pos.x), static_cast<int>(frameData.pos.y), frameData.scale, 0.0, frameData.handle, true);

	//�v���C���̐���
	auto playingString = m_tutorialPlayData[number][static_cast<int>(TutorialManager::TutorialPlayDataIndex::kPlayingString)];

	//�v���C���̐�����`��
	DrawStringCenter(playingString, MyEngine::Vector2(kPlayingStringPosX, kPlayingStringPosY), m_playingFontHandle, GetColor(255, 255, 255), GetColor(0, 0, 0));

	//�����{�^����\������
	for (int i = 0; i < m_drawButtonNum; i++)
	{
		std::string name = kButtonGraphName + std::to_string(i);

		GraphData buttonData = m_drawGraphs[name];

		DrawRotaGraph(static_cast<int>(buttonData.pos.x), static_cast<int>(buttonData.pos.y), buttonData.scale, 0.0, buttonData.handle, true);
	}
}

void TutorialUi::DrawSuccess(int number)
{
	GraphData frameData = m_drawGraphs[kButtonFrameGraphName];

	//�{�^����\������t���[��
	DrawRotaGraph(static_cast<int>(frameData.pos.x), static_cast<int>(frameData.pos.y), frameData.scale, 0.0, frameData.handle, true);

	//�v���C���̐���
	auto playingString = m_tutorialPlayData[number][static_cast<int>(TutorialManager::TutorialPlayDataIndex::kPlayingString)];

	//�v���C���̐�����`��
	DrawStringCenter(playingString, MyEngine::Vector2(kPlayingStringPosX, kPlayingStringPosY), m_playingFontHandle, GetColor(255, 255, 255), GetColor(0, 0, 0));

	//�����{�^����\������
	for (int i = 0; i < m_drawButtonNum; i++)
	{
		std::string name = kButtonGraphName + std::to_string(i);

		GraphData buttonData = m_drawGraphs[name];

		DrawRotaGraph(static_cast<int>(buttonData.pos.x), static_cast<int>(buttonData.pos.y), buttonData.scale, 0.0, buttonData.handle, true);
	}

	GraphData success = m_drawGraphs[kSuccessGraphName];

	//�N���A���̉摜�\��
	DrawRotaGraph(static_cast<int>(success.pos.x), static_cast<int>(success.pos.y), success.scale, 0.0, success.handle, true);
}

void TutorialUi::DrawSelectMenu()
{
	//�X�^�[�g���j���[�̉摜��`��
	if (m_updateFunc == &TutorialUi::UpdateSelectMenu)
	{
		DrawStartMenu();
	}

	//�Z���N�g���j���[�̉摜�\��
	{
		GraphData menu = m_drawGraphs[kSelectMenuGraphName];
		//���j���[��`��
		DrawRotaGraph(static_cast<int>(menu.pos.x), static_cast<int>(menu.pos.y), menu.scale, 0.0, menu.handle, true);
		//�A�C�e���̐��������[�v
		for (int i = 0; i < m_tutorialSelectMenuData[m_selectMenuIndexX + 1].size(); i++)
		{
			//������̗��̃{�b�N�X��\������
			std::string name = kSelectMenuItemGraphName;
			name += std::to_string(i);
			GraphData itemBox = m_drawGraphs[name];
			DrawRotaGraph(static_cast<int>(itemBox.pos.x), static_cast<int>(itemBox.pos.y), itemBox.scale, 0.0, itemBox.handle, true);
		}
		//�I�����Ă��鍀�ڂ̃{�b�N�X��\������
		GraphData selectItemBox = m_drawGraphs[kSelectMenuSelectItemGraphName];
		DrawRotaGraph(static_cast<int>(selectItemBox.pos.x), static_cast<int>(selectItemBox.pos.y), selectItemBox.scale, 0.0, selectItemBox.handle, true);
		//�A�C�e���̐��������[�v
		for (int i = 0; i < m_tutorialSelectMenuData[m_selectMenuIndexX + 1].size(); i++)
		{
			//�{�b�N�X�̍��W�ɍ��킹��
			std::string name = kSelectMenuItemGraphName;
			name += std::to_string(i);
			GraphData itemBox = m_drawGraphs[name];
			//������̍��W��ݒ�
			MyEngine::Vector2 pos = itemBox.pos;
			DrawStringCenter(m_tutorialSelectMenuData[m_selectMenuIndexX + 1][i], pos, m_selectMenuFontHandle, GetColor(0, 0, 0), GetColor(255, 255, 255));
		}
		//�I�����Ă���區�ڂ�\������
		MyEngine::Vector2 selectTutorialKindStringPos = menu.pos;

		//Y���W�̐ݒ�
		selectTutorialKindStringPos.y = kSelectMenuTutorialKindPosY;

		//�`��
		DrawStringCenter(m_tutorialSelectMenuData[m_selectMenuIndexX][0], selectTutorialKindStringPos, m_selectTutorialKindFontHandle, GetColor(64, 64, 255), GetColor(255, 255, 16));

		//RB�{�^����\������
		GraphData rbButton = m_drawGraphs[kSelectMenuRBGraphName];
		DrawRotaGraph(static_cast<int>(rbButton.pos.x), static_cast<int>(rbButton.pos.y), rbButton.scale, 0.0, rbButton.handle, true);
		//LB�{�^����\������
		GraphData lbButton = m_drawGraphs[kSelectMenuLBGraphName];
		DrawRotaGraph(static_cast<int>(lbButton.pos.x), static_cast<int>(lbButton.pos.y), lbButton.scale, 0.0, lbButton.handle, true);
	}
}

void TutorialUi::DrawStringCenter(std::string string, MyEngine::Vector2 centerPos, int font, int color, int edgeColor)
{
	// ������\��������W
	MyEngine::Vector2 pos;

	// ������
	int wordNum = GetStringLength(string.c_str());

	// �����̕����v�Z
	int charWidth = GetFontSizeToHandle(font) + GetFontEdgeSizeToHandle(font);

	// �������������̏ꍇ
	if (wordNum % 2 == 0)
	{
		// �����̍��W�����ɂ��炷
		pos.x = centerPos.x - static_cast<float>(charWidth * wordNum / 2);
	}
	// ��̏ꍇ
	else
	{
		// �����̍��W�����ɂ��炷
		pos.x = centerPos.x - static_cast<float>(charWidth * (wordNum / 2)) - static_cast<float>(charWidth / 2);
	}

	// �����̑傫���̔���������ɂ��炷
	pos.y = centerPos.y - static_cast<float>(charWidth / 2);

	// �����̕`��
	DrawStringToHandle(static_cast<int>(pos.x), static_cast<int>(pos.y), string.c_str(), color, font, edgeColor);
}

void TutorialUi::UpdateStartMenu()
{
	//����
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//m_drawGraphs��kSelectMenuGraphName������̂Ȃ�
	if (m_drawGraphs.find(kSelectMenuGraphName) != m_drawGraphs.end())
	{
		//�I�����j���[���E�ɓ�����
		GraphData menu = m_drawGraphs[kSelectMenuGraphName];
		menu.pos.x += kStartMenuMoveSpeed;
		//���j���[��ʂ̍��W���X�V
		m_drawGraphs[kSelectMenuGraphName] = menu;
		//�I���ł��鍀�ڂ𐔂������[�v
		for (int i = 0; i < m_tutorialSelectMenuData[m_selectMenuIndexX].size() + 1; i++)
		{
			//�I���ł��鍀�ڂ������摜
			std::string name = kSelectMenuItemGraphName;
			name += std::to_string(i);
			auto& itemBox = m_drawGraphs[name];
			//�I���ł��鍀�ڂ��E�ɓ�����
			itemBox.pos.x += kStartMenuMoveSpeed;
			//�I���ł��鍀�ڂ��X�V
			m_drawGraphs[name] = itemBox;
		}
		//�I�����Ă��鍀�ڂ��E�ɓ�����
		auto& selectItemBox = m_drawGraphs[kSelectMenuSelectItemGraphName];
		selectItemBox.pos.x += kStartMenuMoveSpeed;
		//�I�����Ă��鍀�ڂ��X�V
		m_drawGraphs[kSelectMenuSelectItemGraphName] = selectItemBox;

		//RB�{�^�����E�ɓ�����
		auto& rbButton = m_drawGraphs[kSelectMenuRBGraphName];
		rbButton.pos.x += kStartMenuMoveSpeed;
		m_drawGraphs[kSelectMenuRBGraphName] = rbButton;

		//LB�{�^�����E�ɓ�����
		auto& lbButton = m_drawGraphs[kSelectMenuLBGraphName];
		lbButton.pos.x += kStartMenuMoveSpeed;
		m_drawGraphs[kSelectMenuLBGraphName] = lbButton;
	}

	//�X�^�[�g���j���[�̉摜���E�ɓ�����
	if (!m_isSelectStartMenu)
	{
		//���j���[�摜���E�ɓ�����
		GraphData menu = m_drawGraphs[kStartMenuGraphName];
		menu.pos.x += kStartMenuMoveSpeed;

		//�N�����v
		menu.pos.x = fmin(menu.pos.x, static_cast<float>(kStartMenuPosX));

		//���j���[�����̈ʒu�ɗ�����
		if (menu.pos.x == static_cast<float>(kStartMenuPosX))
		{
			//�I���ł���悤�ɂ���
			m_isSelectStartMenu = true;
		}

		//���j���[��ʂ̍��W���X�V
		m_drawGraphs[kStartMenuGraphName] = menu;

		//�I���ł��鍀�ڂ𐔂������[�v
		for (int i = 0; i < static_cast<int>(TutorialUi::StartMenuItem::kItemNum); i++)
		{
			//�I���ł��鍀�ڂ������摜
			std::string name = kStartMenuItemGraphName;
			name += std::to_string(i);
			auto& itemBox = m_drawGraphs[name];
			//�I���ł��鍀�ڂ��E�ɓ�����
			itemBox.pos.x += kStartMenuMoveSpeed;
			//�N�����v
			itemBox.pos.x = fmin(itemBox.pos.x, static_cast<float>(kStartMenuStringPosX));
			//�I���ł��鍀�ڂ��X�V
			m_drawGraphs[name] = itemBox;
		}

		//�I�����Ă��鍀�ڂ��E�ɓ�����
		auto& selectItemBox = m_drawGraphs[kStartMenuSelectItemGraphName];

		selectItemBox.pos.x += kStartMenuMoveSpeed;

		//�N�����v
		selectItemBox.pos.x = fmin(selectItemBox.pos.x, static_cast<float>(kStartMenuStringPosX));

		//�I�����Ă��鍀�ڂ��X�V
		m_drawGraphs[kStartMenuSelectItemGraphName] = selectItemBox;
	}
	else
	{
		//�I�����Ă��鍀��
		int selectItem = static_cast<int>(m_startMenuSelectItem);


		//�㉺���͂őI�����Ă�����̂�ύX����
		if (input->IsTrigger("Up"))
		{
			selectItem--;

			//��ԏ�ŏ���͂����ꍇ
			if (selectItem < 0)
			{
				//��ԉ��Ɉړ�����
				selectItem = static_cast<int>(StartMenuItem::kItemEnd);
			}
		}
		else if (input->IsTrigger("Down"))
		{
			selectItem++;

			//��ԉ��œ��͂��ꂽ�ꍇ
			if (selectItem > static_cast<int>(StartMenuItem::kItemEnd))
			{
				//��ԏ�Ɉړ�����
				selectItem = 0;
			}
		}

		//���s�[�g����
		if (m_selectItemMoveTime > kRepeatInterval)
		{
			if (input->GetPressTime("Up") > kRepeatInputTime)
			{
				selectItem--;
			}
			else if (input->GetPressTime("Down") > kRepeatInputTime)
			{
				selectItem++;
			}
		}

		//�ő�l�ƍŏ��l�𒴂��Ȃ��悤��
		selectItem = max(selectItem, 0);
		selectItem = min(selectItem, static_cast<int>(StartMenuItem::kItemEnd));

		//�I�����Ă��鍀�ڂ�\���摜
		auto& selectItemBox = m_drawGraphs[kStartMenuSelectItemGraphName];

		//�I�����Ă��鍀�ڂ��ω����Ă�����
		if (selectItem != static_cast<int>(m_startMenuSelectItem))
		{
			//�ω����Ă��牽�t���[���������������Z�b�g����
			m_selectItemMoveTime = 0;

			//�I�����Ă��鍀�ڂ������摜�𓮂���
			MyEngine::Vector2 pos = MyEngine::Vector2(kStartMenuStringPosX, kStartMenuStringPosY);
			pos.y += kStartMenuDistanceY * selectItem;

			//���W��ݒ�
			selectItemBox.pos = pos;

			//�T�E���h���Đ�
			SoundManager::GetInstance().PlayOnceSound("Select");

		}
		//�I�����Ă��鍀�ڂ��ω����Ă��Ȃ����
		else
		{
			m_selectItemMoveTime++;
		}

		//�I�����Ă��鍀�ڂ��X�V
		m_startMenuSelectItem = static_cast<StartMenuItem>(selectItem);

		//�I������Ă��鍀�ڂ��E�ɓ������Ă���
		std::string selectItemName = kStartMenuItemGraphName;
		selectItemName += std::to_string(selectItem);

		auto& itemBox = m_drawGraphs[selectItemName];

		itemBox.pos.x += kStartMenuSelectItemMoveSpeed;

		//�N�����v
		itemBox.pos.x = fmin(itemBox.pos.x, static_cast<float>(kStartMenuStringPosX + kStartMenuItemMoveRange));

		//�I�����Ă��鍀�ڂ�\���摜���ꏏ�ɓ�����
		std::string yellowBoxName = kStartMenuSelectItemGraphName;

		auto& yellowBox = m_drawGraphs[yellowBoxName];

		yellowBox.pos.x = itemBox.pos.x;

		//�I������Ă��Ȃ����ڂ����ɓ������Ă���
		for (int i = 0; i < static_cast<int>(StartMenuItem::kItemNum); i++)
		{
			//�I������Ă��鍀�ڂ̓X�L�b�v
			if (i == selectItem)
			{
				continue;
			}
			std::string name = kStartMenuItemGraphName;
			name += std::to_string(i);
			auto& item = m_drawGraphs[name];
			item.pos.x -= kStartMenuSelectItemMoveSpeed;
			//�N�����v
			item.pos.x = fmax(item.pos.x, static_cast<float>(kStartMenuStringPosX));
		}
	}
}

void TutorialUi::UpdateSelectMenu()
{
	if (!m_isSelectSelectMenu)
	{

		{
			//�X�^�[�g���j���[�̉摜�����ɓ�����
			GraphData menu = m_drawGraphs[kStartMenuGraphName];
			menu.pos.x -= kStartMenuMoveSpeed;

			//�I�����̉摜�����ɓ�����
			for (int i = 0; i < static_cast<int>(TutorialUi::StartMenuItem::kItemNum); i++)
			{
				std::string name = kStartMenuItemGraphName;
				name += std::to_string(i);
				auto& itemBox = m_drawGraphs[name];
				itemBox.pos.x -= kStartMenuMoveSpeed;

				m_drawGraphs[name] = itemBox;
			}

			//�I�����Ă��鍀�ڂ������摜�����ɓ�����
			auto& selectItemBox = m_drawGraphs[kStartMenuSelectItemGraphName];
			selectItemBox.pos.x -= kStartMenuMoveSpeed;

			//���j���[�摜�̍��W���X�V
			m_drawGraphs[kStartMenuGraphName] = menu;

			//�I�����Ă��鍀�ڂ������摜���X�V
			m_drawGraphs[kStartMenuSelectItemGraphName] = selectItemBox;
		}

		//�Z���N�g���j���[�̉摜�����ɓ�����
		GraphData menu = m_drawGraphs[kSelectMenuGraphName];
		menu.pos.x -= kSelectMenuMoveSpeed;

		//�N�����v
		menu.pos.x = fmax(menu.pos.x, static_cast<float>(kSelectMenuPosX));

		//���̍��W�܂ŗ�����
		if (menu.pos.x == static_cast<float>(kSelectMenuPosX))
		{
			//�I���ł���悤�ɂ���
			m_isSelectSelectMenu = true;
		}

		//���j���[�摜�̍��W���X�V
		m_drawGraphs[kSelectMenuGraphName] = menu;

		//�I���ł��鍀�ڂ𐔂������[�v
		for (int i = 0; i < m_tutorialSelectMenuData[m_selectMenuIndexX].size() + 1; i++)
		{
			//�I���ł��鍀�ڂ������摜
			std::string name = kSelectMenuItemGraphName;
			name += std::to_string(i);
			auto& itemBox = m_drawGraphs[name];
			//�I���ł��鍀�ڂ����ɓ�����
			itemBox.pos.x -= kSelectMenuMoveSpeed;
			//�N�����v
			itemBox.pos.x = fmax(itemBox.pos.x, static_cast<float>(kSelectMenuStringPosX));
			//�I���ł��鍀�ڂ��X�V
			m_drawGraphs[name] = itemBox;
		}

		//�I�����Ă��鍀�ڂ����ɓ�����
		auto& selectItemBox = m_drawGraphs[kSelectMenuSelectItemGraphName];
		selectItemBox.pos.x -= kSelectMenuMoveSpeed;

		//�N�����v
		selectItemBox.pos.x = fmax(selectItemBox.pos.x, static_cast<float>(kSelectMenuStringPosX));

		//�I�����Ă��鍀�ڂ��X�V
		m_drawGraphs[kSelectMenuSelectItemGraphName] = selectItemBox;

		//LB�{�^�������ɓ�����
		auto& lbButton = m_drawGraphs[kSelectMenuLBGraphName];
		lbButton.pos.x -= kSelectMenuMoveSpeed;

		//�N�����v
		lbButton.pos.x = fmax(lbButton.pos.x, static_cast<float>(kSelectMenuPosX - kSelectMenuRbLbDistance));

		//LB�{�^�����X�V
		m_drawGraphs[kSelectMenuLBGraphName] = lbButton;

		//RB�{�^�������ɓ�����
		auto& rbButton = m_drawGraphs[kSelectMenuRBGraphName];
		rbButton.pos.x -= kSelectMenuMoveSpeed;

		//�N�����v
		rbButton.pos.x = fmax(rbButton.pos.x, static_cast<float>(kSelectMenuPosX + kSelectMenuRbLbDistance));

		//RB�{�^�����X�V
		m_drawGraphs[kSelectMenuRBGraphName] = rbButton;

	}
	else
	{
		auto input = MyEngine::Input::GetInstance().GetInputData(0);

		//�O�̃t���[���őI�����Ă��鍀��
		int lastSelectMenuIndexX = m_selectMenuIndexX;
		int lastSelectMenuIndexY = m_selectMenuIndexY;

		//RBLB���͂őI�����Ă���`���[�g���A���̎�ނ�ύX����
		if (input->IsTrigger("RB"))
		{
			//CSV�f�[�^�̔z�u�̓s����+2����
			m_selectMenuIndexX += 2;
			//��ԉE�ŉE���͂����ꍇ
			if (m_selectMenuIndexX > m_tutorialSelectMenuData.size() - 2)
			{
				//��ԍ��Ɉړ�����
				m_selectMenuIndexX = 0;
			}
			//�I�����Ă��鍀�ڂ����Z�b�g����
			m_selectMenuIndexY = 0;
		}
		else if (input->IsTrigger("LB"))
		{
			//CSV�f�[�^�̔z�u�̓s����-2����
			m_selectMenuIndexX -= 2;
			//��ԍ��ō����͂����ꍇ
			if (m_selectMenuIndexX < 0)
			{
				//��ԉE�Ɉړ�����
				m_selectMenuIndexX = m_tutorialSelectMenuData.size() - 2;
			}
			//�I�����Ă��鍀�ڂ����Z�b�g����
			m_selectMenuIndexY = 0;
		}

		//�I�����Ă��鍀�ڂ��ω����Ă�����
		if (m_selectMenuIndexX != lastSelectMenuIndexX)
		{
			//�T�E���h���Đ�
			SoundManager::GetInstance().PlayOnceSound("Select");
		}



		//�㉺���͂őI�����Ă�����̂�ύX����
		if (input->IsTrigger("Up"))
		{
			m_selectMenuIndexY--;

			//��ԏ�ŏ���͂����ꍇ
			if (m_selectMenuIndexY < 0)
			{
				//��ԉ��Ɉړ�����(CSV�̃f�[�^�̔z�u�̓s����Index��+1����)
				m_selectMenuIndexY = m_tutorialSelectMenuData[m_selectMenuIndexX + 1].size() - 1;
			}
		}
		else if (input->IsTrigger("Down"))
		{
			m_selectMenuIndexY++;

			//��ԉ��œ��͂��ꂽ�ꍇ(CSV�̃f�[�^�̔z�u�̓s����Index��+1����)
			if (m_selectMenuIndexY > m_tutorialSelectMenuData[m_selectMenuIndexX + 1].size() - 1)
			{
				//��ԏ�Ɉړ�����
				m_selectMenuIndexY = 0;
			}
		}

		//���s�[�g����
		if (m_selectItemMoveTime > kRepeatInterval)
		{
			if (input->GetPressTime("Up") > kRepeatInputTime)
			{
				m_selectMenuIndexY--;
			}
			else if (input->GetPressTime("Down") > kRepeatInputTime)
			{
				m_selectMenuIndexY++;
			}
		}

		//�ő�l�ƍŏ��l�𒴂��Ȃ��悤��
		m_selectMenuIndexY = max(m_selectMenuIndexY, 0);
		m_selectMenuIndexY = min(m_selectMenuIndexY, m_tutorialSelectMenuData[m_selectMenuIndexX + 1].size() - 1);

		//�I�����Ă��鍀�ڂ�\���摜
		auto& selectItemBox = m_drawGraphs[kSelectMenuSelectItemGraphName];

		//�I�����Ă��鍀�ڂ��ω����Ă�����
		if (m_selectMenuIndexY != lastSelectMenuIndexY)
		{
			//�ω����Ă��牽�t���[���������������Z�b�g����
			m_selectItemMoveTime = 0;

			//�I�����Ă��鍀�ڂ������摜�𓮂���
			MyEngine::Vector2 pos = MyEngine::Vector2(kSelectMenuStringPosX, kSelectMenuStringPosY);
			pos.y += kSelectMenuDistanceY * m_selectMenuIndexY;

			//���W��ݒ�
			selectItemBox.pos = pos;

			//�T�E���h���Đ�
			SoundManager::GetInstance().PlayOnceSound("Select");
		}
		//�I�����Ă��鍀�ڂ��ω����Ă��Ȃ����
		else
		{
			m_selectItemMoveTime++;
		}
	}


}

void TutorialUi::UpdatePlayMenu()
{
	//����
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//�I�����Ă��鍀��
	int selectItem = static_cast<int>(m_playMenuSelectItem);

	//�㉺���͂őI�����Ă�����̂�ύX����
	if (input->IsTrigger("Up"))
	{
		selectItem--;

		//��ԏ�ŏ���͂����ꍇ
		if (selectItem < 0)
		{
			//��ԉ��Ɉړ�����
			selectItem = static_cast<int>(TutorialUi::PlayMenuItem::kItemEnd);
		}
	}
	else if (input->IsTrigger("Down"))
	{
		selectItem++;

		//��ԉ��œ��͂��ꂽ�ꍇ
		if (selectItem > static_cast<int>(TutorialUi::PlayMenuItem::kItemEnd))
		{
			//��ԏ�Ɉړ�����
			selectItem = 0;
		}
	}

	//���s�[�g����
	if (m_selectItemMoveTime > kRepeatInterval)
	{
		if (input->GetPressTime("Up") > kRepeatInputTime)
		{
			selectItem--;
		}
		else if (input->GetPressTime("Down") > kRepeatInputTime)
		{
			selectItem++;
		}
	}

	//�ő�l�ƍŏ��l�𒴂��Ȃ��悤��
	selectItem = max(selectItem, 0);
	selectItem = min(selectItem, static_cast<int>(PlayMenuItem::kItemEnd));


	//�I�����Ă��鍀�ڂ�\���摜
	auto& selectItemBox = m_drawGraphs[kPlayMenuSelectItemGraphName];

	//�I�����Ă��鍀�ڂ������摜���������傫������
	selectItemBox.scale += kSelectItemBoxScalingSpeed;

	//�傫���Ȃ肷���Ȃ��悤�ɃN�����v
	selectItemBox.scale = min(selectItemBox.scale, 1.0);

	//�I�����Ă��鍀�ڂ��ω����Ă�����
	if (selectItem != static_cast<int>(m_playMenuSelectItem))
	{
		//�ω����Ă��牽�t���[���������������Z�b�g����
		m_selectItemMoveTime = 0;

		//�I�����Ă��鍀�ڂ������摜����������������
		selectItemBox.scale = kSelectItemBoxMinScale;

		//�I�����Ă��鍀�ڂ������摜�𓮂���
		MyEngine::Vector2 pos = MyEngine::Vector2(kPlayMenuStringPosX, kPlayMenuStringPosY);
		pos.y += kPlayMenuStringDistanceY * selectItem;

		//���W��ݒ�
		selectItemBox.pos = pos;

		//�T�E���h���Đ�
		SoundManager::GetInstance().PlayOnceSound("Select");

	}
	//�I�����Ă��鍀�ڂ��ω����Ă��Ȃ����
	else
	{
		m_selectItemMoveTime++;
	}

	//���E�ɓ������鍀�ڂł����
	if (m_playMenuSelectItem == PlayMenuItem::kChangeTutorial ||
		m_playMenuSelectItem == PlayMenuItem::kChangeMode)
	{
		int last = -1;
		int now = -1;

		//�I�����Ă��鍀��
		if (m_playMenuSelectItem == PlayMenuItem::kChangeTutorial)
		{
			last = m_selectTutorialNumber;
			now = m_selectTutorialNumber;
		}
		else if (m_playMenuSelectItem == PlayMenuItem::kChangeMode)
		{
			last = m_nowTutorialMode;
			now = m_nowTutorialMode;
		}

		//�E����������
		if (input->IsTrigger("Right"))
		{
			now++;


		}
		//������������
		else if (input->IsTrigger("Left"))
		{
			now--;
		}

		if (m_playMenuSelectItem == PlayMenuItem::kChangeTutorial)
		{
			//��ԉE�ŉE���͂����ꍇ
			if (now >= static_cast<int>(TutorialManager::TutorialKind::kTutorialNum))
			{
				//��ԍ��Ɉړ�����
				now = 0;
			}
			//��ԍ��ō����͂����ꍇ
			else if (now < 0)
			{
				//��ԉE�Ɉړ�����
				now = static_cast<int>(TutorialManager::TutorialKind::kTutorialNum) - 1;
			}
		}
		else if (m_playMenuSelectItem == PlayMenuItem::kChangeMode)
		{
			//��ԉE�ŉE���͂����ꍇ
			if (now >= static_cast<int>(TutorialManager::TutorialMode::kModeNum))
			{
				//��ԍ��Ɉړ�����
				now = 0;
			}
			//��ԍ��ō����͂����ꍇ
			else if (now < 0)
			{
				//��ԉE�Ɉړ�����
				now = static_cast<int>(TutorialManager::TutorialMode::kModeNum) - 1;
			}
		}

		//���s�[�g����
		if (m_selectItemMoveTime > kPlayMenuChangeTutorialRepeatInputTime)
		{
			if (input->GetPressTime("Right") > kPlayMenuChangeTutorialRepeatInputTime)
			{
				now++;
			}
			else if (input->GetPressTime("Left") > kPlayMenuChangeTutorialRepeatInputTime)
			{
				now--;
			}
		}

		if (m_playMenuSelectItem == PlayMenuItem::kChangeTutorial)
		{
			//�N�����v
			now = max(now, 0);
			now = min(now, static_cast<int>(TutorialManager::TutorialKind::kTutorialNum) - 1);
		}
		else if (m_playMenuSelectItem == PlayMenuItem::kChangeMode)
		{
			//�N�����v
			now = max(now, 0);
			now = min(now, static_cast<int>(TutorialManager::TutorialMode::kModeNum) - 1);
		}

		//�I�����Ă��鍀�ڂ��ω����Ă�����
		if (last != now)
		{
			//�ω����Ă��牽�t���[���������������Z�b�g����
			m_selectItemMoveTime = 0;

			//�T�E���h���Đ�
			SoundManager::GetInstance().PlayOnceSound("Select");
		}

		//�I�����Ă��鍀�ڂ��X�V
		if (m_playMenuSelectItem == PlayMenuItem::kChangeTutorial)
		{
			m_selectTutorialNumber = now;
		}
		else if (m_playMenuSelectItem == PlayMenuItem::kChangeMode)
		{
			m_nowTutorialMode = now;
		}
	}

	//���E�̖���h�炷
	m_selectTutorialStringArrowPos += kPlayMenuTutorialNameSideArrowSpeed;

	//�I�����Ă��鍀�ڂ��X�V
	m_playMenuSelectItem = static_cast<PlayMenuItem>(selectItem);

	//���j���[�摜�̃A���t�@�l�����Z���Ă���
	auto& menu = m_drawGraphs[kPlayMenuGraphName];

	menu.alpha += kMenuFadeSpeed;
	//�ő�l�𒴂��Ȃ��悤�ɃN�����v
	menu.alpha = min(menu.alpha, 255);

}

void TutorialUi::UpdateStart()
{
}

void TutorialUi::UpdatePlaying()
{
	//�������Ă�����
	if (m_isSuccessTutorial)
	{
		InitSuccess();
	}
}

void TutorialUi::UpdateSuccess()
{
	//�N���A���̉摜�����������������Ă���
	auto& success = m_drawGraphs[kSuccessGraphName];

	success.scale -= kSuccessGraphScalingSpeed;

	//�N�����v
	success.scale = max(success.scale, kSuccessGraphFinalScale);

	//�ŏ��܂ŏ������Ȃ�����
	if (success.scale == kSuccessGraphFinalScale)
	{
		//�����h�炷
		if (m_successTime < kSuccessGraphShakeTime)
		{
			success.pos.x = static_cast<float>((kButtonPosX + GetRand(kSuccessGraphShakeScale)) - kSuccessGraphShakeHalfScale);
			success.pos.y = static_cast<float>((kButtonPosY + GetRand(kSuccessGraphShakeScale)) - kSuccessGraphShakeHalfScale);
		}

		if (m_successTime > kSuccessEndTime)
		{
			//���o���I������t���O�𗧂Ă�
			m_isSuccessEnd = true;
		}

		m_successTime++;
	}
}
