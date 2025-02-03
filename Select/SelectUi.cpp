#include "SelectUi.h"
#include "SelectManager.h"
#include "GraphManager.h"
#include "DxLib.h"
#include "Game.h"
#include "EnemyInput.h"

namespace
{
	//�t�H���g�̖��O
	const TCHAR* kFontName = "GN-�L���SU����NB";

	//�t�H���g�̃T�C�Y
	constexpr int kLevelFontSize = 48;
	//�Ȗ���\������t�H���g�̃T�C�Y
	constexpr int kMusicNameFontSize = 36;
	//�Ȗ���\�����镶����̐F(���F)
	const int kMusicNameColor = GetColor(255, 255, 32);	 
	//�Ȗ��̃t�F�[�h�C���̑��x
	constexpr int kMusicNameFadeInSpeed = 13;
	//�Ȗ��̈ړ����x
	constexpr int kMusicNameMoveSpeed = 10;

	//������\��������W
	constexpr int kMusicNotePosX = 1105;
	constexpr int kMusicNotePosY = 850;
	
	//�Ȗ���\��������W
	constexpr int kMusicNamePosX = 1152;
	constexpr int kMusicNamePosY = 835;

	//�Ȗ��̏������W
	constexpr int kMusicNameInitPosX = 1300;

	//�J�����̐ݒ�
	constexpr float kCameraNear = 0.1f;
	constexpr float kCameraFar = 100.0f;

	//�X�J�C�h�[���̉�]���x
	constexpr float kSkyDomeRotaSpeed = 0.002f;

	//�L�����N�^�[�̉摜�ݒ�
	constexpr int kPlayerCharacterPosX = 320;
	constexpr int kEnemyCharacterPosX = 1280;
	constexpr int kCharacterPosY = 450;
	constexpr double kCharacterScale = 1.0;

	//�L�����N�^�[�̉摜�����ɂ���鑬�x
	constexpr int kLastCharacterGraphMoveSpeed = 10;

	//�ǂ̂��炢������L�����N�^�[�������Ă��邩
	constexpr int kCharacterPopDistance = 120;

	//�L�����N�^�[�̉摜�������Ă������x
	constexpr int kLastCharacterGraphFadeSpeed = 20;

	//�A�C�R���̉摜�ݒ�
	constexpr int kIconPosX = 630;
	constexpr int kIconDistance = 170;
	constexpr int kIconPosY = 500;

	//�I�����Ă���A�C�R���������摜�ݒ�
	constexpr double kIconFrameAddScaleMax = 0.1;
	constexpr float kIconFrameScallingSpeed = 0.13f;

	//HeadSet�̉摜�ݒ�
	constexpr int kHeadSetPosX = 800;
	constexpr int kHeadSetPosY = 200;

	//VS�̉摜�ݒ�
	constexpr int kVsPosX = 800;
	constexpr int kVsPosY = 750;

	//RB�̉摜�ݒ�
	constexpr int kRbPosX = Game::kWindowWidth - 130;

	//LB�̉摜�ݒ�
	constexpr int kLbPosX = 130;

	//��Փx�I���̃{�^���̍��W
	constexpr int kLevelSelectButtonPosY = 75;

	//��Փx�I���̖���h�炷�傫��
	constexpr int kLevelSelectArrowShakeScale = 18;

	//��Փx�I���̖���h�炷����
	constexpr float kLevelSelectArrowShakeSpeed = 0.25f;

	//��Փx��\��������W
	constexpr int kRightLevelPosX = kRbPosX - 46;
	constexpr int kLeftLevelPosX = kLbPosX + 55;
	constexpr int kLevelPosY = kLevelSelectButtonPosY - 20;

	//��Փx�̕�����
	const std::string kLevelString[static_cast<int>(EnemyInput::AiLevel::kLevelNum)] =
	{
		"EASY",
		"NORMAL",
		"HARD"
	};

	//��Փx���Ƃ̕�����̐F
	const unsigned int kLevelColor[static_cast<int>(EnemyInput::AiLevel::kLevelNum)] =
	{
		GetColor(255, 255, 255),
		GetColor(32,32, 32),
		GetColor(255, 0, 0)
	};


	const std::string kCharacterNames[static_cast<int>(SelectManager::CharacterNumber::kCharacterNum)] =
	{
		"Character0",
		"Random",
		"Character1"
	};
}

