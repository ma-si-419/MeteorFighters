#include "InputData.h"
#include "DxLib.h"

using namespace MyEngine;

namespace
{
	//トリガーを押していると判定する強さ
	constexpr int kTriggerPower = 128;
	//何フレーム前の入力まで残すか
	constexpr int kLastInputSaveNum = 60;
}


MyEngine::InputData::InputData()
{
}

MyEngine::InputData::~InputData()
{
}

void MyEngine::InputData::Init()
{
	//インプットを保存する数
	m_lastInput.resize(kLastInputSaveNum);
	m_lastTriggerInfo.resize(kLastInputSaveNum);

	//ボタンの設定
	m_inputActionMap["Ok"] = { {InputType::kKeyboard,KEY_INPUT_RETURN},{InputType::kPad,PAD_INPUT_A} };
	m_inputActionMap["Pause"] = { {InputType::kKeyboard,KEY_INPUT_P}, {InputType::kPad,PAD_INPUT_R} };
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

void MyEngine::InputData::PushButton(const std::string& key)
{
	m_currentInput[key] = true;
}

void MyEngine::InputData::BashButton(const std::string& key)
{
	//前のフレームと違う入力にする
	m_currentInput[key] = !m_lastInput[0][key];
}

void MyEngine::InputData::PushTrigger(bool isLeft)
{
	if (isLeft)
	{
		m_triggerInfo.left = 255;
	}
	else
	{
		m_triggerInfo.right = 255;
	}
}

void MyEngine::InputData::TiltStick(MyEngine::Vector2 stickTilt, bool isLeft)
{
	if (isLeft)
	{
		m_stickInfo.leftStickX = stickTilt.x;
		m_stickInfo.leftStickY = stickTilt.y;
	}
	else
	{
		m_stickInfo.rightStickX = stickTilt.x;
		m_stickInfo.rightStickY = stickTilt.y;
	}
}

void MyEngine::InputData::SetInputMap(std::map<std::string, bool> inputMap)
{
	//前のフレームの入力値を保存する
	for (int i = kLastInputSaveNum - 1; i > 0; i--)
	{
		//保存しているインプットをずらす
		m_lastInput[i] = m_lastInput[i - 1];
	}

	m_lastInput[0] = m_currentInput;

	m_currentInput = inputMap;
}

void MyEngine::InputData::SetStickInfo(StickInfo stick)
{
	m_stickInfo = stick;
}

void MyEngine::InputData::SetTriggerInfo(TriggerInfo trigger)
{
	//前のフレームの入力値を保存する
	for (int i = kLastInputSaveNum - 1; i > 0; i--)
	{
		//保存しているトリガーの情報をずらす
		m_lastTriggerInfo[i] = m_lastTriggerInfo[i - 1];
	}

	m_lastTriggerInfo[0] = m_triggerInfo;

	m_triggerInfo = trigger;
}

void MyEngine::InputData::StopInput()
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

bool MyEngine::InputData::IsPress(const std::string& action) const
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

int MyEngine::InputData::GetPressTime(const std::string& action) const
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

bool MyEngine::InputData::IsTrigger(const std::string& action) const
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

bool MyEngine::InputData::IsAnyPress() const
{
	//全てのボタンを調べる
	for (auto& item : m_currentInput)
	{
		//押されていたらtrueを返す
		if (item.second)
		{
			return true;
		}
	}

	//押されているボタンがなければfalseを返す
	return false;
}

bool MyEngine::InputData::IsRelease(const std::string& action) const
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

bool MyEngine::InputData::IsPushTrigger(bool right, int power)
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

bool MyEngine::InputData::IsPushTrigger(bool right)
{
	return IsPushTrigger(right, kTriggerPower);
}

int MyEngine::InputData::GetPushTriggerTime(bool right, int power)
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

int MyEngine::InputData::GetPushTriggerTime(bool right)
{
	return GetPushTriggerTime(right, kTriggerPower);
}
