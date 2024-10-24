#include "DxLib.h"
#include "Input.h"
using namespace MyEngine;

namespace
{
	//�g���K�[�������Ă���Ɣ��肷�鋭��
	constexpr float kTriggerPower = 128;
}

MyEngine::Input::~Input()
{
}

void Input::Init()
{
	//�{�^���̐ݒ�
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
	//�O�̃t���[���̓��͏���ۑ�����
	m_lastInput = m_currentInput;

	//���ׂĂ̓��͂��擾����
	char keyState[256] = {};
	int padState = {};
	int mouseState = {};
	GetHitKeyStateAll(keyState);
	padState = GetJoypadInputState(DX_INPUT_PAD1);
	mouseState = GetMouseInput();


	//�A�N�V�������Ɋ��蓖�Ă��Ă��邷�ׂẴL�[�̓��͂��`�F�b�N����
	for (const auto& keyInfo : m_inputActionMap)
	{
		bool isPress = false;
		for (const auto& inputInfo : keyInfo.second)
		{
			//�L�[�{�[�h�̃`�F�b�N
			if (inputInfo.type == InputType::kKeyboard && keyState[inputInfo.buttonID])
			{
				isPress = true;
			}
			//�p�b�h�̃`�F�b�N
			if (inputInfo.type == InputType::kPad && padState & inputInfo.buttonID)
			{
				isPress = true;
			}
			//�}�E�X�̃`�F�b�N
			if (inputInfo.type == InputType::kMouse && mouseState & inputInfo.buttonID)
			{
				isPress = true;
			}

			//�{�^����������Ă����烋�[�v�𔲂���
			if (isPress)
			{
				break;
			}
		}
		//���݂̃t���[���ŉ�����Ă������ǂ�����Ԃ�
		m_currentInput[keyInfo.first] = isPress;
	}

	//�X�e�B�b�N�̓��͂�����������
	m_stickInfo.leftStickX = 0;
	m_stickInfo.leftStickY = 0;
	m_stickInfo.rightStickX = 0;
	m_stickInfo.rightStickY = 0;

	//�X�e�B�b�N�̓��͂��擾����
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

	for (auto& item : m_lastInput)
	{
		item.second = false;
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

bool Input::IsTrigger(const std::string& action) const
{
	//�܂�������Ă��邩�ǂ�������
	if (IsPress(action))
	{
		//�O�̃t���[�����Q��
		auto last = m_lastInput.find(action);
		//����`�̃{�^������������false��Ԃ�
		if (last == m_lastInput.end())
		{
			return true;
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

bool MyEngine::Input::IsPushTrigger(bool right, int power)
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

bool MyEngine::Input::IsPushTrigger(bool right)
{
	return IsPushTrigger(right, kTriggerPower);
}
