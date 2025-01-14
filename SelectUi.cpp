#include "SelectUi.h"
#include "SelectManager.h"
#include "GraphManager.h"
#include "DxLib.h"

namespace
{
	//�J�����̐ݒ�
	constexpr float kCameraNear = 0.1f;
	constexpr float kCameraFar = 100.0f;


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

	const std::string kCharacterNames[static_cast<int>(SelectManager::CharacterNumber::kCharacterNum)] =
	{
		"Character0",
		"Random",
		"Character1"
	};
}

SelectUi::SelectUi() :
	m_lastPlayerNumber(0),
	m_lastEnemyNumber(0),
	m_playerNumber(0),
	m_enemyNumber(0),
	m_skyDomeHandle(-1),
	m_iconFrameScalling(0.0f)
{

}

SelectUi::~SelectUi()
{
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

	//�I�����Ă���A�C�R���̃t���[��
	GraphData iconFrame;
	iconFrame.posX = kIconPosX;
	iconFrame.posY = kIconPosY;
	iconFrame.handle = graphManager.GetHandle("IconFrame");
	m_drawGraphs[GraphName::kIconFrame] = iconFrame;

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

}

void SelectUi::Update()
{
	auto& graphManager = GraphManager::GetInstance();

	//�O�̃t���[���I��ł����摜�����X�Ƀt�F�[�h�A�E�g������
	m_drawGraphs[GraphName::kLastPlayer].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kLastPlayer].alpha -= kLastCharacterGraphFadeSpeed;
	m_drawGraphs[GraphName::kLastEnemy].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kLastEnemy].alpha -= kLastCharacterGraphFadeSpeed;

	//���ݑI��ł���摜���t�F�[�h�C��������
	m_drawGraphs[GraphName::kPlayer].posX -= kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kPlayer].posX = max(m_drawGraphs[GraphName::kPlayer].posX, kPlayerCharacterPosX);

	m_drawGraphs[GraphName::kPlayer].alpha += kLastCharacterGraphFadeSpeed;

	//2P��
	m_drawGraphs[GraphName::kEnemy].posX = kLastCharacterGraphMoveSpeed;
	m_drawGraphs[GraphName::kEnemy].posX = max(m_drawGraphs[GraphName::kEnemy].posX, kEnemyCharacterPosX);

	m_drawGraphs[GraphName::kEnemy].alpha += kLastCharacterGraphFadeSpeed;

	//�I�����Ă���A�C�R���̃t���[�����g�k������
	m_iconFrameScalling += kIconFrameScallingSpeed;
	m_drawGraphs[GraphName::kIconFrame].scale = (sinf(m_iconFrameScalling) * kIconFrameAddScaleMax) + 1.0f;

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

		DrawRotaGraph(graph.posX, graph.posY, graph.scale, 0.0, graph.handle, true);

		if (graph.alpha < 255)
		{
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}

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

void SelectUi::SetIconFrame(int number)
{
	m_drawGraphs[GraphName::kIconFrame].posX = kIconPosX + (kIconDistance * number);
}

void SelectUi::SetDrawIconFrame(bool flag)
{
	if (flag)
	{
		m_drawGraphs[GraphName::kIconFrame].alpha = 255;
	}
	else
	{
		m_drawGraphs[GraphName::kIconFrame].alpha = 0;
	}
}
