#include "DxLib.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ウィンドウモードに設定
	ChangeWindowMode(true);

	// ウィンドウの大きさ設定
	SetWindowSize(1280,720);

	// 初期化処理
	if (DxLib_Init() == -1) return -1;
	

	// 描画先を設定
	SetDrawScreen(DX_SCREEN_BACK);

	// ループ
	while (ProcessMessage() == 0 && ClearDrawScreen() == 0)
	{
		// スクリーンを更新
		ScreenFlip();
	}
	// 終了処理
	DxLib_End();
	return 0;
}

