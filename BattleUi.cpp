#include "BattleUi.h"
#include "BattleManager.h"
#include "GraphManager.h"
#include "SoundManager.h"
#include "Input.h"

namespace
{
	//���j���[��\��������W
	constexpr int kMenuPosX = Game::kWindowWidth / 2;
	constexpr int kMenuPosY = Game::kWindowHeight / 2;

	//���U���g��win��lose��\��������W
	constexpr int kResultLogoPosX = Game::kWindowWidth / 2;
	constexpr int kResultLogoPosY = 700;

	//���U���g�̃��S��\������܂ł̎���
	constexpr int kResultDisplayStartTime = 90;

	//���U���g�̃��j���[��\������܂ł̎���
	constexpr int kResultMenuDisplayStartTime = 360;

	//���U���g�̃��j���[�̃t�F�[�h�C���̑��x
	constexpr int kMenuFadeinSpeed = 20;

	//���U���g�̌��̍����{�b�N�X�̍ő�A���t�@�l
	constexpr int kMenuBackBoxMaxAlpha = 125;

	//���U���g�̃��S�̍ŏ��̊g�嗦
	constexpr double kResultLogoInitScale = 3.0;

	//���U���g�̃��S�̍Ō�̊g�嗦
	constexpr double kResultLogoFinalScale = 1.0;

	//���U���g�̃��S�̏k�����x
	constexpr double kResultLogoScallingSpeed = 0.12;

	//���U���g�̃��S��h�炷����
	constexpr int kResultLogoShakeTime = 6;

	//���U���g�̃��S��h�炷�傫��
	constexpr int kResultLogoShakeScale = 18;

	//�㉺���͂Ń��s�[�g���͂��󂯕t���鎞��
	constexpr int kRepeatInputTime = 15;

	//���s�[�g���͂̊Ԋu
	constexpr int kRepeatInterval = 5;

	//�t�H���g�̖��O
	const TCHAR* kFontName = "GN-�L���SU����NB";

	//�t�H���g�̃T�C�Y
	constexpr int kResultMenuFontSize = 50;

	//���j���[�̕�����\��������W
	constexpr int kPlayMenuStringPosX = Game::kWindowWidth / 2;
	
	//���j���[�̕���
	const std::string kMenuString[static_cast<int>(BattleUi::MenuItem::kItemNum)] =
	{
		"�o�g���ɖ߂�",
		"�ŏ������蒼��",
		"�L�����N�^�[��I�тȂ���",
		"���j���[�ɖ߂�"
	};

	constexpr int kPlayMenuStringPosY[static_cast<int>(BattleUi::MenuItem::kItemNum)] =
	{
		200,
		380,
		560,
		740
	};

	//���U���g�̕���
	const std::string kResultString[static_cast<int>(BattleUi::ResultItem::kItemNum)] =
	{
		"������x�키",
		"�L�����N�^�[��I�тȂ���",
		"���j���[�ɖ߂�"
	};
	
	//���U���g�̕�����\��������W
	constexpr int kResultStringPosY[static_cast<int>(BattleUi::ResultItem::kItemNum)] =
	{
		250,
		430,
		610
	};



	//�I�����Ă��鍀�ڂ𓮂������Ƃ��I�����Ă��鍀�ڂ��ǂ��܂ŏ��������邩
	constexpr double kSelectItemBoxMinScale = 0.7;

	//�ǂ��܂ő傫�����邩
	constexpr double kSelectItemBoxMaxScale = 1.25;


	//�I�����Ă��鍀�ڂ��傫���Ȃ��Ă������x
	constexpr double kSelectItemBoxScalingSpeed = 0.06;

}

