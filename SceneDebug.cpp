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

	//�t�H���g�̖��O
	const std::string kFontName = "GN-�L���SU����NB";

	//�_���[�W��\��������W
	constexpr int kDamagePosX = Game::kWindowWidth - 250;
	constexpr int kDamagePosY = Game::kWindowHeight / 2 + 70;

	//�_���[�W�̏������W
	constexpr int kDamageInitPosX = Game::kWindowWidth + 300;

	//�R���{��\��������W
	constexpr int kComboPosX[2] = { 200,Game::kWindowWidth - 200 };
	constexpr int kComboPosY = Game::kWindowHeight / 2 - 50;

	//�R���{�̏������W
	constexpr int kComboInitPosX[2] = { -200,Game::kWindowWidth + 200 };

	//�R���{�̐����ȊO��UI��\��������W(�R���{�̍��W�̑��΍��W)
	const MyEngine::Vector2 kComboUIShiftVec = MyEngine::Vector2(50, 40);

	//�R���{�������Ă���Ƃ��̑��x
	constexpr int kComboMoveSpeed = 75;

	//������\������Ԋu
	constexpr float kNumberInterval = 65.0f;

	//�R���{�̕\������
	constexpr int kComboTime = 60;

	//�R���{�������Ă������x
	constexpr int kComboFadeSpeed = 35;

	//�\������_���[�W�𑝂₵�Ă�������
	constexpr int kShowDamageAddTime = 20;
}

SceneDebug::SceneDebug(SceneManager& sceneManager) :
	SceneBase(sceneManager),
	m_selectScene(0),
	m_selectSceneMoveTime(0),
	m_combo(0),
	m_comboTime(0),
	m_numberGraphHandle(),
	m_comboPos(),
	m_comboScale(),
	m_comboAlpha(0),
	m_damage(0),
	m_showDamage(0),
	m_showDamageAddTime(0),
	m_showDamageAddNum(0)
{
	//�_���[�W��\������ۂ̃t�H���g�̃n���h�����擾
	m_damageFontHandle = CreateFontToHandle(kFontName.c_str(), kFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 2);
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

	//�R���{��UI�̓ǂݍ���
	m_comboGraphHandle = LoadGraph("data/image/combo.png");

	//�R���{�̏�����
	m_combo = 0;
	m_comboTime = 0;
	m_comboPos = MyEngine::Vector2(kComboInitPosX[0], kComboPosY);
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

		//�_���[�W�𑝂₷
		m_damage += GetRand(1000);

		//�\������_���[�W�𑝂₷���Ԃ�������
		m_showDamageAddTime = 0;

		//1�t���[���ő��₷�_���[�W���v�Z
		m_showDamageAddNum = (m_damage - m_showDamage) / kShowDamageAddTime;

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
			m_comboPos.x = kComboInitPosX[1];
		}
	}

	//�\������_���[�W�𑝂₷���Ԃ���莞�Ԃ𒴂�����
	if (m_showDamageAddTime > kShowDamageAddTime)
	{
		//�_���[�W�ƕ\�����Ă���_���[�W�ʂ���v������
		m_showDamage = m_damage;
	}
	else
	{
		//�\�����Ă���_���[�W�𑝂₷
		m_showDamage += m_showDamageAddNum;
		m_showDamageAddTime++;
	}

	//�R���{�̊g�嗦�������Ă�
	m_comboScale[1] += 0.12;

	//�R���{�̊g�嗦��1.0�𒴂�����1.0�ɂ���
	m_comboScale[1] = min(m_comboScale[1], 1.0);

	if (m_combo > 0)
	{
		//�R���{�̍��W���ړ�
		m_comboPos.x -= kComboMoveSpeed;
		//�_���[�W�̍��W���ړ�
		m_damagePosX -= kComboMoveSpeed;
	}
	else
	{
		m_comboPos.x = kComboInitPosX[1];
		m_damagePosX = kDamageInitPosX;

		m_damage = 0;
		m_showDamage = 0;
		m_showDamageAddTime = 0;
		m_showDamageAddNum = 0;
	}

	//�R���{�̍��W���N�����v
	m_comboPos.x = max(m_comboPos.x, kComboPosX[1]);

	//�_���[�W�̍��W���N�����v
	m_damagePosX = max(m_damagePosX, kDamagePosX);

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

	//�w�i�𐅐F��
	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, GetColor(0, 192, 192), true);

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

	//�R���{UI��`��

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_comboAlpha);


	//�_���[�W��\��
	DrawFormatStringToHandle(m_damagePosX, kDamagePosY, GetColor(255, 255, 255), m_damageFontHandle, "%d", m_showDamage);

	//�R���{���ɂ���ď�����h��
	if (m_combo < 10)
	{
		DrawRotaGraph(m_comboPos.x, m_comboPos.y, m_comboScale[1], 0.0, m_numberGraphHandle[m_combo], true);

		MyEngine::Vector2 pos = m_comboPos + kComboUIShiftVec;

		//�R���{��UI��`��
		DrawRotaGraph(pos.x, pos.y, 1.0, 0.0, m_comboGraphHandle, true);
	}
	else
	{
		int ten = m_combo / 10;
		int one = m_combo % 10;

		//�\�̈�
		DrawRotaGraph(m_comboPos.x - kNumberInterval, kComboPosY, m_comboScale[1], 0.0, m_numberGraphHandle[ten], true);

		//��̈�
		DrawRotaGraph(m_comboPos.x, kComboPosY, m_comboScale[1], 0.0, m_numberGraphHandle[one], true);

		MyEngine::Vector2 pos = m_comboPos + kComboUIShiftVec;

		//�R���{��UI��`��
		DrawRotaGraph(pos.x, pos.y, 1.0, 0.0, m_comboGraphHandle, true);
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

void SceneDebug::DrawDamage(int damage)
{
	//�������擾
	int digit = 1;

	while(true)
	{
		int num = 1;

		for (int i = 0; i < digit; i++)
		{
			num *= 10;
		}

		if (damage <= num)
		{
			break;
		}

		digit++;
	}

	int digit = 1;
	while(damage > 0)
	{
		damage /= 10;
		digit++;
	}
	return digit;
	


}