SelectUi::SelectUi() :
	m_lastPlayerNumber(0),
	m_lastEnemyNumber(static_cast<int>(SelectManager::CharacterNumber::kBlueHead)),
	m_playerNumber(0),
	m_enemyNumber(static_cast<int>(SelectManager::CharacterNumber::kBlueHead)),
	m_skyDomeHandle(-1),
	m_iconFrameScalling(0.0f),
	m_time(0.0f),
	m_level(0)
{
	//���x���t�H���g�̓ǂݍ���
	m_levelFontHandle = CreateFontToHandle(kFontName, kLevelFontSize, 3, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 2);

	//�Ȗ���\������t�H���g�̓ǂݍ���
	m_musicNameFontHandle = CreateFontToHandle(kFontName, kMusicNameFontSize, 3, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 2);
}

SelectUi::~SelectUi()
{
	DeleteFontToHandle(m_levelFontHandle);
	DeleteFontToHandle(m_musicNameFontHandle);
}

void SelectUi::Init()
{
	//�X�J�C�h�[���̐ݒ�
	m_skyDomeHandle = MV1LoadModel("data/model/Dome.mv1");

	//�J�����̐ݒ�
	SetCameraPositionAndTarget_UpVecY(VGet(0, 0, 0), VGet(0, 0, 1));

	//NearFar�̐ݒ�
	SetCameraNearFar(kCameraNear, kCameraFar);

	//�X�J�C�h�[���̐ݒ�
	MV1SetPosition(m_skyDomeHandle, VGet(0, 0, 0));

	//�X�V�֐��̐ݒ�
	m_updateFunc = &SelectUi::Update1P;

	//�O���t�}�l�[�W���[
	auto& graphManager = GraphManager::GetInstance();

	//�v���C���[�̉摜�ݒ�
	GraphData playerDefault;
	playerDefault.posX = kPlayerCharacterPosX;
	playerDefault.posY = kCharacterPosY;
	playerDefault.scale = kCharacterScale;
	playerDefault.alpha = 0;
	playerDefault.handle = graphManager.GetHandle(kCharacterNames[m_playerNumber]);
	m_drawGraphs[GraphName::kLastPlayer] = playerDefault;
	playerDefault.alpha = 255;
	m_drawGraphs[GraphName::kPlayer] = playerDefault;

	//�G�l�~�[�̉摜�ݒ�
	GraphData enemyDefault;
	enemyDefault.posX = kEnemyCharacterPosX;
	enemyDefault.posY = kCharacterPosY;
	enemyDefault.scale = kCharacterScale;
	enemyDefault.alpha = 0;
	enemyDefault.handle = graphManager.GetHandle(kCharacterNames[m_enemyNumber]);
	m_drawGraphs[GraphName::kLastEnemy] = enemyDefault;
	enemyDefault.alpha = 255;
	m_drawGraphs[GraphName::kEnemy] = enemyDefault;

	//�A�C�R���̉摜�ݒ�

	//��ڂ̃A�C�R��
	GraphData icon;
	icon.posX = kIconPosX;
	icon.posY = kIconPosY;
	icon.handle = graphManager.GetHandle("Icon0");
	m_drawGraphs[GraphName::kIcon0] = icon;

	//��ڂ̃A�C�R��
	icon.posX += kIconDistance;
	icon.handle = graphManager.GetHandle("IconRandom");
	m_drawGraphs[GraphName::kIcon1] = icon;

	//�O�ڂ̃A�C�R��
	icon.posX += kIconDistance;
	icon.handle = graphManager.GetHandle("Icon1");
	m_drawGraphs[GraphName::kIcon2] = icon;

	//�I�����Ă���1P�̃A�C�R���̃t���[��
	GraphData iconFrame1P;
	iconFrame1P.posX = kIconPosX;
	iconFrame1P.posY = kIconPosY;
	iconFrame1P.handle = graphManager.GetHandle("IconFrame1P");
	m_drawGraphs[GraphName::kIconFrame1P] = iconFrame1P;

	//�I�����Ă���2P�̃A�C�R���̃t���[��
	GraphData iconFrame2P;
	iconFrame2P.posX = kIconPosX;
	iconFrame2P.posY = kIconPosY;
	iconFrame2P.handle = graphManager.GetHandle("IconFrame2P");
	//�A���t�@�l��0�ɂ��Ĕ�\���ɂ���
	iconFrame2P.alpha = 0;
	m_drawGraphs[GraphName::kIconFrame2P] = iconFrame2P;

	//�ǂ�������I�����Ă���A�C�R���̃t���[���͔�\��
	GraphData iconFrameBoth;
	iconFrameBoth.posX = kIconPosX;
	iconFrameBoth.posY = kIconPosY;
	iconFrameBoth.handle = graphManager.GetHandle("IconFrameBoth");
	//�A���t�@�l��0�ɂ��Ĕ�\���ɂ���
	iconFrameBoth.alpha = 0;
	m_drawGraphs[GraphName::kIconFrameBoth] = iconFrameBoth;

	//HeadSet�̉摜�ݒ�
	GraphData headSet;
	headSet.posX = kHeadSetPosX;
	headSet.posY = kHeadSetPosY;
	headSet.handle = graphManager.GetHandle("HeadSet");
	m_drawGraphs[GraphName::kHeadSet] = headSet;

	//VS�̉摜�ݒ�
	GraphData vs;
	vs.posX = kVsPosX;
	vs.posY = kVsPosY;
	vs.handle = graphManager.GetHandle("Vs");
	m_drawGraphs[GraphName::kVs] = vs;

	//���̉摜�ݒ�
	GraphData leftArrow;
	leftArrow.posX = kLbPosX;
	leftArrow.posY = kLevelSelectButtonPosY;
	leftArrow.handle = graphManager.GetHandle("Arrow");
	leftArrow.isReverseX = true;
	m_drawGraphs[GraphName::kLeftArrow] = leftArrow;

	GraphData rightArrow;
	rightArrow.posX = kRbPosX;
	rightArrow.posY = kLevelSelectButtonPosY;
	rightArrow.handle = graphManager.GetHandle("Arrow");
	m_drawGraphs[GraphName::kRightArrow] = rightArrow;

	//RB�̉摜�ݒ�
	GraphData rb;
	rb.posX = kRbPosX;
	rb.posY = kLevelSelectButtonPosY;
	rb.handle = graphManager.GetHandle("RB");
	m_drawGraphs[GraphName::kRB] = rb;

	//LB�̉摜�ݒ�
	GraphData lb;
	lb.posX = kLbPosX;
	lb.posY = kLevelSelectButtonPosY;
	lb.handle = graphManager.GetHandle("LB");
	m_drawGraphs[GraphName::kLB] = lb;

	//�����̉摜�ݒ�
	GraphData musicNote;
	musicNote.posX = kMusicNotePosX;
	musicNote.posY = kMusicNotePosY;
	musicNote.handle = graphManager.GetHandle("MusicNote");
	m_drawGraphs[GraphName::kMusicNote] = musicNote;
}

