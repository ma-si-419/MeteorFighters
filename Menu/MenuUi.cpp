#include "MenuUi.h"
#include "SoundManager.h"
#include "GraphManager.h"
#include "Input.h"
#include "Game.h"

namespace
{
	//������̊�{�I�ȕ\�����W
	constexpr int kStringUiPosX = 30;
	constexpr int kStringUiPosY[static_cast<int>(MenuUi::SelectItem::kItemNum)] =
	{
		170,
		350,
		530,
		710
	};

	constexpr int kItemBoxUiPosX = 240;
	constexpr int kItemBoxUiPosY[static_cast<int>(MenuUi::SelectItem::kItemNum)] =
	{
		200,
		380,
		560,
		740
	};

	//�I�����Ă���{�b�N�X�̃A���t�@�l��ύX���鑬�x
	constexpr int kSelectItemBoxBlendSpeed = 20;

	//Ui��I���������ɓ������͈�
	constexpr int kShowUiMoveRange = 130;

	//Ui�𓮂�������
	constexpr int kShowUiMoveSpeed = 18;

	//�㉺���͂Ń��s�[�g���͂��󂯕t���鎞��
	constexpr int kRepeatInputTime = 15;

	//���s�[�g���͂̊Ԋu
	constexpr int kRepeatInterval = 5;

	//�J������NearFar
	constexpr float kCameraNear = 0.1f;
	constexpr float kCameraFar = 100.0f;

	//�J�������񂷑��x
	constexpr float kCameraRotaSpeed = 0.001f;

	//�\�����镶����
	const std::string kUiString[static_cast<int>(MenuUi::SelectItem::kItemNum)] =
	{
		"CPU�ƃo�g��",
		"�`���[�g���A��",
		"�^�C�g���ɖ߂�",
		"�Q�[�����I������",
	};

	//���ꂼ��̍��ڂŕ\������摜�̖��O
	const std::string kUiGraphName[static_cast<int>(MenuUi::SelectItem::kItemNum)] =
	{
		"Battle",
		"Tutorial",
		"SelectItemBox",
		"GameEnd"
	};

	//�t�H���g�̖��O
	const TCHAR* kFontName = "GN-�L���SU����NB";

	//�E���ɕ\������摜�̑傫��
	constexpr int kRightGraphSize = 680;

	//�E���ɕ\������摜�̈ʒu
	constexpr int kRightGraphPosX = 1150;
	constexpr int kRightGraphPosY = Game::kWindowHeight / 2;

	//�E���ɕ\������摜���͂����Ă���Ƃ��ǂ̂��炢���炵�ĕ\�����邩
	constexpr int kRightGraphMoveRange = 150;

	//�E���ɕ\������摜�̈ړ����x
	constexpr int kRightGraphMoveSpeed = 10;

	//�E���ɕ\������摜�̃t�F�[�h�C���A�E�g�̑��x
	constexpr int kRightGraphBlendSpeed = 20;

	//�E���̉摜��؂�ւ��鎞��
	constexpr int kRightGraphChangeTime = 15;

	//�E���̉摜�̐؂�ւ��鐔
	constexpr int kRightGraphChangeNum = 1;
}


MenuUi::MenuUi() :
	m_selectItemFontHandle(-1),
	m_selectItem(SelectItem::kBattle),
	m_selectItemMoveTime(0),
	m_skyDomeHandle(-1),
	m_lastSelectItem(SelectItem::kBattle),
	m_rightGraphSrcNum(0),
	m_rightGraphChangeTime(0)
{
	m_selectItemFontHandle = CreateFontToHandle(kFontName, 64, 0, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 3);
	m_skyDomeHandle = MV1LoadModel("data/model/Dome.mv1");
}

MenuUi::~MenuUi()
{
	DeleteFontToHandle(m_selectItemFontHandle);
}

