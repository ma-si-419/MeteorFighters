#include "SceneDebug.h"
#include "SceneTitle.h"
#include "SceneSelect.h"
#include "SceneMenu.h"
#include "SceneGame.h"
#include "SceneTutorial.h"
#include "SceneManager.h"
#include "Game.h"
#include "Input.h"
#include "DxLib.h"
#include <string>

namespace
{

	//�㉺���͂Ń��s�[�g���͂��󂯕t���鎞��
	constexpr int kRepeatInputTime = 15;

	//���s�[�g���͂̊Ԋu
	constexpr int kRepeatInterval = 5;

	//�I�����Ă��Ȃ��Ƃ��̐F
	const int kUnSelectColor = GetColor(255, 255, 255);

	//�I�����Ă���Ƃ��̐F
	const int kSelectColor = GetColor(255, 0, 0);

	//�R���{��UI�̐F
	const int kComboColor = GetColor(255, 255, 255);

	//�R���{���X�V���̊g�嗦
	constexpr double kComboScale = 0.55;

	//�\�����镶����
	const std::string kSceneName[static_cast<int>(Game::Scene::kEnd)] =
	{
		"Title",
		"Menu",
		"Select",
		"Battle",
		"Tutorial"
	};

	//�t�H���g�̃T�C�Y
	constexpr int kFontSize = 48;

	//�R���{��\��������W
	constexpr int kComboPosX[2] = { 200,Game::kWindowWidth - 200 };
	constexpr int kComboPosY = Game::kWindowHeight / 2;

	//�R���{�̏������W
	constexpr int kComboInitPosX[2] = { -200,Game::kWindowWidth + 200 };

	//�R���{�������Ă���Ƃ��̑��x
	constexpr int kComboMoveSpeed = 60;

	//�R���{�̕\������
	constexpr int kComboTime = 60;

	//�R���{�������Ă������x
	constexpr int kComboFadeSpeed = 35;

}

SceneDebug::SceneDebug(SceneManager& sceneManager) :
	SceneBase(sceneManager),
	m_selectScene(0),
	m_selectSceneMoveTime(0),
	m_combo(0),
	m_comboTime(0),
	m_numberGraphHandle(),
	m_comboPosX(),
	m_comboScale()
{
}

SceneDebug::~SceneDebug()
{
}

void SceneDebug::Init()
{
	//�����̓ǂݍ���
	for (int i = 0; i < 10; i++)
	{
		m_numberGraphHandle[i] = LoadGraph(("data/image/" + std::to_string(i) + ".png").c_str());
	}

	//�R���{�̏�����
	m_combo = 0;
	m_comboTime = 0;
	m_comboPosX[0] = kComboInitPosX[0];
	m_comboPosX[1] = kComboInitPosX[1];
	m_comboScale[0] = 1.0;
	m_comboScale[1] = 1.0;
}

void SceneDebug::Update()
{
	//���͏��̎擾
	auto input = MyEngine::Input::GetInstance().GetInputData(0);


	//�O�̃t���[���őI�����Ă�������
	int lastSelectScene = m_selectScene;

	//�㉺���͂ŃV�[���J��
	if (input->IsTrigger("Up"))
	{
		lastSelectScene--;

		//�ŏ��l�ŉ��������Ă�����ő�l�ɖ߂�
		if (lastSelectScene < 0)
		{
			lastSelectScene = static_cast<int>(Game::Scene::kEnd) - 1;
		}
	}
	if (input->IsTrigger("Down"))
	{
		lastSelectScene++;

		//�ő�l�ŏ�������Ă�����ŏ��l�ɖ߂�
		if (lastSelectScene >= static_cast<int>(Game::Scene::kEnd))
		{
			lastSelectScene = 0;
		}
	}

	//���s�[�g����
	if (m_selectSceneMoveTime > kRepeatInterval)
	{
		if (input->GetPressTime("Up") > kRepeatInputTime)
		{
			lastSelectScene--;
		}
		else if (input->GetPressTime("Down") > kRepeatInputTime)
		{
			lastSelectScene++;
		}
	}


	//�N�����v
	lastSelectScene = max(lastSelectScene, 0);
	lastSelectScene = min(lastSelectScene, static_cast<int>(Game::Scene::kEnd) - 1);


	//�I�����Ă��鍀�ڂ��ω����Ă�����
	if (m_selectScene != lastSelectScene)
	{
		//���ڂ𓮂����Ă���̃t���[���������Z�b�g
		m_selectSceneMoveTime = 0;

		//�I�����Ă��鍀�ڂ��X�V
		m_selectScene = lastSelectScene;

		//�R���{���𑝂₷
		m_combo++;

		//�R���{�̎��Ԃ�������
		m_comboTime = 0;

		//�R���{�̊g�嗦��ύX
		m_comboScale[1] = kComboScale;

		//�R���{�̃A���t�@�l��������
		m_comboAlpha = 255;
	}
	else
	{
		//���ڂ𓮂����Ă���̃t���[�����𑝂₷
		m_selectSceneMoveTime++;

		//�R���{�̎��Ԃ𑝂₷
		m_comboTime++;
	}

	//�R���{�̎��Ԃ���莞�Ԃ𒴂�����
	if (m_comboTime > kComboTime)
	{
		//�R���{�̃A���t�@�l��������
		m_comboAlpha -= kComboFadeSpeed;
		//�R���{�̃A���t�@�l��0�ȉ��ɂȂ�����
		if (m_comboAlpha <= 0)
		{
			//�R���{��������
			m_combo = 0;
			m_comboPosX[0] = kComboInitPosX[0];
		}
	}

	//�R���{�̊g�嗦�������Ă�
	m_comboScale[1] += 0.12;

	//�R���{�̊g�嗦��1.0�𒴂�����1.0�ɂ���
	m_comboScale[1] = min(m_comboScale[1], 1.0);

	if (m_combo > 0)
	{
		//�R���{�̍��W���ړ�
		m_comboPosX[1] -= kComboMoveSpeed;
	}
	else
	{
		m_comboPosX[1] = kComboInitPosX[1];
	}

	//�R���{�̍��W���N�����v
	m_comboPosX[1] = max(m_comboPosX[1], kComboPosX[1]);

	//����{�^���������ꂽ��
	if (input->IsTrigger("A") && !m_sceneManager.IsChangeScene())
	{
		//���̃V�[�����쐬����
		auto next = CreateScene(m_selectScene);

		//�V�[����ύX����
		m_sceneManager.ChangeScene(next);

		return;
	}
}