BattleUi::BattleUi() :
	m_selectItemMoveTime(0),
	m_decisionItem(-1),
	m_isWin(true),
	m_resultLogoScale(kResultLogoInitScale),
	m_resultTime(0),
	m_shakeTime(0),
	m_selectItem(0),
	m_resultMenuAlpha(0),
	m_menuFontHandle(-1),
	m_selectItemBoxScale(kSelectItemBoxMaxScale),
	m_menuAlpha(0),
	m_isResultMenuDisplay(false)
{
	//�ŏ��͉������Ȃ��悤�ɐݒ肵�Ă���
	m_updateFunc = &BattleUi::None;
	m_drawFunc = &BattleUi::None;

	m_menuFontHandle = CreateFontToHandle(kFontName, kResultMenuFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 3);
}

BattleUi::~BattleUi()
{

}

void BattleUi::Update()
{
	(this->*m_updateFunc)();
}

void BattleUi::Draw()
{
	(this->*m_drawFunc)();
}

void BattleUi::ChangeSituation(UiSituation situation)
{
	//���������Ă���
	m_decisionItem = -1;
	m_selectItem = 0;
	m_menuAlpha = 0;
	m_resultMenuAlpha = 0;
	m_resultTime = 0;
	m_resultLogoScale = kResultLogoInitScale;
	m_shakeTime = 0;
	m_selectItemBoxScale = kSelectItemBoxMaxScale;
	m_isResultMenuDisplay = false;

	switch (situation)
	{
	case BattleUi::UiSituation::kMenu:

		//���j���[
		m_updateFunc = &BattleUi::UpdateMenu;
		m_drawFunc = &BattleUi::DrawMenu;

		break;

	case BattleUi::UiSituation::kResult:

		//���U���g��
		m_updateFunc = &BattleUi::UpdateResult;
		m_drawFunc = &BattleUi::DrawResult;

		break;

	case BattleUi::UiSituation::kNone:

		//�������Ȃ��ꍇ
		m_updateFunc = &BattleUi::None;
		m_drawFunc = &BattleUi::None;

		break;


	default:
		break;
	}
}

void BattleUi::UpdateMenu()
{

	//���j���[�̃t�F�[�h�C�����I���܂ő���s��
	if (m_menuAlpha < 255) return;

	m_selectItemBoxScale += kSelectItemBoxScalingSpeed;

	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	auto lastItem = m_selectItem;

	//A�{�^�����������Ƃ��ɑI������Ă��鍀�ڂ����s����
	if (input->IsTrigger("A"))
	{
		m_decisionItem = m_selectItem;

		//�T�E���h���Đ�
		SoundManager::GetInstance().PlayOnceSound("Ok");
	}
	//�㉺���͂��󂯎��
	if (input->IsTrigger("Down"))
	{
		m_selectItem++;

	}
	else if (input->IsTrigger("Up"))
	{
		m_selectItem--;

	}

	//���s�[�g����
	if (m_selectItemMoveTime > kRepeatInterval)
	{
		if (input->GetPressTime("Up") > kRepeatInputTime)
		{
			m_selectItem--;
		}
		else if (input->GetPressTime("Down") > kRepeatInputTime)
		{
			m_selectItem++;
		}
	}

	//�N�����v
	m_selectItem = min(m_selectItem, static_cast<int>(MenuItem::kItemEnd));
	m_selectItem = max(m_selectItem, 0);

	//�O�̃t���[���Ƃ���J�[�\���𓮂����Ă�����
	if (lastItem != m_selectItem)
	{
		m_selectItemMoveTime = 0;

		//�I�����Ă��鍀�ڂ̌��̃{�b�N�X�̃T�C�Y���k������
		m_selectItemBoxScale = kSelectItemBoxMinScale;

		//�T�E���h���Đ�
		SoundManager::GetInstance().PlayOnceSound("Select");

	}
	//�������Ă��Ȃ����
	else
	{
		m_selectItemMoveTime++;
	}

	//���̃{�b�N�X�̃T�C�Y�̃N�����v
	m_selectItemBoxScale = min(m_selectItemBoxScale, kSelectItemBoxMaxScale);
}