void SelectUi::Update()
{
	(this->*m_updateFunc)();

	m_time += kLevelSelectArrowShakeSpeed;

	//���̉摜��h�炷
	m_drawGraphs[GraphName::kLeftArrow].posX = kLbPosX - static_cast<int>(sinf(m_time) * kLevelSelectArrowShakeScale);
	m_drawGraphs[GraphName::kRightArrow].posX = kRbPosX + static_cast<int>(sinf(m_time) * kLevelSelectArrowShakeScale);

	//���̉摜�̍��W���N�����v
	m_drawGraphs[GraphName::kLeftArrow].posX = min(m_drawGraphs[GraphName::kLeftArrow].posX, kLbPosX);
	m_drawGraphs[GraphName::kRightArrow].posX = max(m_drawGraphs[GraphName::kRightArrow].posX, kRbPosX);

	//�X�J�C�h�[������]������
	MV1SetRotationXYZ(m_skyDomeHandle, VGet(0, MV1GetRotationXYZ(m_skyDomeHandle).y + kSkyDomeRotaSpeed, 0));
}

void SelectUi::Draw()
{
	//�X�J�C�h�[���̕`��
	MV1DrawModel(m_skyDomeHandle);

	//�摜���܂Ƃ߂ĕ\������
	for (auto& item : m_drawGraphs)
	{
		auto graph = item.second;

		if (graph.alpha < 255)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, graph.alpha);
		}

		DrawRotaGraph(graph.posX, graph.posY, graph.scale, 0.0, graph.handle, true, graph.isReverseX);

		if (graph.alpha < 255)
		{
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}

	//�������\������
	{
		//�E���̓�Փx
		MyEngine::Vector2 pos = MyEngine::Vector2(kRightLevelPosX, kLevelPosY);

		//����̃��x��
		int nextLevel = m_level + 1;

		if (nextLevel > static_cast<int>(EnemyInput::AiLevel::kHard))
		{
			nextLevel = static_cast<int>(EnemyInput::AiLevel::kEasy);
		}


		//������̍��W���E�����ɐݒ�
		pos.x -= GetFontSizeToHandle(m_levelFontHandle) * kLevelString[nextLevel].size();

		DrawStringToHandle(static_cast<int>(pos.x), static_cast<int>(pos.y), kLevelString[nextLevel].c_str(), kLevelColor[nextLevel], m_levelFontHandle);

		//�����̓�Փx
		pos = MyEngine::Vector2(kLeftLevelPosX, kLevelPosY);

		//��O�̃��x��
		int prevLevel = m_level - 1;

		if (prevLevel < static_cast<int>(EnemyInput::AiLevel::kEasy))
		{
			prevLevel = static_cast<int>(EnemyInput::AiLevel::kHard);
		}

		DrawStringToHandle(static_cast<int>(pos.x), static_cast<int>(pos.y), kLevelString[prevLevel].c_str(), kLevelColor[prevLevel], m_levelFontHandle);

		//�Ȗ��̃A���t�@�l�𑝂₷
		m_musicNameAlpha += kMusicNameFadeInSpeed;

		//�Ȗ��̍��W�����ɂ��炷
		m_musicNamePosX -= kMusicNameMoveSpeed;

		//�Ȗ��̍��W���N�����v
		m_musicNamePosX = max(m_musicNamePosX, kMusicNamePosX);

		//�u�����h���[�h��ݒ�
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, min(m_musicNameAlpha, 255));

		//�Ȗ���\������
		DrawStringToHandle(m_musicNamePosX, kMusicNamePosY, m_musicName.c_str(), kMusicNameColor, m_musicNameFontHandle);

		//�u�����h���[�h�����ɖ߂�
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void SelectUi::SetNumber(int number, bool isPlayer)
{
	auto& graphManager = GraphManager::GetInstance();

	if (isPlayer)
	{
		//�O�̉摜��\������
		if (m_lastPlayerNumber != number)
		{
			m_drawGraphs[GraphName::kLastPlayer].alpha = 255;
			m_drawGraphs[GraphName::kLastPlayer].posX = kPlayerCharacterPosX;
			//�n���h�����X�V����
			m_drawGraphs[GraphName::kLastPlayer].handle = graphManager.GetHandle(kCharacterNames[m_lastPlayerNumber]);

			//���݂̉摜�����ɂ��炷
			m_drawGraphs[GraphName::kPlayer].posX = kPlayerCharacterPosX + kCharacterPopDistance;
			m_drawGraphs[GraphName::kPlayer].alpha = 0;

		}

		m_lastPlayerNumber = m_playerNumber;
		m_playerNumber = number;
		//�n���h�����X�V����
		m_drawGraphs[GraphName::kPlayer].handle = graphManager.GetHandle(kCharacterNames[m_playerNumber]);
	}
	else
	{
		if (m_lastEnemyNumber != number)
		{
			m_drawGraphs[GraphName::kLastEnemy].alpha = 255;
			m_drawGraphs[GraphName::kLastEnemy].posX = kEnemyCharacterPosX;
			//�n���h�����X�V����
			m_drawGraphs[GraphName::kLastEnemy].handle = graphManager.GetHandle(kCharacterNames[m_lastEnemyNumber]);

			//���݂̉摜�����ɂ��炷
			m_drawGraphs[GraphName::kEnemy].posX = kPlayerCharacterPosX + kCharacterPopDistance;
			m_drawGraphs[GraphName::kEnemy].alpha = 0;


		}

		m_lastEnemyNumber = m_enemyNumber;
		m_enemyNumber = number;
		//�n���h�����X�V����
		m_drawGraphs[GraphName::kEnemy].handle = graphManager.GetHandle(kCharacterNames[m_enemyNumber]);
	}
}

