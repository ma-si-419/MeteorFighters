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
		"�I�v�V����",
		"�Q�[�����I������",
	};

	//���ꂼ��̍��ڂŕ\������摜�̖��O
	const std::string kUiGraphName[static_cast<int>(MenuUi::SelectItem::kItemNum)] =
	{
		"Battle",
		"Tutorial",
		"Option",
		"GameEnd"
	};

	//�t�H���g�̖��O
	const TCHAR* kFontName = "GN-�L���SU����NB";

	//�I�����̃t�H���g�T�C�Y
	constexpr int kSelectItemFontSize = 64;

	//�I�v�V�����̃t�H���g�T�C�Y
	constexpr int kOptionFontSize = 40;

	//�I�v�V�����̉E���̕����̃t�H���g�T�C�Y
	constexpr int kOptionButtonStringFontSize = 28;

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

	//�I�v�V�����̃V�[�N�o�[�̈ʒu
	constexpr int kOptionBarPosX = Game::kWindowWidth / 2;
	constexpr int kOptionBarPosY = 230;

	//�V�[�N�o�[�̊Ԋu
	constexpr int kOptionBarInterval = 165;

	//�I�v�V�����̕������\��������W
	constexpr int kOptionStringPosX = 360;
	constexpr int kOptionStringPosY = 135;

	//�I�v�V�����̕�����
	const std::string kOptionString[static_cast<int>(SoundManager::OptionSoundKind::kOptionSoundKindNum)] =
	{
		"MASTER",
		"SE",
		"BGM",
		"VOICE"
	};

	//�I�v�V�����̃V�[�N�o�[�̒���
	constexpr int kOptionBarLength = 764;

	//�V�[�N�o�[�̒[�̉摜���ǂ̂��炢���炵�ĕ\�����邩
	constexpr int kOptionBarMarkShiftLange = 63;

	//�I�v�V�����̃V�[�N�o�[�𓮂������x
	constexpr float kOptionBarMoveSpeed = 0.01f;

	//�V�[�N�o�[�̍ő�g�嗦
	constexpr float kThumbMaxScale = 1.4f;

	//�V�[�N�o�[�̍ŏ��g�嗦
	constexpr float kThumbMinScale = 1.3f;

	//�V�[�N�o�[�̊g�嗦�̕ω����x
	constexpr float kThumbScaleSpeed = 0.2f;

	//�I�v�V�����̃{�^����\��������W
	constexpr int kOptionButtonPosX = 1146;
	constexpr int kOptionButtonPosY = 782;

	//�I�v�V�����̃{�^���̐�����\��������W
	constexpr int kOptionButtonStringPosX = 1150;
	constexpr int kOptionButtonStringPosY = 770;

	//�I�v�V�����̃t�F�[�h�C���̑���
	constexpr int kOptionFadeInSpeed = 20;
}


MenuUi::MenuUi() :
	m_selectItemFontHandle(-1),
	m_selectItem(SelectItem::kSelect),
	m_selectItemMoveTime(0),
	m_skyDomeHandle(-1),
	m_lastSelectItem(SelectItem::kSelect),
	m_rightGraphSrcNum(0),
	m_rightGraphChangeTime(0),
	m_optionSelectItem(0),
	m_isMoveSeekBar(false),
	m_thumbSize(0),
	m_optionFontHandle(-1),
	m_optionAlpha(0),
	m_isEndOption(false)
{
	m_selectItemFontHandle = CreateFontToHandle(kFontName, kSelectItemFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 3);
	m_optionFontHandle = CreateFontToHandle(kFontName, kOptionFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 3);
	m_optionButtonFontHandle = CreateFontToHandle(kFontName, kOptionButtonStringFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 3);


	m_skyDomeHandle = MV1LoadModel("data/model/Dome.mv1");

	m_updateFunc = &MenuUi::UpdateSelect;
	m_drawFunc = &MenuUi::DrawNone;
}

