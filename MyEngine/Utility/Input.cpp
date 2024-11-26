#include "DxLib.h"
#include "Input.h"
using namespace MyEngine;

namespace
{
	//トリガーを押していると判定する強さ
	constexpr int kTriggerPower = 128;
	//何フレーム前の入力まで残すか
	constexpr int kLastInputSaveNum = 30;
}

MyEngine::Input::~Input()
{
}

void Input::Init()
{
	//インプットを保存する数
	m_lastInput.resize(kLastInputSaveNum);
	m_lastTriggerInfo.resize(kLastInputSaveNum);

	//ボタンの設定
	m_inputActionMap["Ok"] = { {InputType::kKeyboard,KEY_INPUT_RETURN},{InputType::kPad,PAD_INPUT_A} };
	m_inputActionMap["Pause"] = { {InputType::kKeyboard,KEY_INPUT_P}, {InputType::kPad,PAD_INPUT_START} };
	m_inputActionMap["Select"] = { {InputType::kKeyboard,KEY_INPUT_TAB}, {InputType::kPad,PAD_INPUT_7} };
	m_inputActionMap["Cancel"] = { {InputType::kKeyboard,KEY_INPUT_ESCAPE}, {InputType::kPad,PAD_INPUT_B} };
	m_inputActionMap["Up"] = { {InputType::kKeyboard,KEY_INPUT_W}, {InputType::kPad,PAD_INPUT_UP} };
	m_inputActionMap["Down"] = { {InputType::kKeyboard,KEY_INPUT_S}, {InputType::kPad,PAD_INPUT_DOWN} };
	m_inputActionMap["Left"] = { {InputType::kKeyboard,KEY_INPUT_A}, {InputType::kPad,PAD_INPUT_LEFT} };
	m_inputActionMap["Right"] = { {InputType::kKeyboard,KEY_INPUT_D}, {InputType::kPad,PAD_INPUT_RIGHT} };
	m_inputActionMap["LB"] = { {InputType::kKeyboard,KEY_INPUT_J}, {InputType::kPad,PAD_INPUT_5} };
	m_inputActionMap["RB"] = { {InputType::kKeyboard,KEY_INPUT_L}, {InputType::kPad,PAD_INPUT_6} };
	m_inputActionMap["A"] = { {InputType::kKeyboard,KEY_INPUT_U}, {InputType::kPad,PAD_INPUT_1} };
	m_inputActionMap["B"] = { {InputType::kKeyboard,KEY_INPUT_H}, {InputType::kPad,PAD_INPUT_2} };
	m_inputActionMap["X"] = { {InputType::kKeyboard,KEY_INPUT_G}, {InputType::kPad,PAD_INPUT_3} };
	m_inputActionMap["Y"] = { {InputType::kKeyboard,KEY_INPUT_Y}, {InputType::kPad,PAD_INPUT_4} };
}
void Input::Update()
{
	//前のフレームの入力値を保存する
	for (int i = kLastInputSaveNum - 1; i > 0; i--)
	{
		//保存しているインプットをずらす
		m_lastInput[i] = m_lastInput[i - 1];
	}

	m_lastInput[0] = m_currentInput;

	//前のフレームの入力値を保存する
	for (int i = kLastInputSaveNum - 1; i > 0; i--)
	{
		//保存しているトリガーの情報をずらす
		m_lastTriggerInfo[i] = m_lastTriggerInfo[i - 1];
	}

	m_lastTriggerInfo[0] = m_triggerInfo;


	//すべての入力を取得する
	char keyState[256] = {};
	int padState = {};
	int mouseState = {};
	GetHitKeyStateAll(keyState);
	padState = GetJoypadInputState(DX_INPUT_PAD1);
	mouseState = GetMouseInput();


	//アクション名に割り当てられているすべてのキーの入力をチェックする
	for (const auto& keyInfo : m_inputActionMap)
	{
		bool isPress = false;
		for (const auto& inputInfo : keyInfo.second)
		{
			//キーボードのチェック
			if (inputInfo.type == InputType::kKeyboard && keyState[inputInfo.buttonID])
			{
				isPress = true;
			}
			//パッドのチェック
			if (inputInfo.type == InputType::kPad && padState & inputInfo.buttonID)
			{
				isPress = true;
			}
			//マウスのチェック
			if (inputInfo.type == InputType::kMouse && mouseState & inputInfo.buttonID)
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
		m_currentInput[keyInfo.first] = isPress;
	}

	//スティックの入力を初期化する
	m_stickInfo.leftStickX = 0;
	m_stickInfo.leftStickY = 0;
	m_stickInfo.rightStickX = 0;
	m_stickInfo.rightStickY = 0;

	//スティックの入力を取得する
	GetJoypadAnalogInput(&m_stickInfo.leftStickX, &m_stickInfo.leftStickY, DX_INPUT_PAD1);
	GetJoypadAnalogInputRight(&m_stickInfo.rightStickX, &m_stickInfo.rightStickY, DX_INPUT_PAD1);
	XINPUT_STATE* xInputState = new XINPUT_STATE;
	GetJoypadXInputState(DX_INPUT_PAD1, xInputState);
	m_triggerInfo.left = xInputState->LeftTrigger;
	m_triggerInfo.right = xInputState->RightTrigger;
}

void MyEngine::Input::StopUpdate()
{
	for (auto& item : m_currentInput)
	{
		item.second = false;
	}

	for (auto& map : m_lastInput)
	{
		for (auto& item : map)
		{
			item.second = false;
		}
	}

	m_stickInfo.leftStickX = 0;
	m_stickInfo.leftStickY = 0;
	m_stickInfo.rightStickX = 0;
	m_stickInfo.rightStickY = 0;

	m_triggerInfo.left = 0;
	m_triggerInfo.right = 0;
}

bool Input::IsPress(const std::string& action) const
{
	auto keyInfo = m_currentInput.find(action);

	//ボタン名が定義されていなかったらfalseを返す
	if (keyInfo == m_currentInput.end())
	{
		return false;
	}
	else
	{
		//見つかったらboolの値を返す
		return keyInfo->second;
	}
}

int MyEngine::Input::GetPressTime(const std::string& action) const
{
	int pressTime = 0;
	//まず押されているかどうか判定
	if (IsPress(action))
	{
		for (int i = 0; i < m_lastInput.size(); i++)
		{
			auto last = m_lastInput[i].find(action);
			//未定義のボタン名が来たら0を返す
			if (last == m_lastInput[i].end())
			{
				return 0;
			}
			//何フレーム前から押されているか調べる
			else
			{
				//押されていたら時間プラス
				if (last->second)
				{
					pressTime++;
				}
				//離されていたらそのフレームまでの時間を返す
				else
				{
					return pressTime;
				}
			}
		}
	}
	else
	{
		return pressTime;
	}
	return pressTime;
}

bool Input::IsTrigger(const std::string& action) const
{
	//まず押されているかどうか判定
	if (IsPress(action))
	{
		//前のフレームを参照
		auto last = m_lastInput[0].find(action);
		//未定義のボタン名が来たらfalseを返す
		if (last == m_lastInput[0].end())
		{
			return false;
		}
		//前のフレームでも押されていたら
		else
		{
			return !last->second;
		}
	}
	else
	{
		return false;
	}
}

bool MyEngine::Input::IsRelease(const std::string& action) const
{
	//まず押されているか判定
	if (IsPress(action))
	{
		//押されていたらfalseを返す
		return false;
	}
	//押されていなければ
	else
	{
		//前のフレームを参照
		auto last = m_lastInput[0].find(action);
		//未定義のボタン名が来たらfalseを返す
		if (last == m_lastInput[0].end())
		{
			return false;
		}
		else
		{
			return last->second;
		}
	}


	return false;
}

bool MyEngine::Input::IsPushTrigger(bool right, int power)
{
	//トリガーの状態取得
	auto trigger = GetTriggerInfo();

	//右側を判定する場合
	if (right)
	{
		//引数よりも強く押し込まれたら
		if (trigger.right >= power)
		{
			return true;
		}
	}
	//左側を判定する場合
	else
	{
		//引数よりも強く押し込まれたら
		if (trigger.left >= power)
		{
			return true;
		}
	}
	return false;
}

bool MyEngine::Input::IsPushTrigger(bool right)
{
	return IsPushTrigger(right, kTriggerPower);
}

int MyEngine::Input::GetPushTriggerTime(bool right, int power)
{
	//現在のトリガーの状態取得
	auto trigger = GetTriggerInfo();

	int pushTime = 0;

	//右側を判定する場合
	if (right)
	{
		//まずは今のフレーム押されているかを取得
		if (trigger.right >= power)
		{
			//押されていれば前のフレームを見て何フレーム前から押しているかを返す
			for (int i = 0; i < kLastInputSaveNum; i++)
			{
				if (m_lastTriggerInfo[i].right > power)
				{
					pushTime++;
				}
				else
				{
					return pushTime;
				}
			}
		}
		//押されていなければ0を返す
		else
		{
			return 0;
		}
	}
	//左側を判定する場合
	else
	{
		//まずは今のフレーム押されているかを取得
		if (trigger.left >= power)
		{
			//押されていれば前のフレームを見て何フレーム前から押しているかを返す
			for (int i = 0; i < kLastInputSaveNum; i++)
			{
				if (m_lastTriggerInfo[i].left > power)
				{
					pushTime++;
				}
				else
				{
					return pushTime;
				}
			}
		}
		//押されていなければ0を返す
		else
		{
			return 0;
		}
	}
	return pushTime;
}

int MyEngine::Input::GetPushTriggerTime(bool right)
{
	return GetPushTriggerTime(right, kTriggerPower);
}
