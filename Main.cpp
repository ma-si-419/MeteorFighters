#include "DxLib.h"
#include "SceneManager.h"
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
	bool isWindow = true;

#ifdef _DEBUG

	isWindow = true;

#endif // _DEBUG

	ChangeWindowMode(isWindow);

	SetGraphMode(Game::kWindowWidth, Game::kWindowHeight, 16);

	SetChangeScreenModeGraphicsSystemResetFlag(false);

	SetWindowText("�f�J�w�b�h");

	if (DxLib_Init() == -1)		// �c�w���C�u��������������
	{
		return -1;			// �G���[���N�����璼���ɏI��
	}

	//���C�g�֌W������
	SetUseLighting(true);
	SetUseZBuffer3D(true);
	SetWriteZBuffer3D(true);
	SetLightEnable(true);

	//Effekseer�֌W������
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	Effekseer_Init(kParticleMax);
	//Effekseer_InitDistortion();
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
	Effekseer_Sync3DSetting();

	//�t�H���g�̓ǂݍ���
	LPCSTR fontPath = "data/font/default.ttf";
	if (AddFontResourceEx(fontPath, FR_PRIVATE, NULL) > 0)
	{
	}
	else
	{
		// �t�H���g�Ǎ��G���[����
		assert(0 && "�t�H���g�ǂݍ��ݎ��s");
	}

	SetDrawScreen(DX_SCREEN_BACK);

	SoundManager::GetInstance().Init();
	GraphManager::GetInstance().Init();
	LoadManager::GetInstance().Init();
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

		auto& input = MyEngine::Input::GetInstance();

		// �Q�[���̏���
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

		// ��ʂ��؂�ւ��̂�҂�
		ScreenFlip();


		//�I���t���O�������Ă����
		if (sceneManager.IsEnd())
		{
			break;
		}

		// FPS60�ɌŒ肷��
		while (GetNowHiPerformanceCount() - start < 16667)
		{
			// 16.66�~���b(16667�}�C�N���b)�o�߂���܂ő҂�
		}
	}

	Effkseer_End();

	DxLib_End();				// �c�w���C�u�����g�p�̏I������

	return 0;				// �\�t�g�̏I�� 
}