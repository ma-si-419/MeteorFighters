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

	//フォントの名前
	const std::string kFontName = "GN-キルゴUかなNB";

	//ダメージを表示する座標
	constexpr int kDamagePosX = Game::kWindowWidth - 250;
	constexpr int kDamagePosY = Game::kWindowHeight / 2 + 70;

	//ダメージの初期座標
	constexpr int kDamageInitPosX = Game::kWindowWidth + 300;

	//コンボを表示する座標
	constexpr int kComboPosX[2] = { 200,Game::kWindowWidth - 200 };
	constexpr int kComboPosY = Game::kWindowHeight / 2 - 50;

	//コンボの初期座標
	constexpr int kComboInitPosX[2] = { -200,Game::kWindowWidth + 200 };

	//コンボの数字以外のUIを表示する座標(コンボの座標の相対座標)
	const MyEngine::Vector2 kComboUIShiftVec = MyEngine::Vector2(50, 40);

	//コンボが入ってくるときの速度
	constexpr int kComboMoveSpeed = 75;

	//数字を表示する間隔
	constexpr float kNumberInterval = 65.0f;

	//コンボの表示時間
	constexpr int kComboTime = 60;

	//コンボを消していく速度
	constexpr int kComboFadeSpeed = 35;

	//表示するダメージを増やしていく時間
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
	//ダメージを表示する際のフォントのハンドルを取得
	m_damageFontHandle = CreateFontToHandle(kFontName.c_str(), kFontSize, 0, DX_FONTTYPE_ANTIALIASING_EDGE, 0, 2);
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

	//コンボのUIの読み込み
	m_comboGraphHandle = LoadGraph("data/image/combo.png");

	//コンボの初期化
	m_combo = 0;
	m_comboTime = 0;
	m_comboPos = MyEngine::Vector2(kComboInitPosX[0], kComboPosY);
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

		//ダメージを増やす
		m_damage += GetRand(1000);

		//表示するダメージを増やす時間を初期化
		m_showDamageAddTime = 0;

		//1フレームで増やすダメージを計算
		m_showDamageAddNum = (m_damage - m_showDamage) / kShowDamageAddTime;

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
			m_comboPos.x = kComboInitPosX[1];
		}
	}

	//表示するダメージを増やす時間が一定時間を超えたら
	if (m_showDamageAddTime > kShowDamageAddTime)
	{
		//ダメージと表示しているダメージ量を一致させる
		m_showDamage = m_damage;
	}
	else
	{
		//表示しているダメージを増やす
		m_showDamage += m_showDamageAddNum;
		m_showDamageAddTime++;
	}

	//コンボの拡大率をあげてく
	m_comboScale[1] += 0.12;

	//コンボの拡大率が1.0を超えたら1.0にする
	m_comboScale[1] = min(m_comboScale[1], 1.0);

	if (m_combo > 0)
	{
		//コンボの座標を移動
		m_comboPos.x -= kComboMoveSpeed;
		//ダメージの座標を移動
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

	//コンボの座標をクランプ
	m_comboPos.x = max(m_comboPos.x, kComboPosX[1]);

	//ダメージの座標をクランプ
	m_damagePosX = max(m_damagePosX, kDamagePosX);

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

	//背景を水色に
	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, GetColor(0, 192, 192), true);

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

	//コンボUIを描画

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_comboAlpha);


	//ダメージを表示
	DrawFormatStringToHandle(m_damagePosX, kDamagePosY, GetColor(255, 255, 255), m_damageFontHandle, "%d", m_showDamage);

	//コンボ数によって処理を派生
	if (m_combo < 10)
	{
		DrawRotaGraph(m_comboPos.x, m_comboPos.y, m_comboScale[1], 0.0, m_numberGraphHandle[m_combo], true);

		MyEngine::Vector2 pos = m_comboPos + kComboUIShiftVec;

		//コンボのUIを描画
		DrawRotaGraph(pos.x, pos.y, 1.0, 0.0, m_comboGraphHandle, true);
	}
	else
	{
		int ten = m_combo / 10;
		int one = m_combo % 10;

		//十の位
		DrawRotaGraph(m_comboPos.x - kNumberInterval, kComboPosY, m_comboScale[1], 0.0, m_numberGraphHandle[ten], true);

		//一の位
		DrawRotaGraph(m_comboPos.x, kComboPosY, m_comboScale[1], 0.0, m_numberGraphHandle[one], true);

		MyEngine::Vector2 pos = m_comboPos + kComboUIShiftVec;

		//コンボのUIを描画
		DrawRotaGraph(pos.x, pos.y, 1.0, 0.0, m_comboGraphHandle, true);
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

void SceneDebug::DrawDamage(int damage)
{
	//桁数を取得
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