void SelectUi::SetIconFrame(int number, bool isPlayer)
{
	if (isPlayer)
	{
		m_drawGraphs[GraphName::kIconFrame1P].posX = kIconPosX + (kIconDistance * number);
	}
	else
	{
		m_drawGraphs[GraphName::kIconFrame2P].posX = kIconPosX + (kIconDistance * number);
	}
}

void SelectUi::ChangeSituation(UiSituation situation)
{
	switch (situation)
	{
	case UiSituation::kSelect1P:

		//1P�̃A�C�R���̃t���[����\������
		m_drawGraphs[GraphName::kIconFrame1P].alpha = 255;

		//2P�̃A�C�R���̃t���[�����\���ɂ���
		m_drawGraphs[GraphName::kIconFrame2P].alpha = 0;

		//�ǂ���Ƃ��I�����Ă���A�C�R���̃t���[�����\���ɂ���
		m_drawGraphs[GraphName::kIconFrameBoth].alpha = 0;

		m_updateFunc = &SelectUi::Update1P;
		break;
	case UiSituation::kSelect2P:

		//2P�̃A�C�R���̃t���[����\������
		m_drawGraphs[GraphName::kIconFrame2P].alpha = 255;

		m_updateFunc = &SelectUi::Update2P;
		break;
	}
}