void BattleUi::UpdateResult()
{
	//���U���g�̃��S�̊g�k���I�������瑀��\�ɂ���
	if (m_resultLogoScale != kResultLogoFinalScale) return;

	//���j���[�̃t�F�[�h�C�����I���܂ő���s��
	if (m_resultMenuAlpha < 255) return;

	m_selectItemBoxScale += kSelectItemBoxScalingSpeed;

	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	auto lastItem = m_selectItem;


	//A�{�^�����������Ƃ��ɑI������Ă��鍀�ڂ����s����
	if (input->IsTrigger("A"))
	{
		m_decisionItem = m_selectItem;

		//�T�E���h���Đ�
		SoundManager::GetInstance().PlayOnceSound("Ok");

	}
	//�㉺���͂��󂯎��
	if (input->IsTrigger("Down"))
	{
		m_selectItem++;

	}
	else if (input->IsTrigger("Up"))
	{
		m_selectItem--;

	}

	//���s�[�g����
	if (m_selectItemMoveTime > kRepeatInterval)
	{
		if (input->GetPressTime("Up") > kRepeatInputTime)
		{
			m_selectItem--;
		}
		else if (input->GetPressTime("Down") > kRepeatInputTime)
		{
			m_selectItem++;
		}
	}

	//�N�����v
	m_selectItem = min(m_selectItem, static_cast<int>(ResultItem::kItemEnd));
	m_selectItem = max(m_selectItem, 0);

	//�O�̃t���[���Ƃ���J�[�\���𓮂����Ă�����
	if (lastItem != m_selectItem)
	{
		m_selectItemMoveTime = 0;

		//�I�����Ă��鍀�ڂ̌��̃{�b�N�X�̃T�C�Y���k������
		m_selectItemBoxScale = kSelectItemBoxMinScale;

		//�T�E���h���Đ�����
		SoundManager::GetInstance().PlayOnceSound("Select");

	}
	//�������Ă��Ȃ����
	else
	{
		m_selectItemMoveTime++;
	}

	//���̃{�b�N�X�̃T�C�Y�̃N�����v
	m_selectItemBoxScale = min(m_selectItemBoxScale, kSelectItemBoxMaxScale);

}

void BattleUi::UpdateStart1P()
{
}

void BattleUi::UpdateStart2P()
{
}


void BattleUi::DrawMenu()
{
	auto& graphManager = GraphManager::GetInstance();

	//���j���[���t�F�[�h�C��������
	m_menuAlpha += kMenuFadeinSpeed;

	//��ʑS�̂��Â�����
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, min(m_menuAlpha, kMenuBackBoxMaxAlpha));

	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, GetColor(0, 0, 0), true);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_menuAlpha);

	//���j���[��\������
	DrawRotaGraph(kMenuPosX, kMenuPosY, 1.0, 0.0, graphManager.GetHandle("Menu"), true);

	//�I�����Ă��鍀�ڂ������{�b�N�X��\��
	DrawRotaGraph(kPlayMenuStringPosX, kPlayMenuStringPosY[m_selectItem], m_selectItemBoxScale, 0.0, graphManager.GetHandle("SelectItem"), true);

	//���j���[�̕�����\������
	for (int i = 0; i < static_cast<int>(BattleUi::MenuItem::kItemNum); i++)
	{
		MyEngine::Vector2 pos(kPlayMenuStringPosX, kPlayMenuStringPosY[i]);

		DrawStringCenter(kMenuString[i], pos, m_menuFontHandle, GetColor(255, 255, 255), GetColor(0, 0, 0));;
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}

