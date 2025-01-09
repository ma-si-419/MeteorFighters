#include "LoadManager.h"
#include "DxLib.h"
#include "EffekseerForDXLib.h"
#include "Game.h"

namespace
{
	//フェードインフェードアウトの速さ
	constexpr int kFadeSpeed = 20;

	//ローディングと表示する座標
	constexpr int kStringPosX = 500;
	constexpr int kStringPosY = 450;

	//ボールの初期座標
	constexpr int kBallInitPosX = -100;
	constexpr int kBallPosY = 750;
	//どこまで行ったら初期位置に戻るか
	constexpr int kBallFinalPosX = Game::kWindowWidth + 200;

	//ボールの最低速度
	constexpr int kBallMinSpeed = 5;

	//ボールのランダムの速度
	constexpr int kBallRandSpeed = 10;

	//ボールの回る速度
	constexpr double kBallRotaSpeed = 0.02;

	//移動速度によって増やす回る速度
	constexpr double kBallAddSpeedRate = 0.015;

	//フォントの名前
	const TCHAR* kFontName = "GN-キルゴUかなNB";
}

LoadManager::~LoadManager()
{
}

void LoadManager::Init()
{
	m_fontHandle = CreateFontToHandle(kFontName, 64, 0, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 3);

	m_isAsyncLoad = false;

	//ローディング中に使用する画像ロード
	m_ballGraphs[static_cast<int>(BallKind::kBlue)].handle = LoadGraph("data/image/BlueBall.png");
	m_ballGraphs[static_cast<int>(BallKind::kGreen)].handle = LoadGraph("data/image/GreenBall.png");
	m_ballGraphs[static_cast<int>(BallKind::kYellow)].handle = LoadGraph("data/image/YellowBall.png");
	m_ballGraphs[static_cast<int>(BallKind::kOrange)].handle = LoadGraph("data/image/OrangeBall.png");


	//ローディング中に使用する画像の初期化
	for (auto& item : m_ballGraphs)
	{
		item.speed = GetRand(kBallRandSpeed) + kBallMinSpeed;
		item.posX = kBallInitPosX;
		item.posY = kBallPosY;
	}
}

void LoadManager::Update()
{
	//非同期ロードを行うとしていなければ早期リターン
	if (!m_isAsyncLoad) return;

	m_alpha += kFadeSpeed;

	//ロードするものがあればロードする
	if (m_handles.size() > 0)
	{
		//配列の分だけ回す
		for (auto& item : m_handles)
		{
			//ロードがすんでいたら行わない
			if (item.second.handle != -1) continue;

			item.second.handle = Load(item.second.path, item.second.kind);
		}
	}

	//画像の座標更新
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
	//非同期ロードが行われていなければ早期リターン
	if (!m_isAsyncLoad) return;

	//描画処理を書く

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_alpha);

	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, GetColor(0, 0, 0), true);

	DrawStringToHandle(kStringPosX, kStringPosY, "Loading...", GetColor(255, 255, 255), m_fontHandle, GetColor(192, 192, 192));

	//ボール表示
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
	//非同期ロードが行われていたら値をすべて-1で返す
	if (GetASyncLoadNum() > 0) return -1;

	int ans = m_handles[name].handle;

	//ハンドルが返ってきていたら
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
