#include "InputData.h"
#include "DxLib.h"

using namespace MyEngine;

namespace
{
	//�g���K�[�������Ă���Ɣ��肷�鋭��
	constexpr int kTriggerPower = 128;
	//���t���[���O�̓��͂܂Ŏc����
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
	//�C���v�b�g��ۑ����鐔
	m_lastInput.resize(kLastInputSaveNum);
	m_lastTriggerInfo.resize(kLastInputSaveNum);

	//�{�^���̐ݒ�
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
	//�O�̃t���[���ƈႤ���͂ɂ���
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
	//�O�̃t���[���̓��͒l��ۑ�����
	for (int i = kLastInputSaveNum - 1; i > 0; i--)
	{
		//�ۑ����Ă���C���v�b�g�����炷
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
	//�O�̃t���[���̓��͒l��ۑ�����
	for (int i = kLastInputSaveNum - 1; i > 0; i--)
	{
		//�ۑ����Ă���g���K�[�̏������炷
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

	//�{�^��������`����Ă��Ȃ�������false��Ԃ�
	if (keyInfo == m_currentInput.end())
	{
		return false;
	}
	else
	{
		//����������bool�̒l��Ԃ�
		return keyInfo->second;
	}
}

int MyEngine::InputData::GetPressTime(const std::string& action) const
{
	int pressTime = 0;
	//�܂�������Ă��邩�ǂ�������
	if (IsPress(action))
	{
		for (int i = 0; i < m_lastInput.size(); i++)
		{
			auto last = m_lastInput[i].find(action);
			//����`�̃{�^������������0��Ԃ�
			if (last == m_lastInput[i].end())
			{
				return 0;
			}
			//���t���[���O���牟����Ă��邩���ׂ�
			else
			{
				//������Ă����玞�ԃv���X
				if (last->second)
				{
					pressTime++;
				}
				//������Ă����炻�̃t���[���܂ł̎��Ԃ�Ԃ�
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
	//�܂�������Ă��邩�ǂ�������
	if (IsPress(action))
	{
		//�O�̃t���[�����Q��
		auto last = m_lastInput[0].find(action);
		//����`�̃{�^������������false��Ԃ�
		if (last == m_lastInput[0].end())
		{
			return false;
		}
		//�O�̃t���[���ł�������Ă�����
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
	//�S�Ẵ{�^���𒲂ׂ�
	for (auto& item : m_currentInput)
	{
		//������Ă�����true��Ԃ�
		if (item.second)
		{
			return true;
		}
	}

	//������Ă���{�^�����Ȃ����false��Ԃ�
	return false;
}

bool MyEngine::InputData::IsRelease(const std::string& action) const
{
	//�܂�������Ă��邩����
	if (IsPress(action))
	{
		//������Ă�����false��Ԃ�
		return false;
	}
	//������Ă��Ȃ����
	else
	{
		//�O�̃t���[�����Q��
		auto last = m_lastInput[0].find(action);
		//����`�̃{�^������������false��Ԃ�
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
	//�g���K�[�̏�Ԏ擾
	auto trigger = GetTriggerInfo();

	//�E���𔻒肷��ꍇ
	if (right)
	{
		//�������������������܂ꂽ��
		if (trigger.right >= power)
		{
			return true;
		}
	}
	//�����𔻒肷��ꍇ
	else
	{
		//�������������������܂ꂽ��
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
	//���݂̃g���K�[�̏�Ԏ擾
	auto trigger = GetTriggerInfo();

	int pushTime = 0;

	//�E���𔻒肷��ꍇ
	if (right)
	{
		//�܂��͍��̃t���[��������Ă��邩���擾
		if (trigger.right >= power)
		{
			//������Ă���ΑO�̃t���[�������ĉ��t���[���O���牟���Ă��邩��Ԃ�
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
		//������Ă��Ȃ����0��Ԃ�
		else
		{
			return 0;
		}
	}
	//�����𔻒肷��ꍇ
	else
	{
		//�܂��͍��̃t���[��������Ă��邩���擾
		if (trigger.left >= power)
		{
			//������Ă���ΑO�̃t���[�������ĉ��t���[���O���牟���Ă��邩��Ԃ�
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
		//������Ă��Ȃ����0��Ԃ�
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