void SelectUi::SetSkyDomeHandle(int handle)
{
	MV1SetTextureGraphHandle(m_skyDomeHandle, 0, handle, FALSE);
}

void SelectUi::SetMusicName(std::string musicName)
{
	m_musicName = musicName;

	//�Ȗ��̃A���t�@�l��0�ɂ���
	m_musicNameAlpha = 0;

	//�Ȗ��̍��W��������
	m_musicNamePosX = kMusicNameInitPosX;
}

void SelectUi::Update1P()
{
	auto& graphManager = GraphManager::GetInstance();

	//�O�̃t���[���I��ł����摜�����X�Ƀt�F�[�h�A�E�g������
	m_drawGraphs[GraphName::kLastPlayer].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kLastPlayer].alpha -= kLastCharacterGraphFadeSpeed;

	//���ݑI��ł���摜���t�F�[�h�C��������
	m_drawGraphs[GraphName::kPlayer].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kPlayer].posX = max(m_drawGraphs[GraphName::kPlayer].posX, kPlayerCharacterPosX);

	m_drawGraphs[GraphName::kPlayer].alpha += kLastCharacterGraphFadeSpeed;

	//�O�̃t���[���I��ł����摜�����X�Ƀt�F�[�h�A�E�g������
	m_drawGraphs[GraphName::kLastEnemy].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kLastEnemy].alpha -= kLastCharacterGraphFadeSpeed;
	//���ݑI��ł���摜���t�F�[�h�C��������
	m_drawGraphs[GraphName::kEnemy].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kEnemy].posX = max(m_drawGraphs[GraphName::kEnemy].posX, kEnemyCharacterPosX);
	m_drawGraphs[GraphName::kEnemy].alpha += kLastCharacterGraphFadeSpeed;

	//�I�����Ă���A�C�R���̃t���[�����g�k������
	m_iconFrameScalling += kIconFrameScallingSpeed;
	m_drawGraphs[GraphName::kIconFrame1P].scale = (sinf(m_iconFrameScalling) * kIconFrameAddScaleMax) + 1.0f;
}

void SelectUi::Update2P()
{
	auto& graphManager = GraphManager::GetInstance();

	//�O�̃t���[���I��ł����摜�����X�Ƀt�F�[�h�A�E�g������
	m_drawGraphs[GraphName::kLastPlayer].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kLastPlayer].alpha -= kLastCharacterGraphFadeSpeed;

	//���ݑI��ł���摜���t�F�[�h�C��������
	m_drawGraphs[GraphName::kPlayer].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kPlayer].posX = max(m_drawGraphs[GraphName::kPlayer].posX, kPlayerCharacterPosX);

	m_drawGraphs[GraphName::kPlayer].alpha += kLastCharacterGraphFadeSpeed;

	//�O�̃t���[���I��ł����摜�����X�Ƀt�F�[�h�A�E�g������
	m_drawGraphs[GraphName::kLastEnemy].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kLastEnemy].alpha -= kLastCharacterGraphFadeSpeed;
	//���ݑI��ł���摜���t�F�[�h�C��������
	m_drawGraphs[GraphName::kEnemy].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kEnemy].posX = max(m_drawGraphs[GraphName::kEnemy].posX, kEnemyCharacterPosX);
	m_drawGraphs[GraphName::kEnemy].alpha += kLastCharacterGraphFadeSpeed;
	//�I�����Ă���A�C�R���̃t���[�����g�k������
	m_iconFrameScalling += kIconFrameScallingSpeed;
	m_drawGraphs[GraphName::kIconFrame2P].scale = (sinf(m_iconFrameScalling) * kIconFrameAddScaleMax) + 1.0f;

	//�����I��������Ă�����
	if (m_playerNumber == m_enemyNumber)
	{
		m_drawGraphs[GraphName::kIconFrameBoth].alpha = 255;
		m_drawGraphs[GraphName::kIconFrameBoth].posX = kIconPosX + (kIconDistance * m_playerNumber);
		m_drawGraphs[GraphName::kIconFrameBoth].scale = m_drawGraphs[GraphName::kIconFrame2P].scale;

		//1P�̕\��������
		m_drawGraphs[GraphName::kIconFrame1P].alpha = 0;
	}
	else
	{
		m_drawGraphs[GraphName::kIconFrameBoth].alpha = 0;
		//1P�̃t���[���̊g�k�����ɖ߂�
		m_drawGraphs[GraphName::kIconFrame1P].scale = 1.0f;
		m_drawGraphs[GraphName::kIconFrame1P].alpha = 255;
	}
}