void MenuUi::Init()
{
	//�J�����̐ݒ�
	SetCameraPositionAndTarget_UpVecY(VGet(0, 0, 0), VGet(0, 0, 1));

	//NearFar�̐ݒ�
	SetCameraNearFar(kCameraNear, kCameraFar);


	//�������ݒ肷��
	for (int i = 0; i < static_cast<int>(SelectItem::kItemNum); i++)
	{
		m_stringUi[i].showPosX = kStringUiPosX;
		m_stringUi[i].showPosY = kStringUiPosY[i];
		m_stringUi[i].showString = kUiString[i];
	}

	//�I���ł��鍀�ڂ̃{�b�N�X�̉摜��ݒ肷��
	for (int i = 0; i < static_cast<int>(SelectItem::kItemNum); i++)
	{
		std::string name = "ItemBox";
		name += std::to_string(i);

		GraphUiStatus status;

		status.handle = GraphManager::GetInstance().GetHandle("ItemBox");
		status.showPosX = kItemBoxUiPosX;
		status.showPosY = kItemBoxUiPosY[i];

		EntryGraph(name, status);
	}

	//�I�����Ă��鍀�ڂ̃{�b�N�X�̉摜��ݒ肷��
	{
		std::string selectItemBox = "SelectItemBox";

		GraphUiStatus status;

		status.handle = GraphManager::GetInstance().GetHandle("SelectItemBox");
		status.showPosX = kItemBoxUiPosX;
		status.showPosY = kItemBoxUiPosY[0];

		EntryGraph(selectItemBox, status);
	}


	//�ЂƂO�ɑI�����Ă������ڂ̉E���ɕ\�����Ă����摜��ݒ肷��
	{
		std::string name = "LastRightGraph";
		GraphUiStatus status;
		status.handle = GraphManager::GetInstance().GetHandle(kUiGraphName[static_cast<int>(m_lastSelectItem)]);
		status.showPosX = kRightGraphPosX;
		status.showPosY = kRightGraphPosY;
		status.graphSizeX = kRightGraphSize;
		status.graphSizeY = kRightGraphSize;
		status.srcPosX = 0;
		status.alpha = 0;
		EntryGraph(name, status);
	}

	//�E���ɕ\������摜��ݒ肷��
	{
		std::string name = "RightGraph";
		GraphUiStatus status;
		status.handle = GraphManager::GetInstance().GetHandle(kUiGraphName[static_cast<int>(m_selectItem)]);
		status.showPosX = kRightGraphPosX;
		status.showPosY = kRightGraphPosY;
		status.graphSizeX = kRightGraphSize;
		status.graphSizeY = kRightGraphSize;
		status.srcPosX = 0;
		EntryGraph(name, status);
	}

	//�X�J�C�h�[���̐ݒ�
	MV1SetPosition(m_skyDomeHandle, VGet(0, 0, 0));

}

