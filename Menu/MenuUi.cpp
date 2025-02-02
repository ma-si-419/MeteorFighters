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

	//�t�H���g�̖��O
	const TCHAR* kFontName = "GN-�L���SU����NB";

}


MenuUi::MenuUi() :
	m_selectItemFontHandle(-1),
	m_selectItem(SelectItem::kBattle),
	m_selectItemMoveTime(0)
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
	std::string name = "SelectItemBox";

	GraphUiStatus status;

	status.handle = GraphManager::GetInstance().GetHandle("SelectItemBox");
	status.showPosX = kItemBoxUiPosX;
	status.showPosY = kItemBoxUiPosY[0];

	EntryGraph(name, status);

	//�X�J�C�h�[���̐ݒ�
	MV1SetPosition(m_skyDomeHandle, VGet(0, 0, 0));

}

int MenuUi::Update()
{
	//�X�J�C�h�[������]������
	MV1SetRotationXYZ(m_skyDomeHandle, VGet(0, MV1GetRotationXYZ(m_skyDomeHandle).y + kCameraRotaSpeed, 0));

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

		DrawRotaGraph(data.showPosX, data.showPosY, 1.0, 0.0, data.handle, true);

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