MenuUi::~MenuUi()
{
	DeleteFontToHandle(m_selectItemFontHandle);
	DeleteFontToHandle(m_optionFontHandle);
	DeleteFontToHandle(m_optionButtonFontHandle);
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
		m_selectStringUi[i].showPosX = kStringUiPosX;
		m_selectStringUi[i].showPosY = kStringUiPosY[i];
		m_selectStringUi[i].showString = kUiString[i];
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
	//�X�V����
	UpdateCommon();

	//�I�����̍X�V
	int selectItem = (this->*m_updateFunc)();

	return selectItem;
}

void MenuUi::DrawItem()
{
	//�摜�����ׂĕ\������
	for (auto item : m_selectGraphMap)
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
	for (auto item : m_selectStringUi)
	{
		//�`�揈��
		DrawStringToHandle(item.showPosX, item.showPosY, item.showString.c_str(), GetColor(255, 255, 255), m_selectItemFontHandle, GetColor(0, 0, 0));
	}

	//�I�v�V������Q�[���I���̉摜��\������
	(this->*m_drawFunc)();
}

void MenuUi::DrawModel()
{
	MV1DrawModel(m_skyDomeHandle);
}

void MenuUi::EntryGraph(std::string name, GraphUiStatus status)
{
	m_selectGraphMap[name] = status;
}

MenuUi::GraphUiStatus& MenuUi::GetGraphRef(std::string name)
{
	MenuUi::GraphUiStatus& ans = m_selectGraphMap[name];

	return ans;
}

void MenuUi::UpdateCommon()
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
	if (m_selectStringUi[static_cast<int>(m_selectItem)].showPosX < kStringUiPosX + kShowUiMoveRange)
	{
		//�E�ɓ�����
		m_selectStringUi[static_cast<int>(m_selectItem)].showPosX += kShowUiMoveSpeed;
		//�s����������߂�
		m_selectStringUi[static_cast<int>(m_selectItem)].showPosX = min(m_selectStringUi[static_cast<int>(m_selectItem)].showPosX, kStringUiPosX + kShowUiMoveRange);

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
			m_selectStringUi[i].showPosX -= kShowUiMoveSpeed;
			//�����������Ȃ��悤�ɕ␳
			m_selectStringUi[i].showPosX = max(m_selectStringUi[i].showPosX, kStringUiPosX);

			//���̃{�b�N�X���ꏏ�ɓ�����
			std::string name = "ItemBox";

			name += std::to_string(static_cast<int>(i));

			auto& itemBox = GetGraphRef(name);
			itemBox.showPosX -= kShowUiMoveSpeed;
			itemBox.showPosX = max(itemBox.showPosX, kItemBoxUiPosX);
		}
	}
}

int MenuUi::UpdateSelect()
{
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

	//�I�����Ă���I�����̗���
	auto& selectItemBox = GetGraphRef("SelectItemBox");

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

	int ans = -1;

	//����{�^������������
	if (input->IsTrigger("A"))
	{
		SoundManager::GetInstance().PlayOnceSound("Ok");

		//�I�v�V������I�����Ă�����
		if (m_selectItem == SelectItem::kOption)
		{
			//�I�v�V�����̍X�V�֐���ݒ肷��
			m_updateFunc = &MenuUi::UpdateOption;
			//�I�v�V�����̕`��֐���ݒ肷��
			m_drawFunc = &MenuUi::DrawOption;

			//�I�v�V�����̃A���t�@�l������������
			m_optionAlpha = 0;
			//�V�[�N�o�[�𓮂������ǂ���������������
			m_isMoveSeekBar = false;

			m_isEndOption = false;

			//�����̉��ʂ��擾����
			m_seVolume = SoundManager::GetInstance().GetVolume(SoundManager::OptionSoundKind::kSe);
			m_bgmVolume = SoundManager::GetInstance().GetVolume(SoundManager::OptionSoundKind::kBgm);
			m_voiceVolume = SoundManager::GetInstance().GetVolume(SoundManager::OptionSoundKind::kVoice);
			m_masterVolume = SoundManager::GetInstance().GetVolume(SoundManager::OptionSoundKind::kMaster);
		}
		//�Q�[���I����I�����Ă�����
		else if (m_selectItem == SelectItem::kEndGame)
		{
			//�Q�[���I���̍X�V�֐���ݒ肷��
			m_updateFunc = &MenuUi::UpdateEndGame;
			//�Q�[���I���̕`��֐���ݒ肷��
			m_drawFunc = &MenuUi::DrawEndGame;
		}
		else
		{
			//�I�����Ă��鍀�ڂ�Ԃ�
			ans = static_cast<int>(m_selectItem);
		}
	}

	return ans;

}