int MenuUi::Update()
{
	//�X�J�C�h�[������]������
	MV1SetRotationXYZ(m_skyDomeHandle, VGet(0, MV1GetRotationXYZ(m_skyDomeHandle).y + kCameraRotaSpeed, 0));

	//�E���̉摜��؂�ւ��鎞��
	m_rightGraphChangeTime++;

	//�E���ɕ\�����Ă���摜�̃A���t�@�l�̐ݒ�
	auto& lastRightGraph = GetGraphRef("LastRightGraph");
	auto& rightGraph = GetGraphRef("RightGraph");

	//�I�����Ă��鍀�ڂ̉E���ɕ\�����Ă����摜
	if (lastRightGraph.alpha > 0)
	{
		lastRightGraph.alpha -= kRightGraphBlendSpeed;
		lastRightGraph.alpha = max(lastRightGraph.alpha, 0);
	}

	//�I�����Ă��鍀�ڂ̉E���ɕ\������摜
	if (rightGraph.alpha < 255)
	{
		rightGraph.alpha += kRightGraphBlendSpeed;
		rightGraph.alpha = min(rightGraph.alpha, 255);
	}

	//�O�I�����Ă������ڂ��I�����Ă��鍀�ڂ����������ꍇ
	if (m_lastSelectItem < m_selectItem)
	{
		//�E���ɕ\�����Ă����摜����ɓ�����
		lastRightGraph.showPosY -= kRightGraphMoveSpeed;
				 
		//�E���ɕ\������摜����ɓ�����
		rightGraph.showPosY -= kRightGraphMoveSpeed;
		//�����������Ȃ��悤�ɕ␳
		rightGraph.showPosY = max(rightGraph.showPosY, kRightGraphPosY);
	}
	//�O�I�����Ă������ڂ��I�����Ă��鍀�ڂ����傫���ꍇ
	else if (m_lastSelectItem > m_selectItem)
	{
		//�E���ɕ\�����Ă����摜�����ɓ�����
		lastRightGraph.showPosY += kRightGraphMoveSpeed;
		//�E���ɕ\������摜�����ɓ�����
		rightGraph.showPosY += kRightGraphMoveSpeed;
		//�����������Ȃ��悤�ɕ␳
		rightGraph.showPosY = min(rightGraph.showPosY, kRightGraphPosY);
	}

	//�E���̉摜��؂�ւ��鎞�ԂɂȂ�����
	if (m_rightGraphChangeTime > kRightGraphChangeTime)
	{
		m_rightGraphChangeTime = 0;

		m_rightGraphSrcNum++;

		//���l�𒴂�����
		if (m_rightGraphSrcNum > kRightGraphChangeNum)
		{
			m_rightGraphSrcNum = 0;
		}

		//�E���ɕ\�����Ă����摜�̐؂�ւ�
		lastRightGraph.srcPosX = lastRightGraph.graphSizeX * m_rightGraphSrcNum;
		//�E���ɕ\������摜�̐؂�ւ�
		rightGraph.srcPosX = rightGraph.graphSizeX * m_rightGraphSrcNum;
	}

	//�I�����Ă���I�����̗���
	auto& selectItemBox = GetGraphRef("SelectItemBox");

	//�I�����Ă���I�������E�ɓ�����
	if (m_stringUi[static_cast<int>(m_selectItem)].showPosX < kStringUiPosX + kShowUiMoveRange)
	{
		//�E�ɓ�����
		m_stringUi[static_cast<int>(m_selectItem)].showPosX += kShowUiMoveSpeed;
		//�s����������߂�
		m_stringUi[static_cast<int>(m_selectItem)].showPosX = min(m_stringUi[static_cast<int>(m_selectItem)].showPosX, kStringUiPosX + kShowUiMoveRange);

		//�����̃{�b�N�X���ꏏ�ɓ�����

		//���{�b�N�X
		std::string name = "ItemBox";

		name += std::to_string(static_cast<int>(m_selectItem));

		auto& itemBox = GetGraphRef(name);
		itemBox.showPosX += kShowUiMoveSpeed;
		itemBox.showPosX = min(itemBox.showPosX, kItemBoxUiPosX + kShowUiMoveRange);

		//���F���{�b�N�X
		selectItemBox.showPosX += kShowUiMoveSpeed;
		selectItemBox.showPosX = min(selectItemBox.showPosX, kItemBoxUiPosX + kShowUiMoveRange);
		selectItemBox.showPosY = kItemBoxUiPosY[static_cast<int>(m_selectItem)];
	}

	//�I�����Ă��鍀�ڂ̃A���t�@�l��ύX����
	selectItemBox.alpha += kSelectItemBoxBlendSpeed;

	selectItemBox.alpha = min(selectItemBox.alpha, 255);

	//�I������ĂȂ��I���������ɓ�����
	for (int i = 0; i < static_cast<int>(SelectItem::kItemNum); i++)
	{
		//�I������Ă��邩�ǂ������m�F
		if (static_cast<int>(m_selectItem) != i)
		{
			//���ɓ�����
			m_stringUi[i].showPosX -= kShowUiMoveSpeed;
			//�����������Ȃ��悤�ɕ␳
			m_stringUi[i].showPosX = max(m_stringUi[i].showPosX, kStringUiPosX);

			//���̃{�b�N�X���ꏏ�ɓ�����
			std::string name = "ItemBox";

			name += std::to_string(static_cast<int>(i));

			auto& itemBox = GetGraphRef(name);
			itemBox.showPosX -= kShowUiMoveSpeed;
			itemBox.showPosX = max(itemBox.showPosX, kItemBoxUiPosX);
		}
	}

	//�㉺���͂őI�����Ă��鍀�ڂ�ω�������
	auto input = MyEngine::Input::GetInstance().GetInputData(0);

	//�O�̃t���[���̏��
	auto lastItem = m_selectItem;

	//�������₷���悤��int�^�ɃL���X�g
	auto selectItem = static_cast<int>(m_selectItem);

	//�g���K�[����
	if (input->IsTrigger("Up"))
	{

		//��ɃJ�[�\���𓮂���
		selectItem--;

		//��ԏ�ɃJ�[�\�����������ꍇ
		if (selectItem < 0)
		{
			//��ԉ��Ɉړ�����
			selectItem = static_cast<int>(SelectItem::kItemEnd);
		}
	}
	else if (input->IsTrigger("Down"))
	{
		//���ɃJ�[�\���𓮂���
		selectItem++;

		if (selectItem > static_cast<int>(SelectItem::kItemEnd))
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

	//�N�����v
	selectItem = max(selectItem, 0);
	selectItem = min(selectItem, static_cast<int>(SelectItem::kItemNum) - 1);

	m_selectItem = static_cast<SelectItem>(selectItem);

	//�J�[�\���������Ă�����
	if (m_selectItem != lastItem)
	{
		//���������^�C�~���O�őI�����Ă��鍀�ڂ̌��̃{�b�N�X��X���W�������ʒu�ɖ߂�
		selectItemBox.showPosX = kItemBoxUiPosX;
		//�A���t�@�l��0�ɂ���
		selectItemBox.alpha = 0;

		//�����ĉ��t���[�������������Z�b�g
		m_selectItemMoveTime = 0;

		//�ЂƂO�I�����Ă������ڂ��L������
		m_lastSelectItem = lastItem;

		//�I�����Ă������ڂ̉E���ɕ\�����Ă����摜��ݒ肷��
		{
			std::string name = "LastRightGraph";
			auto& lastRightGraph = GetGraphRef(name);
			lastRightGraph.handle = GraphManager::GetInstance().GetHandle(kUiGraphName[static_cast<int>(m_lastSelectItem)]);
			lastRightGraph.alpha = 255;
			lastRightGraph.showPosX = kRightGraphPosX;
			lastRightGraph.showPosY = kRightGraphPosY;
		}

		//�E���ɕ\������摜��ݒ肷��
		{
			std::string name = "RightGraph";
			auto& rightGraph = GetGraphRef(name);
			rightGraph.handle = GraphManager::GetInstance().GetHandle(kUiGraphName[static_cast<int>(m_selectItem)]);
			rightGraph.alpha = 0;
			rightGraph.showPosX = kRightGraphPosX;

			//���ɓ������Ă����ꍇ
			if (static_cast<int>(m_selectItem) > static_cast<int>(m_lastSelectItem))
			{
				rightGraph.showPosY = kRightGraphPosY + kRightGraphMoveRange;
			}
			//��ɓ������Ă����ꍇ
			else
			{
				rightGraph.showPosY = kRightGraphPosY - kRightGraphMoveRange;
			}
		}

		//�I������炷
		SoundManager::GetInstance().PlayOnceSound("Select");
	}
	else
	{
		m_selectItemMoveTime++;
	}

	if (input->IsTrigger("A"))
	{
		SoundManager::GetInstance().PlayOnceSound("Ok");

		return static_cast<int>(m_selectItem);
	}

	return -1;
}

void MenuUi::DrawItem()
{
	//�摜�����ׂĕ\������
	for (auto item : m_graphMap)
	{
		auto data = item.second;

		//�A���t�@�l���ݒ肳��Ă�����u�����h���[�h��ύX����
		if (data.alpha < 255) SetDrawBlendMode(DX_BLENDMODE_ALPHA, data.alpha);

		//�摜�̃T�C�Y���w�肳��Ă�����
		if (data.graphSizeX != 0 && data.graphSizeY != 0)
		{
			//�؂����ĕ`��
			DrawRectRotaGraph(data.showPosX, data.showPosY, data.srcPosX, 0, data.graphSizeX, data.graphSizeY, 1.0, 0.0, data.handle, true);
		}
		else
		{
			//���̂܂ܕ`��
			DrawRotaGraph(data.showPosX, data.showPosY, 1.0, 0.0, data.handle, true);
		}
		if (data.alpha < 255) SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	//�I������\������
	for (auto item : m_stringUi)
	{
		//�`�揈��
		DrawStringToHandle(item.showPosX, item.showPosY, item.showString.c_str(), GetColor(255, 255, 255), m_selectItemFontHandle, GetColor(0, 0, 0));
	}

}

void MenuUi::DrawModel()
{
	MV1DrawModel(m_skyDomeHandle);
}

void MenuUi::EntryGraph(std::string name, GraphUiStatus status)
{
	m_graphMap[name] = status;
}

MenuUi::GraphUiStatus& MenuUi::GetGraphRef(std::string name)
{
	MenuUi::GraphUiStatus& ans = m_graphMap[name];

	return ans;
}