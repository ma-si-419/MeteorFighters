#include "TutorialUi.h"
#include "LoadCsv.h"
#include "TutorialManager.h"
#include "GraphManager.h"
#include "Input.h"

namespace
{
	//�t�H���g�̖��O
	const TCHAR* kFontName = "GN-�L���SU����NB";

	//�X�^�[�g���ɕ\������t�H���g�̃T�C�Y
	constexpr int kStartFontSize = 84;
	//�v���C���̐�����\������t�H���g�̃T�C�Y
	constexpr int kPlayingFontSize = 32;
	//���j���[�̕����̃t�H���g�̃T�C�Y
	constexpr int kMenuFontSize = 32;

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
	constexpr int kButtonInterval = 80;

	//���j���[���o�Ă���Ƃ��̃t�F�[�h�C���̑���
	constexpr int kMenuFadeSpeed = 20;

	//���j���[��\�����Ă���Ƃ��̃Q�[���V�[���ɂ����鍕���{�b�N�X�̍ő�A���t�@�l
	constexpr int kBackBoxMaxAlpha = 150;

	//�㉺���͂Ń��s�[�g���͂��󂯕t���鎞��
	constexpr int kRepeatInputTime = 15;

	//���s�[�g���͂̊Ԋu
	constexpr int kRepeatInterval = 5;

	//���j���[�̕�����\��������W
	constexpr int kMenuStringPosX = Game::kWindowWidth / 2;

	//���E�ŕύX�ł��郁�j���[�̕�����\��������W
	constexpr int kMoveMenuStringPosX = 500;

	//���j���[�Ń`���[�g���A���̖��O��\��������W
	constexpr int kMenuTutorialNamePosX = 1100;

	//��ڂ̍���
	constexpr int kMenuStringPosY = 250;

	//���j���[�̕�����\������Ԋu
	constexpr int kMenuStringDistanceY = 130;

	//�\�����镶����
	const std::string kMenuStrings[static_cast<int>(TutorialUi::MenuItem::kItemNum)] =
	{
		"�󋵂����Z�b�g����",
		"�`���[�g���A����ύX",
		"���j���[�����",
		"�`���[�g���A�����I������"
	};

	//�I�����Ă��鍀�ڂ𓮂������Ƃ��I�����Ă��鍀�ڂ��ǂ��܂ŏ��������邩
	constexpr double kSelectItemBoxMinScale = 0.7;

	//�I�����Ă��鍀�ڂ��傫���Ȃ��Ă������x
	constexpr double kSelectItemBoxScalingSpeed = 0.05;
}

TutorialUi::TutorialUi() :
	m_drawButtonNum(0),
	m_selectItem(static_cast<MenuItem>(0)),
	m_tutorialNumber(0),
	m_selectItemMoveTime(0)
{
	LoadCsv load;

	m_tutorialData = load.LoadFile("data/csv/tutorialData.csv");

	m_updateFunc = &TutorialUi::UpdateMenu;

	m_startFontHandle = CreateFontToHandle(kFontName, kStartFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 3);
	m_playingFontHandle = CreateFontToHandle(kFontName, kPlayingFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 3);
	m_menuFontHandle = CreateFontToHandle(kFontName, kMenuFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 2);
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
	DeleteFontToHandle(m_startFontHandle);
	DeleteFontToHandle(m_playingFontHandle);
	DeleteFontToHandle(m_menuFontHandle);
}

void TutorialUi::InitMenu()
{
	//�o�^���Ă���摜�����ׂď���
	m_drawGraphs.clear();

	//�摜�}�l�[�W���[
	auto& manager = GraphManager::GetInstance();

	//���j���[��ʂ̉摜��o�^����
	GraphData menu;
	menu.handle = manager.GetHandle("Menu");
	menu.pos = MyEngine::Vector2(Game::kWindowWidth / 2, Game::kWindowHeight / 2);
	menu.alpha = 0;
	m_drawGraphs["Menu"] = menu;

	//�A�b�v�f�[�g��ύX����
	m_updateFunc = &TutorialUi::UpdateMenu;

	//���j���[��ʂőI�����Ă��鍀�ڂ������摜��o�^����
	GraphData selectItemBox;
	selectItemBox.handle = manager.GetHandle("SelectItem");
	selectItemBox.pos = MyEngine::Vector2(kMenuStringPosX, kMenuStringPosY);
	m_drawGraphs["SelectItemBox"] = selectItemBox;

	//�I�����Ă��鍀�ڂ���ԏ�ɂ���
	m_selectItem = static_cast<MenuItem>(0);

}

