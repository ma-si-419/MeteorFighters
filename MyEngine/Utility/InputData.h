#pragma once
#include <map>
#include <string>
#include <vector>
#include <list>
#include "Vector2.h"

namespace MyEngine
{
	class InputData
	{
	public:

		InputData();
		~InputData();

		void Init();

		/// <summary>
			/// �X�e�B�b�N�̌X����ۑ�����\����
			/// </summary>
		struct StickInfo
		{
			StickInfo()
			{
				leftStickX = 0;
				leftStickY = 0;
				rightStickX = 0;
				rightStickY = 0;
			}
			int leftStickX;
			int leftStickY;
			int rightStickX;
			int rightStickY;
		};

		struct TriggerInfo
		{
			TriggerInfo()
			{
				left = 0;
				right = 0;
			}
			int left;
			int right;
		};
	public:

		/// <summary>
		/// ���͎��
		/// </summary>
		enum class InputType
		{
			kKeyboard,
			kPad,
			kMouse
		};

		struct InputMapInfo
		{
			InputType type;
			int buttonID;
		};

	public:


		/// <summary>
		/// �{�^��������
		/// </summary>
		/// <param name="key">���̃{�^����</param>
		void PushButton(const std::string& key);

		/// <summary>
		/// �{�^����A�ł���
		/// </summary>
		/// <param name="key">���̃{�^����A�ł��邩</param>
		void BashButton(const std::string& key);

		/// <summary>
		/// �g���K�[������
		/// </summary>
		/// <param name="isLeft">���t�g�g���K�[�������ꍇtrue</param>
		void PushTrigger(bool isLeft);

		/// <summary>
		/// �X�e�B�b�N���X����
		/// </summary>
		/// <param name="stickTilt">�X�e�B�b�N�̌X��</param>
		/// <param name="isLeft">���̃X�e�B�b�N���X����Ȃ�true</param>
		void TiltStick(MyEngine::Vector2 stickTilt,bool isLeft);

		/// <summary>
		/// �L�[���͂�ݒ肷��
		/// </summary>
		/// <param name="inputMap">�ǂ̃L�[��������Ă��邩</param>
		void SetInputMap(std::map<std::string, bool> inputMap);

		/// <summary>
		/// �X�e�B�b�N�̓��͏���ݒ肷��
		/// </summary>
		/// <param name="stick">�X�e�B�b�N�̓��͏��</param>
		void SetStickInfo(StickInfo stick);

		/// <summary>
		/// �g���K�[���͂�ݒ肷��
		/// </summary>
		/// <param name="trigger">�g���K�[�̓��͏��</param>
		void SetTriggerInfo(TriggerInfo trigger);

		/// <summary>
		/// ���͂����ׂď���
		/// </summary>
		void StopInput();


		/// <summary>
		/// ���̃L�[���ݒ肳��Ă��邩���擾����
		/// </summary>
		/// <returns>�ݒ肳�ꂽ�L�[���</returns>
		std::map<std::string, std::vector<InputMapInfo>> GetInputActionMap() { return m_inputActionMap; }

		/// <summary>
		/// �L�[���������ǂ���
		/// </summary>
		/// <param name="keyName">���肵�����L�[</param>
		/// <returns>���������ǂ���</returns>
		bool IsPress(const std::string& action)const;

		/// <summary>
		/// �L�[��������Ă��鎞�Ԃ�Ԃ�(60�t���[���܂�)
		/// </summary>
		/// <param name="action">���肵�����L�[</param>
		/// <returns>�����Ă��鎞��</returns>
		int GetPressTime(const std::string& action)const;

		/// <summary>
		/// �L�[�������ꂽ�u�Ԃ����擾����
		/// </summary>
		/// <param name="action">���肵�����L�[</param>
		/// <returns>�����ꂽ�u�Ԃ̂�true��Ԃ�</returns>
		bool IsTrigger(const std::string& action)const;

		/// <summary>
		/// �����L�[�����������ǂ���
		/// </summary>
		/// <returns>�o�^����Ă���L�[�̂ǂꂩ�������ꂽ��true��Ԃ�</returns>
		bool IsAnyPress()const;

		/// <summary>
		/// �L�[�������ꂽ�u�Ԃ��擾����
		/// </summary>
		/// <param name="action">���肵�����L�[</param>
		/// <returns>�����ꂽ�u�Ԃ̂�True��Ԃ�</returns>
		bool IsRelease(const std::string& action)const;

		/// <summary>
		/// �X�e�B�b�N�̌X�����擾����
		/// </summary>
		/// <returns>�X�e�B�b�N���ǂꂾ���|��Ă��邩</returns>
		StickInfo GetStickInfo() { return m_stickInfo; }

		/// <summary>
		/// �g���K�[�̉������݋���擾����
		/// </summary>
		/// <returns>�g���K�[���ǂ̂��炢�������܂�Ă��邩</returns>
		TriggerInfo GetTriggerInfo() { return m_triggerInfo; }

		/// <summary>
		/// �g���K�[�{�^���������Ă��邩�ǂ�����Ԃ�(�������݂̋�����)
		/// </summary>
		/// <param name="right">�E���̃{�^�����擾�������Ȃ�true</param>
		/// <param name="power">�ǂ̂��炢�������܂�Ă�����true�Ƃ��邩(�ő�255,�ŏ�0)</param>
		/// <returns>������Ă�����true</returns>
		bool IsPushTrigger(bool right, int power);

		/// <summary>
		/// �g���K�[�{�^���������Ă��邩�ǂ�����Ԃ�(�������݂̋����Œ�)
		/// </summary>
		/// <param name="right">�E���̃{�^�����擾�������ꍇtrue</param>
		/// <returns>������Ă�����true</returns>
		bool IsPushTrigger(bool right);

		/// <summary>
		/// �g���K�[�{�^���������Ă��鎞�Ԃ�Ԃ�(�������݂̋�����)
		/// </summary>
		/// <param name="right">�E���̃g���K�[�̏����擾�������ꍇtrue</param>
		/// <param name="power">�ǂ̂��炢�������܂�Ă�����true�Ƃ��邩(�ő�255,�ŏ�0)</param>
		/// <returns>������Ă��鎞��</returns>
		int GetPushTriggerTime(bool right, int power);

		/// <summary>
		/// �g���K�[�{�^���������Ă��鎞�Ԃ�Ԃ�(�������݂̋����Œ�)
		/// </summary>
		/// <param name="right">�E���̃g���K�[�̏����擾�������ꍇtrue</param>
		/// <returns>������Ă��鎞��</returns>
		int GetPushTriggerTime(bool right);

	private:

		std::map<std::string, std::vector<InputMapInfo>> m_inputActionMap;

		//���݂̃t���[���̓���
		std::map<std::string, bool> m_currentInput;
		//�O�̓���
		std::vector<std::map<std::string, bool>>m_lastInput;

		StickInfo m_stickInfo = StickInfo();

		TriggerInfo m_triggerInfo = TriggerInfo();

		std::vector<TriggerInfo> m_lastTriggerInfo;

	};
}

