#include "SceneMenu.h"
#include "SceneSelect.h"
#include "SceneTitle.h"
#include "DxLib.h"
#include "Input.h"

namespace
{
	//Ui�̊�{�I�ȕ\�����W
	constexpr int kShowUiPosX = 200;
	constexpr int kShowUiPosY[static_cast<int>(SceneMenu::ItemKind::kItemKindNum)] = { 100,200,300,400,500 };

	//Ui��I���������ɓ������͈�
	constexpr int kShowUiMoveRange = 100;

	//Ui�𓮂�������
	constexpr int kShowUiMoveSpeed = 8;

	const std::string kshowUiString[static_cast<int>(SceneMenu::ItemKind::kItemKindNum)] =
	{
		"1P VS CPU",
		"1P VS 2P",
		"CPU VS CPU",
		"Option",
		"Exit"
	};

	const int kWhiteColor = GetColor(255, 255, 255);
}

SceneMenu::SceneMenu(SceneManager& sceneManager) :
	SceneBase(sceneManager),
	m_selectKind(0)
{
}

SceneMenu::~SceneMenu()
{
}

void SceneMenu::Init()
{
	for (int i = 0; i < static_cast<int>(ItemKind::kItemKindNum); i++)
	{
		m_showUi[i].showPosX = kShowUiPosX;
		m_showUi[i].showPosY = kShowUiPosY[i];
		m_showUi[i].showString = kshowUiString[i];
	}
}

void SceneMenu::Update()
{
	//�I�����Ă���I�������E�ɓ�����
	if (m_showUi[m_selectKind].showPosX < kShowUiPosX + kShowUiMoveRange)
	{
		//�E�ɓ�����
		m_showUi[m_selectKind].showPosX += kShowUiMoveSpeed;
		//�s����������߂�
		if (m_showUi[m_selectKind].showPosX > kShowUiPosX + kShowUiMoveRange)
		{
			m_showUi[m_selectKind].showPosX = kShowUiPosX + kShowUiMoveRange;
		}
	}
	//�I������ĂȂ��I���������ɓ�����
	for (int i = 0; i < static_cast<int>(ItemKind::kItemKindNum); i++)
	{
		//�I������Ă��邩�ǂ������m�F
		if (m_selectKind != i)
		{
			//���ɓ�����
			m_showUi[i].showPosX -= kShowUiMoveSpeed;
			//�����������Ȃ��悤�ɕ␳
			if (m_showUi[i].showPosX < kShowUiPosX)
			{
				m_showUi[i].showPosX = kShowUiPosX;
			}
		}
	}

	//�㉺���͂őI�����Ă��鍀�ڂ�ω�������
	auto& input = MyEngine::Input::GetInstance();

	if (input.IsTrigger("Up"))
	{
		//���ڂ��Ȃ������ɂ����Ȃ��悤��
		if (m_selectKind > 0)
		{
			m_selectKind--;
		}
	}
	else if (input.IsTrigger("Down"))
	{
		//���ڂ��Ȃ������ɍs���Ȃ��悤��
		if (m_selectKind < static_cast<int>(ItemKind::kSelectEnd))
		{
			m_selectKind++;
		}
	}

	if (MyEngine::Input::GetInstance().IsTrigger("A"))
	{
		if (m_selectKind == static_cast<int>(ItemKind::k1PvsCPU))
		{
			m_sceneManager.ChangeScene(std::make_shared<SceneSelect>(m_sceneManager));
		}
		else if (m_selectKind == static_cast<int>(ItemKind::k1Pvs2P))
		{
			m_sceneManager.ChangeScene(std::make_shared<SceneSelect>(m_sceneManager));
		}
		else if (m_selectKind == static_cast<int>(ItemKind::kCPUvsCPU))
		{
			m_sceneManager.ChangeScene(std::make_shared<SceneSelect>(m_sceneManager));
		}
		else if (m_selectKind == static_cast<int>(ItemKind::kOption))
		{
		}
		else
		{
			m_sceneManager.ChangeScene(std::make_shared<SceneTitle>(m_sceneManager));
		}
	}
}

void SceneMenu::Draw()
{
#ifdef _DEBUG 
	//�I������\������
	for (auto item : m_showUi)
	{
		//�`�揈��
		DrawString(item.showPosX, item.showPosY, item.showString.c_str(), kWhiteColor);

	}

	DrawString(0, 0, "SceneMenu", kWhiteColor);

#endif // _DEBUG

}

void SceneMenu::End()
{
}