void TutorialUi::InitStart()
{
	//�o�^���Ă���摜�����ׂď���
	m_drawGraphs.clear();

	m_updateFunc = &TutorialUi::UpdateStart;

	auto& manager = GraphManager::GetInstance();

	GraphData data;

	data.pos = MyEngine::Vector2(Game::kWindowWidth / 2, Game::kWindowHeight / 2);

	data.handle = manager.GetHandle("BackBox");

	m_drawGraphs["BackBox"] = data;
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

	frame.handle = manager.GetHandle("ButtonFrame");

	m_drawGraphs["ButtonFrame"] = frame;

	auto buttonString = m_tutorialData[number][static_cast<int>(TutorialManager::TutorialDataIndex::kButton)];

	auto button = stringFunc.Split(buttonString, '/');

	m_drawButtonNum = button.size();

	//�{�^���̕`�搔��1�ł����
	if (m_drawButtonNum == 1)
	{
		GraphData buttonData;

		buttonData.handle = manager.GetHandle(button[0]);

		buttonData.pos = MyEngine::Vector2(kButtonPosX, kButtonPosY);

		m_drawGraphs["Button0"] = buttonData;
	}
	//�{�^���̕`�搔�������ł����
	else if (m_drawButtonNum % 2 == 0)
	{
		//�`��J�n�ʒu���v�Z����
		int posX = kButtonPosX - (kButtonInterval * m_drawButtonNum / 2);
		posX += kButtonInterval / 2;

		//�`�搔�������[�v
		for (int i = 0; i < m_drawButtonNum; i++)
		{
			//�{�^����`�悷��
			DrawRotaGraph(posX, kButtonPosY, 1.0, 0.0, manager.GetHandle(button[i]), true);

			GraphData buttonData;

			buttonData.handle = manager.GetHandle(button[i]);

			buttonData.pos = MyEngine::Vector2(posX, kButtonPosY);

			std::string name = "Button" + std::to_string(i);

			m_drawGraphs[name] = buttonData;

			posX += kButtonInterval;
		}
	}
	//��̏ꍇ
	else
	{

	}

}

void TutorialUi::DrawMenu()
{
	GraphData menu = m_drawGraphs["Menu"];

	//��ʑS�̂𕢂������{�b�N�X�̃A���t�@�l
	int backBoxAlpha = menu.alpha;

	backBoxAlpha = min(backBoxAlpha, kBackBoxMaxAlpha);

	//�u�����h���[�h�ύX
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, backBoxAlpha);

	//��ʑS�̂𕢂������{�b�N�X�\��
	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, GetColor(0, 0, 0), true);

	//�A���t�@�l�ύX
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, menu.alpha);

	DrawRotaGraph(menu.pos.x, menu.pos.y, menu.scale, 0.0, menu.handle, true);

	//�u�����h���[�h�����ɖ߂�
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//�I�����Ă��鍀�ڂ̌��̃{�b�N�X��\������
	GraphData itemBox = m_drawGraphs["SelectItemBox"];

	DrawRotaGraph(itemBox.pos.x,itemBox.pos.y,itemBox.scale,0.0,itemBox.handle,true);

	//�������\������
	for (int i = 0; i < static_cast<int>(TutorialUi::MenuItem::kItemNum); i++)
	{
		//��ԏ�̕����̍��W
		MyEngine::Vector2 pos = MyEngine::Vector2(kMenuStringPosX, kMenuStringPosY);

		//������Y���W�����炷
		pos.y += kMenuStringDistanceY * i;

		//��ڂ̕����̍��W�����ɂ��炷(�ʂ̏����ɂ�����)

		if (i == static_cast<int>(TutorialUi::MenuItem::kChangeTutorial))
		{
			pos.x = kMoveMenuStringPosX;
		}

		DrawStringCenter(kMenuStrings[i], pos, m_menuFontHandle, GetColor(0, 0, 0), GetColor(255, 255, 255));
	}
}

void TutorialUi::DrawStart(int number)
{
	GraphData backBox = m_drawGraphs["BackBox"];

	DrawRotaGraph(backBox.pos.x, backBox.pos.y, backBox.scale, 0.0, backBox.handle, true);

	std::string startWord = m_tutorialData[number][static_cast<int>(TutorialManager::TutorialDataIndex::kStartString)];

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

			shift.y += GetFontSizeToHandle(m_startFontHandle) * i;

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
	GraphData frameData = m_drawGraphs["ButtonFrame"];

	//�{�^����\������t���[��
	DrawRotaGraph(frameData.pos.x, frameData.pos.y, frameData.scale, 0.0, frameData.handle, true);

	//�v���C���̐���
	auto playingString = m_tutorialData[number][static_cast<int>(TutorialManager::TutorialDataIndex::kPlayingString)];

	//�v���C���̐�����`��
	DrawStringCenter(playingString, MyEngine::Vector2(kPlayingStringPosX, kPlayingStringPosY), m_playingFontHandle, GetColor(255, 255, 255), GetColor(0, 0, 0));

	//�����{�^����\������
	for (int i = 0; i < m_drawButtonNum; i++)
	{
		std::string name = "Button" + std::to_string(i);

		GraphData buttonData = m_drawGraphs[name];

		DrawRotaGraph(buttonData.pos.x, buttonData.pos.y, buttonData.scale, 0.0, buttonData.handle, true);
	}
}

