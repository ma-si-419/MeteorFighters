#include "LoadManager.h"
#include "DxLib.h"
#include "EffekseerForDXLib.h"
#include "Game.h"

namespace
{
	//�t�F�[�h�C���t�F�[�h�A�E�g�̑���
	constexpr int kFadeSpeed = 20;

	//���[�f�B���O�ƕ\��������W
	constexpr int kStringPosX = 500;
	constexpr int kStringPosY = 450;

	//�{�[���̏������W
	constexpr int kBallInitPosX = -100;
	constexpr int kBallPosY = 750;
	//�ǂ��܂ōs�����珉���ʒu�ɖ߂邩
	constexpr int kBallFinalPosX = Game::kWindowWidth + 200;

	//�{�[���̍Œᑬ�x
	constexpr int kBallMinSpeed = 5;

	//�{�[���̃����_���̑��x
	constexpr int kBallRandSpeed = 10;

	//�{�[���̉�鑬�x
	constexpr double kBallRotaSpeed = 0.02;

	//�ړ����x�ɂ���đ��₷��鑬�x
	constexpr double kBallAddSpeedRate = 0.015;

	//�t�H���g�̖��O
	const TCHAR* kFontName = "GN-�L���SU����NB";
}

LoadManager::~LoadManager()
{
}

void LoadManager::Init()
{
	m_fontHandle = CreateFontToHandle(kFontName, 64, 0, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 3);

	m_isAsyncLoad = false;

	//���[�f�B���O���Ɏg�p����摜���[�h
	m_ballGraphs[static_cast<int>(BallKind::kBlue)].handle = LoadGraph("data/image/BlueBall.png");
	m_ballGraphs[static_cast<int>(BallKind::kGreen)].handle = LoadGraph("data/image/GreenBall.png");
	m_ballGraphs[static_cast<int>(BallKind::kYellow)].handle = LoadGraph("data/image/YellowBall.png");
	m_ballGraphs[static_cast<int>(BallKind::kOrange)].handle = LoadGraph("data/image/OrangeBall.png");


	//���[�f�B���O���Ɏg�p����摜�̏�����
	for (auto& item : m_ballGraphs)
	{
		item.speed = GetRand(kBallRandSpeed) + kBallMinSpeed;
		item.posX = kBallInitPosX;
		item.posY = kBallPosY;
	}
}

void LoadManager::Update()
{
	//�񓯊����[�h���s���Ƃ��Ă��Ȃ���Α������^�[��
	if (!m_isAsyncLoad) return;

	m_alpha += kFadeSpeed;

	//���[�h������̂�����΃��[�h����
	if (m_handles.size() > 0)
	{
		//�z��̕�������
		for (auto& item : m_handles)
		{
			//���[�h������ł�����s��Ȃ�
			if (item.second.handle != -1) continue;

			item.second.handle = Load(item.second.path, item.second.kind);
		}
	}

	//�摜�̍��W�X�V
	for (auto& item : m_ballGraphs)
	{
		item.posX += item.speed;
		item.rota += kBallRotaSpeed + static_cast<double>(item.speed) * kBallAddSpeedRate;

		if (item.posX > kBallFinalPosX)
		{
			item.speed = GetRand(kBallRandSpeed) + kBallMinSpeed;
			item.posX = kBallInitPosX;
		}
	}


}

void LoadManager::Draw()
{
	//�񓯊����[�h���s���Ă��Ȃ���Α������^�[��
	if (!m_isAsyncLoad) return;

	//�`�揈��������

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_alpha);

	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, GetColor(0, 0, 0), true);

	DrawStringToHandle(kStringPosX, kStringPosY, "Loading...", GetColor(255, 255, 255), m_fontHandle, GetColor(192, 192, 192));

	//�{�[���\��
	for (const auto& item : m_ballGraphs)
	{
		DrawRotaGraph(item.posX, item.posY, 1.0, item.rota, item.handle, true);
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void LoadManager::LoadHandle(std::string name, std::string path, FileKind kind)
{
	LoadData pushData;

	pushData.path = path;
	pushData.kind = kind;
	pushData.handle = -1;

	m_handles[name] = pushData;
}

int LoadManager::GetHandle(std::string name)
{
	//�񓯊����[�h���s���Ă�����l�����ׂ�-1�ŕԂ�
	if (GetASyncLoadNum() > 0) return -1;

	int ans = m_handles[name].handle;

	//�n���h�����Ԃ��Ă��Ă�����
	if (ans != -1)
	{
		m_handles.erase(name);

	}

	return ans;
}

void LoadManager::StartAsyncLoad()
{
	m_isAsyncLoad = true;
	SetUseASyncLoadFlag(true);
	m_alpha = 0;
}

void LoadManager::EndAsyncLoad()
{
	m_isAsyncLoad = false;
	SetUseASyncLoadFlag(false);
}

bool LoadManager::IsEndLoad()
{
	return !m_isAsyncLoad;
}

int LoadManager::Load(std::string path, FileKind kind)
{
	int ans = -1;

	switch (kind)
	{
	case LoadManager::FileKind::kModel:

		ans = MV1LoadModel(path.c_str());

		break;
	case LoadManager::FileKind::kSound:

		ans = LoadSoundMem(path.c_str());

		break;
	case LoadManager::FileKind::kImage:

		ans = LoadGraph(path.c_str());

		break;
	case LoadManager::FileKind::kEffect:

		ans = LoadEffekseerEffect(path.c_str());

		break;
	default:

		ans = -1;

		break;
	}

	return ans;
}