int MenuUi::UpdateOption()
{
	auto input = MyEngine::Input::GetInstance().GetInputData(0);
	auto& soundManager = SoundManager::GetInstance();

	//�I�v�V�����̍X�V����

	//�A���t�@�l�������Ă���
	if (!m_isEndOption)
	{
		m_optionAlpha += kOptionFadeInSpeed;
		if (m_optionAlpha >= 255)
		{
			m_optionAlpha = 255;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		m_optionAlpha -= kOptionFadeInSpeed;

		if (m_optionAlpha < 0)
		{
			//�Z���N�g���[�h�ɖ߂�
			m_updateFunc = &MenuUi::UpdateSelect;
			m_drawFunc = &MenuUi::DrawNone;
		}
		return -1;
	}

	//�V�[�N�o�[�𓮂������ǂ�����false�̎�
	if (!m_isMoveSeekBar)
	{
		//����͂�����Ă�����
		if (MyEngine::Input::GetInstance().GetInputData(0)->IsTrigger("Up"))
		{
			m_optionSelectItem--;

			//�ŏ��l�����������Ȃ�����
			if (m_optionSelectItem < 0)
			{
				//�ő�l�ɖ߂�
				m_optionSelectItem = static_cast<int>(SoundManager::OptionSoundKind::kOptionSoundKindNum) - 1;
			}
		}
		//�����͂�����Ă�����
		else if (MyEngine::Input::GetInstance().GetInputData(0)->IsTrigger("Down"))
		{
			m_optionSelectItem++;
			//�ő�l�𒴂�����
			if (m_optionSelectItem >= static_cast<int>(SoundManager::OptionSoundKind::kOptionSoundKindNum))
			{
				//�ŏ��l�ɖ߂�
				m_optionSelectItem = 0;
			}
		}

		//B�{�^���������ꂽ��
		if (input->IsTrigger("B"))
		{
			m_isEndOption = true;

			//�L�����Z���T�E���h��炷
			SoundManager::GetInstance().PlayOnceSound("Cancel");
		}
	}
	//�V�[�N�o�[�𓮂�����
	else
	{
		//�����͂�����Ă�����
		if (input->IsPress("Left"))
		{
			//�I�����Ă���V�[�N�o�[�̉��ʂ�������
			if (m_optionSelectItem == static_cast<int>(SoundManager::OptionSoundKind::kSe))
			{
				m_seVolume -= kOptionBarMoveSpeed;
			}
			else if (m_optionSelectItem == static_cast<int>(SoundManager::OptionSoundKind::kBgm))
			{
				m_bgmVolume -= kOptionBarMoveSpeed;
			}
			else if (m_optionSelectItem == static_cast<int>(SoundManager::OptionSoundKind::kVoice))
			{
				m_voiceVolume -= kOptionBarMoveSpeed;
			}
			else if (m_optionSelectItem == static_cast<int>(SoundManager::OptionSoundKind::kMaster))
			{
				m_masterVolume -= kOptionBarMoveSpeed;
			}
		}
		//�E���͂�����Ă�����
		else if (input->IsPress("Right"))
		{
			//�I�����Ă���V�[�N�o�[�̉��ʂ��グ��
			if (m_optionSelectItem == static_cast<int>(SoundManager::OptionSoundKind::kSe))
			{
				m_seVolume += kOptionBarMoveSpeed;
			}
			else if (m_optionSelectItem == static_cast<int>(SoundManager::OptionSoundKind::kBgm))
			{
				m_bgmVolume += kOptionBarMoveSpeed;
			}
			else if (m_optionSelectItem == static_cast<int>(SoundManager::OptionSoundKind::kVoice))
			{
				m_voiceVolume += kOptionBarMoveSpeed;
			}
			else if (m_optionSelectItem == static_cast<int>(SoundManager::OptionSoundKind::kMaster))
			{
				m_masterVolume += kOptionBarMoveSpeed;
			}
		}

		//���ʂ̃N�����v
		m_seVolume = fmax(m_seVolume, 0.0f);
		m_seVolume = fmin(m_seVolume, 1.0f);

		m_bgmVolume = fmax(m_bgmVolume, 0.0f);
		m_bgmVolume = fmin(m_bgmVolume, 1.0f);

		m_voiceVolume = fmax(m_voiceVolume, 0.0f);
		m_voiceVolume = fmin(m_voiceVolume, 1.0f);

		m_masterVolume = fmax(m_masterVolume, 0.0f);
		m_masterVolume = fmin(m_masterVolume, 1.0f);


		//SE���ʂ��ύX����Ă��邩�`�F�b�N
		if (soundManager.GetVolume(SoundManager::OptionSoundKind::kSe) != m_seVolume)
		{
			//���ʂ�ύX����
			soundManager.SetVolume(SoundManager::OptionSoundKind::kSe, m_seVolume);
		}

		//BGM���ʂ��ύX����Ă��邩�`�F�b�N
		if (soundManager.GetVolume(SoundManager::OptionSoundKind::kBgm) != m_bgmVolume)
		{
			//���ʂ�ύX����
			soundManager.SetVolume(SoundManager::OptionSoundKind::kBgm, m_bgmVolume);
		}

		//VOICE���ʂ��ύX����Ă��邩�`�F�b�N
		if (soundManager.GetVolume(SoundManager::OptionSoundKind::kVoice) != m_voiceVolume)
		{
			//���ʂ�ύX����
			soundManager.SetVolume(SoundManager::OptionSoundKind::kVoice, m_voiceVolume);
		}

		//MASTER���ʂ��ύX����Ă��邩�`�F�b�N
		if (soundManager.GetVolume(SoundManager::OptionSoundKind::kMaster) != m_masterVolume)
		{
			//���ʂ�ύX����
			soundManager.SetVolume(SoundManager::OptionSoundKind::kMaster, m_masterVolume);
		}

		//B�{�^���������ꂽ��
		if (MyEngine::Input::GetInstance().GetInputData(0)->IsTrigger("B"))
		{
			//�V�[�N�o�[�𓮂������ǂ�����false�ɂ���
			m_isMoveSeekBar = false;

			//�L�����Z���T�E���h��炷
			SoundManager::GetInstance().PlayOnceSound("Cancel");
		}
	}

	//A�{�^���������ꂽ��
	if (MyEngine::Input::GetInstance().GetInputData(0)->IsTrigger("A"))
	{
		m_isMoveSeekBar = true;

		//����T�E���h��炷
		SoundManager::GetInstance().PlayOnceSound("Ok");
	}



	//��{�I��-1��Ԃ�
	return -1;
}

int MenuUi::UpdateEndGame()
{
	//�Q�[�����I������
	return static_cast<int>(SelectItem::kEndGame);
}

void MenuUi::DrawOption()
{
	//�I�v�V�����̕`�揈��
	int optionMenu = GraphManager::GetInstance().GetHandle("OptionMenu");

	int bar = GraphManager::GetInstance().GetHandle("Bar");

	int thumb = GraphManager::GetInstance().GetHandle("Thumb");

	int thumbSelect = GraphManager::GetInstance().GetHandle("ThumbSelect");

	int minSound = GraphManager::GetInstance().GetHandle("MinSound");

	int maxSound = GraphManager::GetInstance().GetHandle("MaxSound");

	//�܂݂̃T�C�Y��ݒ肷��
	m_thumbSize += kThumbScaleSpeed;
	float thumbScale = kThumbMinScale + (kThumbMaxScale - kThumbMinScale) * sin(m_thumbSize);

	//�u�����h���[�h�̐ݒ�
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_optionAlpha);

	//�I�v�V�����̉摜��`�悷��
	DrawRotaGraph(Game::kWindowWidth / 2, Game::kWindowHeight / 2, 1.0, 0.0, optionMenu, true);

	//�V�[�N�o�[�𒲐��ł��鐔���\������
	for (int i = 0; i < static_cast<int>(SoundManager::OptionSoundKind::kOptionSoundKindNum); i++)
	{
		//�V�[�N�o�[�̈ʒu��ݒ肷��
		int posX = kOptionBarPosX;
		int posY = kOptionBarPosY + kOptionBarInterval * i;

		//�V�[�N�o�[��`�悷��
		DrawRotaGraph(posX, posY, 1.0, 0.0, bar, true);

		//�V�[�N�o�[�̒[�̉摜��`�悷��
		DrawRotaGraph(posX - kOptionBarLength / 2 - kOptionBarMarkShiftLange, posY, 1.0, 0.0, minSound, true);

		//�V�[�N�o�[�̒[�̉摜��`�悷��
		DrawRotaGraph(posX + kOptionBarLength / 2 + kOptionBarMarkShiftLange, posY, 1.0, 0.0, maxSound, true);

		//�܂݂̈ʒu��ݒ肷��
		posX -= kOptionBarLength / 2;

		if (i == static_cast<int>(SoundManager::OptionSoundKind::kSe))
		{
			posX += kOptionBarLength * m_seVolume;
		}
		else if (i == static_cast<int>(SoundManager::OptionSoundKind::kBgm))
		{
			posX += kOptionBarLength * m_bgmVolume;
		}
		else if (i == static_cast<int>(SoundManager::OptionSoundKind::kVoice))
		{
			posX += kOptionBarLength * m_voiceVolume;
		}
		else if (i == static_cast<int>(SoundManager::OptionSoundKind::kMaster))
		{
			posX += kOptionBarLength * m_masterVolume;
		}

		if (i == m_optionSelectItem)
		{
			if (m_isMoveSeekBar)
			{
				//�܂݂̈ʒu��`�悷��
				DrawRotaGraph(posX, posY, thumbScale, 0.0, thumb, true);
			}
			else
			{

				//�܂݂̈ʒu��`�悷��
				DrawRotaGraph(posX, posY, 1.0, 0.0, thumb, true);

				DrawRotaGraph(posX, posY, thumbScale, 0.0, thumbSelect, true);
			}
		}
		else
		{
			//�܂݂̈ʒu��`�悷��
			DrawRotaGraph(posX, posY, 1.0, 0.0, thumb, true);
		}
	}

	//�I�v�V�����̕������\������
	for (int i = 0; i < static_cast<int>(SoundManager::OptionSoundKind::kOptionSoundKindNum); i++)
	{
		DrawStringToHandle(kOptionStringPosX, kOptionStringPosY + kOptionBarInterval * i, kOptionString[i].c_str(), GetColor(255, 255, 255), m_optionFontHandle, GetColor(0, 0, 0));
	}


	if (m_isMoveSeekBar)
	{
		//�I�v�V�����̃{�^����\������
		int button = GraphManager::GetInstance().GetHandle("B");
		DrawRotaGraph(kOptionButtonPosX, kOptionButtonPosY, 1.0, 0.0, button, true);

		DrawStringToHandle(kOptionButtonStringPosX, kOptionButtonStringPosY, " :�߂�", GetColor(255, 255, 255), m_optionButtonFontHandle, GetColor(0, 0, 0));

	}
	else
	{
		//�I�v�V�����̃{�^����\������
		int button = GraphManager::GetInstance().GetHandle("A");
		DrawRotaGraph(kOptionButtonPosX, kOptionButtonPosY, 1.0, 0.0, button, true);

		DrawStringToHandle(kOptionButtonStringPosX, kOptionButtonStringPosY, " :����", GetColor(255, 255, 255), m_optionButtonFontHandle, GetColor(0, 0, 0));
	}

	//�u�����h���[�h��߂�
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}

void MenuUi::DrawEndGame()
{

}
