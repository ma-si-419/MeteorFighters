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
	//���ׂĂ̓��͂��擾����
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

		//�A�N�V�������Ɋ��蓖�Ă��Ă��邷�ׂẴL�[�̓��͂��`�F�b�N����
		for (const auto& keyInfo : actionMap)
		{
			bool isPress = false;
			for (const auto& inputInfo : keyInfo.second)
			{
				//�L�[�{�[�h�̃`�F�b�N
				if (inputInfo.type == InputData::InputType::kKeyboard && keyState[inputInfo.buttonID])
				{
					isPress = true;
				}
				//�p�b�h�̃`�F�b�N
				if (inputInfo.type == InputData::InputType::kPad && padState & inputInfo.buttonID)
				{
					isPress = true;
				}
				//�}�E�X�̃`�F�b�N
				if (inputInfo.type == InputData::InputType::kMouse && mouseState & inputInfo.buttonID)
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
			inputMap[keyInfo.first] = isPress;
		}

		m_inputData[i]->SetInputMap(inputMap);


		//�X�e�B�b�N�̓���
		InputData::StickInfo stickInfo;

		//�g���K�[�̓���
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

		//�X�e�B�b�N�̓��͂�ݒ肷��
		m_inputData[i]->SetStickInfo(stickInfo);

		//�g���K�[�̏���ݒ肷��
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