void TutorialUi::DrawStringCenter(std::string string, MyEngine::Vector2 centerPos, int font, int color, int edgeColor)
{
	//������\��������W
	MyEngine::Vector2 pos;

	//������
	int wordNum = GetStringLength(string.c_str());

	//�������������̏ꍇ
	if (wordNum % 2 == 0)
	{
		//�����̍��W�����ɂ��炷
		pos.x = centerPos.x - static_cast<float>(GetFontSizeToHandle(font) * wordNum / 2);

		//�����̑傫���̔���������ɂ��炷
		pos.y = centerPos.y - static_cast<float>(GetFontSizeToHandle(font) / 2);
	}
	//��̏ꍇ
	else
	{
		wordNum++;

		//�����̍��W�����ɂ��炷
		pos.x = centerPos.x - static_cast<float>(GetFontSizeToHandle(font) * wordNum / 2);
		pos.x += GetFontEdgeSizeToHandle(font) / 2;

		//�����̑傫���̔���������ɂ��炷
		pos.y = centerPos.y - static_cast<float>(GetFontSizeToHandle(font) / 2);
	}

	//�����̕`��
	DrawStringToHandle(static_cast<int>(pos.x), static_cast<int>(pos.y), string.c_str(), color, font, edgeColor);
}

void TutorialUi::UpdateMenu()
{
	//����
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//�I�����Ă��鍀��
	int selectItem = static_cast<int>(m_selectItem);



	//�㉺���͂őI�����Ă�����̂�ύX����
	if (input->IsTrigger("Up"))
	{
		selectItem--;

		//��ԏ�ŏ���͂����ꍇ
		if (selectItem < 0)
		{
			//��ԉ��Ɉړ�����
			selectItem = static_cast<int>(TutorialUi::MenuItem::kItemEnd);
		}
	}
	else if (input->IsTrigger("Down"))
	{
		selectItem++;

		//��ԉ��œ��͂��ꂽ�ꍇ
		if (selectItem > static_cast<int>(TutorialUi::MenuItem::kItemEnd))
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
	selectItem = min(selectItem, static_cast<int>(MenuItem::kItemEnd));


	//�I�����Ă��鍀�ڂ�\���摜
	auto& selectItemBox = m_drawGraphs["SelectItemBox"];

	//�I�����Ă��鍀�ڂ������摜���������傫������
	selectItemBox.scale += kSelectItemBoxScalingSpeed;

	//�傫���Ȃ肷���Ȃ��悤�ɃN�����v
	selectItemBox.scale = min(selectItemBox.scale, 1.0);

	//�I�����Ă��鍀�ڂ��ω����Ă�����
	if (selectItem != static_cast<int>(m_selectItem))
	{
		//�ω����Ă��牽�t���[���������������Z�b�g����
		m_selectItemMoveTime = 0;

		//�I�����Ă��鍀�ڂ������摜����������������
		selectItemBox.scale = kSelectItemBoxMinScale;

		//�I�����Ă��鍀�ڂ������摜�𓮂���
		MyEngine::Vector2 pos = MyEngine::Vector2(kMenuStringPosX, kMenuStringPosY);
		pos.y += kMenuStringDistanceY * selectItem;

		//���W��ݒ�
		selectItemBox.pos = pos;
	}
	//�I�����Ă��鍀�ڂ��ω����Ă��Ȃ����
	else
	{
		m_selectItemMoveTime++;
	}
	
	//�I�����Ă��鍀�ڂ��X�V
	m_selectItem = static_cast<MenuItem>(selectItem);

	//���j���[�摜�̃A���t�@�l�����Z���Ă���
	auto& menu = m_drawGraphs["Menu"];

	menu.alpha += kMenuFadeSpeed;
	//�ő�l�𒴂��Ȃ��悤�ɃN�����v
	menu.alpha = min(menu.alpha, 255);

}

void TutorialUi::UpdateStart()
{
}

void TutorialUi::UpdatePlaying()
{
}