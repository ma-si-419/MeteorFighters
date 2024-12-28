#include "TutorialUi.h"
#include "LoadCsv.h"
#include "TutorialManager.h"
#include "GraphManager.h"

namespace
{
	//�t�H���g�̖��O
	const TCHAR* kFontName = "GN-�L���SU����NB";

	//�X�^�[�g���ɕ\������t�H���g�̃T�C�Y
	constexpr int kStartFontSize = 84;
	constexpr int kPlayingFontSize = 32;

	//�`���[�g���A���̃{�^����\��������W�̃t���[��
	constexpr int kButtonFramePosX = Game::kWindowWidth / 2;
	constexpr int kButtonFramePosY = 150;

	//�v���C���̃`���[�g���A���̐�����\��������WY
	constexpr int kPlayingStringPosX = Game::kWindowWidth / 2;
	constexpr int kPlayingStringPosY = 210;

	//���̃{�^���������΂����̂���\��������W
	constexpr int kButtonPosX = Game::kWindowWidth / 2;
	constexpr int kButtonPosY = 135;

	//�{�^����\������Ԋu
	constexpr int kButtonInterval = 80;
}

TutorialUi::TutorialUi()
{
	LoadCsv load;

	m_tutorialData = load.LoadFile("data/csv/tutorialData.csv");

	m_updateFunc = &TutorialUi::UpdateMenu;

	m_startFontHandle = CreateFontToHandle(kFontName, kStartFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 3);
	m_playingFontHandle = CreateFontToHandle(kFontName, kPlayingFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 3);
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
}

void TutorialUi::InitMenu()
{
	//�o�^���Ă���摜�����ׂď���
	m_drawGraphs.clear();

	m_updateFunc = &TutorialUi::UpdateMenu;
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

			buttonData.pos = MyEngine::Vector2(posX,kButtonPosY);

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

}

void TutorialUi::DrawStart(int number)
{
	GraphData data = m_drawGraphs["BackBox"];

	DrawRotaGraph(data.pos.x,data.pos.y, data.scale, 0.0, data.handle, true);

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
		pos.x = centerPos.x - static_cast<int>(GetFontSizeToHandle(font) * wordNum / 2);

		//�����̑傫���̔���������ɂ��炷
		pos.y = centerPos.y - static_cast<int>(GetFontSizeToHandle(font) / 2);
	}
	//��̏ꍇ
	else
	{
		wordNum++;

		//�����̍��W�����ɂ��炷
		pos.x = centerPos.x - static_cast<int>(GetFontSizeToHandle(font) * wordNum / 2);
		pos.x += GetFontEdgeSizeToHandle(font) / 2;

		//�����̑傫���̔���������ɂ��炷
		pos.y = centerPos.y - static_cast<int>(GetFontSizeToHandle(font) / 2);
	}

	//�����̕`��
	DrawStringToHandle(pos.x, pos.y, string.c_str(), color, font, edgeColor);
}

void TutorialUi::UpdateMenu()
{
}

void TutorialUi::UpdateStart()
{
}

void TutorialUi::UpdatePlaying()
{
}