void SceneDebug::Draw()
{
	//��{�I�ɔ��ŕ`��
	int color[static_cast<int>(Game::Scene::kEnd)] =
	{
		kUnSelectColor,
		kUnSelectColor,
		kUnSelectColor,
		kUnSelectColor,
		kUnSelectColor
	};

	//�I�����Ă��鍀�ڂ����Ԃɂ���
	color[m_selectScene] = kSelectColor;

	//�������`��
	for (int i = 0; i < static_cast<int>(Game::Scene::kEnd); i++)
	{
		DrawString(100, 100 + i * 50, kSceneName[i].c_str(), color[i]);
	}

	//�R���{��`��

	SetDrawBlendMode(DX_BLENDMODE_ALPHA,m_comboAlpha);

	if (m_combo < 10)
	{
		DrawRotaGraph(m_comboPosX[1], kComboPosY, m_comboScale[1], 0.0, m_numberGraphHandle[m_combo], true);
	}
	else
	{
		int ten = m_combo / 10;
		int one = m_combo % 10;

		DrawRotaGraph(m_comboPosX[1] - 60, kComboPosY, m_comboScale[1], 0.0, m_numberGraphHandle[ten], true);
		DrawRotaGraph(m_comboPosX[1], kComboPosY, m_comboScale[1], 0.0, m_numberGraphHandle[one], true);
	}

	//�u�����h���[�h�����ɖ߂�
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}

void SceneDebug::End()
{
}

std::shared_ptr<SceneBase> SceneDebug::CreateScene(int sceneNum)
{
	Game::Scene scene = static_cast<Game::Scene>(sceneNum);

	//�V�[���ɉ����ăV�[�����쐬����

	//�^�C�g���V�[��
	if (scene == Game::Scene::kTitle)
	{
		return std::make_shared<SceneTitle>(m_sceneManager);
	}
	//���j���[�V�[��
	else if (scene == Game::Scene::kMenu)
	{
		return std::make_shared<SceneMenu>(m_sceneManager);
	}
	//�Z���N�g�V�[��
	else if (scene == Game::Scene::kSelect)
	{
		return std::make_shared<SceneSelect>(m_sceneManager);
	}
	//�o�g���V�[��
	else if (scene == Game::Scene::kBattle)
	{
		//�o�g���V�[���̏ꍇ�̓L�����N�^�[�̐ݒ���s��
		auto scene = std::make_shared<SceneGame>(m_sceneManager);
		scene->SetCharacter(1, 1, 1);
		scene->SetMusicName("�f�J�w�b�h�͎~�܂�Ȃ�");
		return scene;
	}
	//�`���[�g���A���V�[��
	else if (scene == Game::Scene::kTutorial)
	{
		return std::make_shared<SceneTutorial>(m_sceneManager);
	}

	//�f�t�H���g�̓^�C�g���V�[��
	return std::make_shared<SceneTitle>(m_sceneManager);
}
