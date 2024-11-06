#include "DxLib.h"
#include "SceneManager.h"
#include "GraphManager.h"
#include "Physics.h"
#include "Input.h"
#include "Game.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	bool isWindow = false;

#ifdef _DEBUG

	isWindow = true;

#endif // _DEBUG

	ChangeWindowMode(isWindow);

	SetGraphMode(Game::kWindowWidth, Game::kWindowHeight, 16);

	SetChangeScreenModeGraphicsSystemResetFlag(false);

	SetWindowText("メテオファイターズ");

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}


	SetDrawScreen(DX_SCREEN_BACK);

	GraphManager::GetInstance().Init();
	MyEngine::Input::GetInstance().Init();
	SceneManager sceneManager;

	sceneManager.Init();

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

		// ゲームの処理
		MyEngine::Input::GetInstance().Update();
#ifdef _DEBUG


		if (MyEngine::Input::GetInstance().IsTrigger("A"))
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

		if (MyEngine::Input::GetInstance().IsTrigger("Pause"))
		{
			isStop = !isStop;
		}

		isMove = false;

		if (MyEngine::Input::GetInstance().IsTrigger("Select"))
		{
			isWindow = !isWindow;

			ChangeWindowMode(isWindow);

		}

		DrawFormatString(1400,0,GetColor(255,255,255),"FPS:%0.2f",GetFPS());

#endif // _DEBUG
		sceneManager.Draw();

		// 画面が切り替わるのを待つ
		ScreenFlip();

		// escキーでゲーム終了
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}

		// FPS60に固定する
		while (GetNowHiPerformanceCount() - start < 16667)
		{
			// 16.66ミリ秒(16667マイクロ秒)経過するまで待つ
		}
	}

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}