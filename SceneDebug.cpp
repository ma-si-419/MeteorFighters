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

	//上下入力でリピート入力を受け付ける時間
	constexpr int kRepeatInputTime = 15;

	//リピート入力の間隔
	constexpr int kRepeatInterval = 5;

	//選択していないときの色
	const int kUnSelectColor = GetColor(255, 255, 255);

	//選択しているときの色
	const int kSelectColor = GetColor(255, 0, 0);

	//コンボのUIの色
	const int kComboColor = GetColor(255, 255, 255);

	//コンボが更新時の拡大率
	constexpr double kComboScale = 0.55;

	//表示する文字列
	const std::string kSceneName[static_cast<int>(Game::Scene::kEnd)] =
	{
		"Title",
		"Menu",
		"Select",
		"Battle",
		"Tutorial"
	};

	//フォントのサイズ
	constexpr int kFontSize = 48;

	//コンボを表示する座標
	constexpr int kComboPosX[2] = { 200,Game::kWindowWidth - 200 };
	constexpr int kComboPosY = Game::kWindowHeight / 2;

	//コンボの初期座標
	constexpr int kComboInitPosX[2] = { -200,Game::kWindowWidth + 200 };

	//コンボが入ってくるときの速度
	constexpr int kComboMoveSpeed = 60;

	//コンボの表示時間
	constexpr int kComboTime = 60;

	//コンボを消していく速度
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
	//数字の読み込み
	for (int i = 0; i < 10; i++)
	{
		m_numberGraphHandle[i] = LoadGraph(("data/image/" + std::to_string(i) + ".png").c_str());
	}

	//コンボの初期化
	m_combo = 0;
	m_comboTime = 0;
	m_comboPosX[0] = kComboInitPosX[0];
	m_comboPosX[1] = kComboInitPosX[1];
	m_comboScale[0] = 1.0;
	m_comboScale[1] = 1.0;
}

void SceneDebug::Update()
{
	//入力情報の取得
	auto input = MyEngine::Input::GetInstance().GetInputData(0);


	//前のフレームで選択していた項目
	int lastSelectScene = m_selectScene;

	//上下入力でシーン遷移
	if (input->IsTrigger("Up"))
	{
		lastSelectScene--;

		//最小値で下を押していたら最大値に戻す
		if (lastSelectScene < 0)
		{
			lastSelectScene = static_cast<int>(Game::Scene::kEnd) - 1;
		}
	}
	if (input->IsTrigger("Down"))
	{
		lastSelectScene++;

		//最大値で上を押していたら最小値に戻す
		if (lastSelectScene >= static_cast<int>(Game::Scene::kEnd))
		{
			lastSelectScene = 0;
		}
	}

	//リピート入力
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


	//クランプ
	lastSelectScene = max(lastSelectScene, 0);
	lastSelectScene = min(lastSelectScene, static_cast<int>(Game::Scene::kEnd) - 1);


	//選択している項目が変化していたら
	if (m_selectScene != lastSelectScene)
	{
		//項目を動かしてからのフレーム数をリセット
		m_selectSceneMoveTime = 0;

		//選択している項目を更新
		m_selectScene = lastSelectScene;

		//コンボ数を増やす
		m_combo++;

		//コンボの時間を初期化
		m_comboTime = 0;

		//コンボの拡大率を変更
		m_comboScale[1] = kComboScale;

		//コンボのアルファ値を初期化
		m_comboAlpha = 255;
	}
	else
	{
		//項目を動かしてからのフレーム数を増やす
		m_selectSceneMoveTime++;

		//コンボの時間を増やす
		m_comboTime++;
	}

	//コンボの時間が一定時間を超えたら
	if (m_comboTime > kComboTime)
	{
		//コンボのアルファ値を下げる
		m_comboAlpha -= kComboFadeSpeed;
		//コンボのアルファ値が0以下になったら
		if (m_comboAlpha <= 0)
		{
			//コンボを初期化
			m_combo = 0;
			m_comboPosX[0] = kComboInitPosX[0];
		}
	}

	//コンボの拡大率をあげてく
	m_comboScale[1] += 0.12;

	//コンボの拡大率が1.0を超えたら1.0にする
	m_comboScale[1] = min(m_comboScale[1], 1.0);

	if (m_combo > 0)
	{
		//コンボの座標を移動
		m_comboPosX[1] -= kComboMoveSpeed;
	}
	else
	{
		m_comboPosX[1] = kComboInitPosX[1];
	}

	//コンボの座標をクランプ
	m_comboPosX[1] = max(m_comboPosX[1], kComboPosX[1]);

	//決定ボタンが押されたら
	if (input->IsTrigger("A") && !m_sceneManager.IsChangeScene())
	{
		//次のシーンを作成する
		auto next = CreateScene(m_selectScene);

		//シーンを変更する
		m_sceneManager.ChangeScene(next);

		return;
	}
}

void SceneDebug::Draw()
{
	//基本的に白で描画
	int color[static_cast<int>(Game::Scene::kEnd)] =
	{
		kUnSelectColor,
		kUnSelectColor,
		kUnSelectColor,
		kUnSelectColor,
		kUnSelectColor
	};

	//選択している項目だけ赤にする
	color[m_selectScene] = kSelectColor;

	//文字列を描画
	for (int i = 0; i < static_cast<int>(Game::Scene::kEnd); i++)
	{
		DrawString(100, 100 + i * 50, kSceneName[i].c_str(), color[i]);
	}

	//コンボを描画

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

	//ブレンドモードを元に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}

void SceneDebug::End()
{
}

std::shared_ptr<SceneBase> SceneDebug::CreateScene(int sceneNum)
{
	Game::Scene scene = static_cast<Game::Scene>(sceneNum);

	//シーンに応じてシーンを作成する

	//タイトルシーン
	if (scene == Game::Scene::kTitle)
	{
		return std::make_shared<SceneTitle>(m_sceneManager);
	}
	//メニューシーン
	else if (scene == Game::Scene::kMenu)
	{
		return std::make_shared<SceneMenu>(m_sceneManager);
	}
	//セレクトシーン
	else if (scene == Game::Scene::kSelect)
	{
		return std::make_shared<SceneSelect>(m_sceneManager);
	}
	//バトルシーン
	else if (scene == Game::Scene::kBattle)
	{
		//バトルシーンの場合はキャラクターの設定を行う
		auto scene = std::make_shared<SceneGame>(m_sceneManager);
		scene->SetCharacter(1, 1, 1);
		scene->SetMusicName("デカヘッドは止まらない");
		return scene;
	}
	//チュートリアルシーン
	else if (scene == Game::Scene::kTutorial)
	{
		return std::make_shared<SceneTutorial>(m_sceneManager);
	}

	//デフォルトはタイトルシーン
	return std::make_shared<SceneTitle>(m_sceneManager);
}