void BattleUi::DrawResult()
{

	auto& graphManager = GraphManager::GetInstance();

	m_resultTime++;

	//���U���g��ʂɂȂ��Ĉ�莞�Ԃ�������
	if (m_resultTime > kResultDisplayStartTime)
	{
		//���S�̑傫�������������Ă���
		m_resultLogoScale -= kResultLogoScallingSpeed;

		//���ȏ㏬�����Ȃ�Ȃ��悤�ɂ���
		m_resultLogoScale = max(m_resultLogoScale, kResultLogoFinalScale);

		int logoPosX = kResultLogoPosX;
		int logoPosY = kResultLogoPosY;

		//���ȏ㏬�����Ȃ����班���h�炷
		if (m_resultLogoScale == kResultLogoFinalScale)
		{
			if (m_shakeTime < kResultLogoShakeTime)
			{
				//���S�̍��W��h�炷(�����_�����v���X�����o�Ȃ��̂Ŕ������炷)
				logoPosX += GetRand(kResultLogoShakeScale) + static_cast<int>(kResultLogoShakeScale * 0.5);
				logoPosY += GetRand(kResultLogoShakeScale) + static_cast<int>(kResultLogoShakeScale * 0.5);
			}
			m_shakeTime++;
		}

		if (m_isWin)
		{
			DrawRotaGraph(logoPosX, logoPosY, m_resultLogoScale, 0.0, graphManager.GetHandle("Winner"), true);
		}
		else
		{
			DrawRotaGraph(logoPosX, logoPosY, m_resultLogoScale, 0.0, graphManager.GetHandle("Loser"), true);
		}
	}
	//����Ɉ�莞�Ԃ�������
	if (m_resultTime > kResultMenuDisplayStartTime || m_isResultMenuDisplay)
	{
		//���j���[���t�F�[�h�C��������
		m_resultMenuAlpha += kMenuFadeinSpeed;

		//��ʑS�̂��Â�����
		SetDrawBlendMode(DX_BLENDMODE_ALPHA,min(m_resultMenuAlpha,kMenuBackBoxMaxAlpha));

		DrawBox(0,0,Game::kWindowWidth,Game::kWindowHeight,GetColor(0,0,0),true);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA,m_resultMenuAlpha);

		//���j���[��\������
		DrawRotaGraph(kMenuPosX,kMenuPosY,1.0,0.0,graphManager.GetHandle("Menu"),true);
		
		//�I�����Ă��鍀�ڂ������{�b�N�X��\��
		DrawRotaGraph(kPlayMenuStringPosX, kResultStringPosY[m_selectItem], m_selectItemBoxScale, 0.0, graphManager.GetHandle("SelectItem"), true);

		//���j���[�̕�����\������
		for (int i = 0; i < static_cast<int>(BattleUi::ResultItem::kItemNum); i++)
		{
			MyEngine::Vector2 pos(kPlayMenuStringPosX, kResultStringPosY[i]);

			DrawStringCenter(kResultString[i], pos, m_menuFontHandle, GetColor(255, 255, 255), GetColor(0, 0, 0));;
		}

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	}


}

void BattleUi::DrawStart1P()
{

}

void BattleUi::DrawStart2P()
{
}

void BattleUi::DrawStringCenter(std::string string, MyEngine::Vector2 centerPos, int font, int color, int edgeColor)
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
		pos.x = centerPos.x;

		//��������1��������Ȃ����
		if (wordNum > 1)
		{
			//�����̍��W�����ɂ��炷
			pos.x -= static_cast<float>(GetFontSizeToHandle(font) * (wordNum + 1) / 2);
		}
		//���������ꕶ���ł����
		else
		{
			//�����̍��W���ꕶ�������ɂ��炷
			pos.x -= static_cast<float>(GetFontSizeToHandle(font) * wordNum);
		}

		//���p���E�ɓ�����
		pos.x += GetFontSizeToHandle(font) / 2;

		//�����̑傫���̔���������ɂ��炷
		pos.y = centerPos.y - static_cast<float>(GetFontSizeToHandle(font) / 2);
	}

	//�����̕`��
	DrawStringToHandle(static_cast<int>(pos.x), static_cast<int>(pos.y), string.c_str(), color, font, edgeColor);
}
