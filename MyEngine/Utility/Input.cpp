#include "Input.h"
#include "DxLib.h"

using namespace MyEngine;


MyEngine::Input::~Input()
{
}

void Input::Init()
{
	m_inputData[0] = std::make_shared <MyEngine::InputData>();
	m_inputData[1] = std::make_shared <MyEngine::InputData>();

	for (auto input : m_inputData)
	{
		input->Init();
	}

}
void Input::Update()
{
	//すべての入力を取得する
	char keyState[256] = {};
	int padState = {};
	int mouseState = {};
	GetHitKeyStateAll(keyState);
	mouseState = GetMouseInput();

	for (int i = 0; i < 2; i++)
	{
		if (i == 0)
		{
			padState = GetJoypadInputState(DX_INPUT_PAD1);
		}
		else
		{
			padState = GetJoypadInputState(DX_INPUT_PAD2);
		}

		auto actionMap = m_inputData[i]->GetInputActionMap();

		std::map<std::string, bool> inputMap;

		//アクション名に割り当てられているすべてのキーの入力をチェックする
		for (const auto& keyInfo : actionMap)
		{
			bool isPress = false;
			for (const auto& inputInfo : keyInfo.second)
			{
				//キーボードのチェック
				if (inputInfo.type == InputData::InputType::kKeyboard && keyState[inputInfo.buttonID])
				{
					isPress = true;
				}
				//パッドのチェック
				if (inputInfo.type == InputData::InputType::kPad && padState & inputInfo.buttonID)
				{
					isPress = true;
				}
				//マウスのチェック
				if (inputInfo.type == InputData::InputType::kMouse && mouseState & inputInfo.buttonID)
				{
					isPress = true;
				}

				//ボタンが押されていたらループを抜ける
				if (isPress)
				{
					break;
				}
			}
			//現在のフレームで押されていたかどうかを返す
			inputMap[keyInfo.first] = isPress;
		}

		m_inputData[i]->SetInputMap(inputMap);


		//スティックの入力
		InputData::StickInfo stickInfo;

		//トリガーの入力
		XINPUT_STATE* xInputState = new XINPUT_STATE;
		if (i == 0)
		{
			GetJoypadAnalogInput(&stickInfo.leftStickX, &stickInfo.leftStickY, DX_INPUT_PAD1);
			GetJoypadAnalogInputRight(&stickInfo.rightStickX, &stickInfo.rightStickY, DX_INPUT_PAD1);
			GetJoypadXInputState(DX_INPUT_PAD1, xInputState);
		}
		else
		{
			GetJoypadAnalogInput(&stickInfo.leftStickX, &stickInfo.leftStickY, DX_INPUT_PAD2);
			GetJoypadAnalogInputRight(&stickInfo.rightStickX, &stickInfo.rightStickY, DX_INPUT_PAD2);
			GetJoypadXInputState(DX_INPUT_PAD2, xInputState);
		}

		InputData::TriggerInfo triggerInfo;
		triggerInfo.left = xInputState->LeftTrigger;
		triggerInfo.right = xInputState->RightTrigger;

		//スティックの入力を設定する
		m_inputData[i]->SetStickInfo(stickInfo);

		//トリガーの情報を設定する
		m_inputData[i]->SetTriggerInfo(triggerInfo);

	}
}

void MyEngine::Input::StopAllInput()
{
	for (int i = 0; i < 2; i++)
	{
		m_inputData[i]->StopInput();
	}
}
