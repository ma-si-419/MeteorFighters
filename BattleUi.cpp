#include "BattleUi.h"
#include "BattleManager.h"
#include "GraphManager.h"
#include "Input.h"

namespace
{
	constexpr int kMenuDrawPosX = Game::kWindowWidth / 2;
	constexpr int kMenuDrawPosY = Game::kWindowHeight / 2;

	//���U���g��win��lose��\��������W
	constexpr int kResultLogoPosX = Game::kWindowWidth / 2;
	constexpr int kResultLogoPosY = 700;

	//���U���g�̃��S��\������܂ł̎���
	constexpr int kResultDisplayStartTime = 90;

	//���U���g�̃��S�̍ŏ��̊g�嗦
	constexpr double kResultLogoDefaultScale = 3.0;

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
}

BattleUi::BattleUi():
	m_selectItemMoveTime(0)
{
	//�ŏ��͉������Ȃ��悤�ɐݒ肵�Ă���
	m_updateFunc = &BattleUi::None;
	m_drawFunc = &BattleUi::None;
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

}

void BattleUi::UpdateResult()
{
	//���U���g�̃��S�̊g�k���I�������瑀��\�ɂ���
	if (m_resultLogoScale != kResultLogoFinalScale) return;

	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//A�{�^�����������Ƃ��ɑI������Ă��鍀�ڂ����s����
	if (input->IsTrigger("A"))
	{
		m_decisionItem = m_selectItem;
	}
	//�㉺���͂��󂯎��
	if (input->IsTrigger("Down"))
	{
		m_selectItem++;

		m_selectItem = min(m_selectItem, static_cast<int>(ResultItem::kItemNum));
	}
	else if (input->IsTrigger("Up"))
	{
		m_selectItem--;

		m_selectItem = max(m_selectItem, 0);
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


}


void BattleUi::DrawMenu()
{
	auto& graphManager = GraphManager::GetInstance();

	//���j���[�\��
	DrawRotaGraph(kMenuDrawPosX, kMenuDrawPosY, 1.0, 0.0, graphManager.GetHandle("Menu"), true);;

	//

}

void BattleUi::DrawResult()
{
	m_resultTime++;

	DrawString(50, 300, "������x��������", GetColor(255, 255, 255));
	DrawString(50, 350, "�L�����N�^�[��I�тȂ���", GetColor(255, 255, 255));
	DrawString(50, 400, "���C�����j���[�ɖ߂�", GetColor(255, 255, 255));

	int posY = 0;

	if (m_selectItem == static_cast<int>(ResultItem::kRetry))
	{
		posY = 300;
	}
	else if (m_selectItem == static_cast<int>(ResultItem::kCharacterSelect))
	{
		posY = 350;
	}
	else if (m_selectItem == static_cast<int>(ResultItem::kMenu))
	{
		posY = 400;
	}

	DrawString(30, posY, "��", GetColor(255, 255, 255));

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
			DrawRotaGraph(logoPosX, logoPosY, m_resultLogoScale, 0.0, GraphManager::GetInstance().GetHandle("Winner"), true);
		}
		else
		{
			DrawRotaGraph(logoPosX, logoPosY, m_resultLogoScale, 0.0, GraphManager::GetInstance().GetHandle("Loser"), true);
		}
	}

}
