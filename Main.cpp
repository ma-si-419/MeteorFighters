#include "DxLib.h"
#include "SceneManager.h"
#include "SceneTitle.h"
#include "SceneDebug.h"
#include "GraphManager.h"
#include "SoundManager.h"
#include "LoadManager.h"
#include "Physics.h"
#include "Input.h"
#include "Game.h"
#include "EffekseerForDXLib.h"

namespace
{
	constexpr int kParticleMax = 8000;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	bool isWindow = false;

#ifdef _DEBUG

	isWindow = true;

#endif // _DEBUG

	ChangeWindowMode(isWindow);

	SetGraphMode(Game::kWindowWidth, Game::kWindowHeight, 16);

	SetChangeScreenModeGraphicsSystemResetFlag(false);

	SetWindowText("デカヘッド");

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	//ライト関係初期化
	SetUseLighting(true);
	SetUseZBuffer3D(true);
	SetWriteZBuffer3D(true);
	SetLightEnable(true);

	//Effekseer関係初期化
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	Effekseer_Init(kParticleMax);
	//Effekseer_InitDistortion();
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
	Effekseer_Sync3DSetting();

	//フォントの読み込み
	LPCSTR fontPath = "data/font/default.ttf";
	if (AddFontResourceEx(fontPath, FR_PRIVATE, NULL) > 0)
	{
	}
	else
	{
		// フォント読込エラー処理
		assert(0 && "フォント読み込み失敗");
	}

	SetDrawScreen(DX_SCREEN_BACK);

	SoundManager::GetInstance().Init();
	GraphManager::GetInstance().Init();
	LoadManager::GetInstance().Init();
	MyEngine::Input::GetInstance().Init();
	SceneManager sceneManager;

	std::shared_ptr<SceneBase> startScene;

	//タイトルシーン
	startScene = std::make_shared<SceneTitle>(sceneManager);

#ifdef _DEBUG

	//デバッグシーン
	startScene = std::make_shared<SceneDebug>(sceneManager);

#endif // _DEBUG

	sceneManager.Init(startScene);

#ifdef _DEBUG

	bool isStop = false;
	bool isMove = false;

#endif // _DEBUG


	// ゲームループ
	while (ProcessMessage() != -1)
	{
		// このフレームの開始時刻を覚えておく
		LONGLONG start = GetNowHiPerformanceCount();

		// 描画を行う前に画面をクリアする
		ClearDrawScreen();

		auto& input = MyEngine::Input::GetInstance();

		// ゲームの処理
		input.Update();
		LoadManager::GetInstance().Update();
#ifdef _DEBUG
		auto inputData = input.GetInputData(0);


		if (inputData->IsTrigger("LB"))
		{
			isMove = true;
		}

		if (!isStop || isMove)
		{
#endif // _DEBUG

			Physics::GetInstance().Update();

			sceneManager.Update();


#ifdef _DEBUG

		}

		isMove = false;

		if (inputData->IsTrigger("Select"))
		{
			isWindow = !isWindow;

			ChangeWindowMode(isWindow);

		}


#endif // _DEBUG

		sceneManager.Draw();

		LoadManager::GetInstance().Draw();

#ifdef _DEBUG

		DrawFormatString(1400, 0, GetColor(255, 0, 0), "FPS:%0.2f", GetFPS());

#endif // _DEBUG

		// 画面が切り替わるのを待つ
		ScreenFlip();


		//終了フラグがたっていれば
		if (sceneManager.IsEnd())
		{
			break;
		}

		// FPS60に固定する
		while (GetNowHiPerformanceCount() - start < 16667)
		{
			// 16.66ミリ秒(16667マイクロ秒)経過するまで待つ
		}
	}

	Effkseer_End();

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}