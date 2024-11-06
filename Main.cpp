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

	SetWindowText("���e�I�t�@�C�^�[�Y");

	if (DxLib_Init() == -1)		// �c�w���C�u��������������
	{
		return -1;			// �G���[���N�����璼���ɏI��
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


	// �Q�[�����[�v
	while (ProcessMessage() != -1)
	{
		// ���̃t���[���̊J�n�������o���Ă���
		LONGLONG start = GetNowHiPerformanceCount();

		// �`����s���O�ɉ�ʂ��N���A����
		ClearDrawScreen();

		// �Q�[���̏���
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

		// ��ʂ��؂�ւ��̂�҂�
		ScreenFlip();

		// esc�L�[�ŃQ�[���I��
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}

		// FPS60�ɌŒ肷��
		while (GetNowHiPerformanceCount() - start < 16667)
		{
			// 16.66�~���b(16667�}�C�N���b)�o�߂���܂ő҂�
		}
	}

	DxLib_End();				// �c�w���C�u�����g�p�̏I������

	return 0;				// �\�t�g�̏I�� 
}