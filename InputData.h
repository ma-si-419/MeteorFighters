#pragma once
#include <map>
#include <string>
#include <vector>
#include <list>

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

		void SetInputMap(std::map<std::string, bool> inputMap);

		void SetStickInfo(StickInfo stick);

		void SetTriggerInfo(TriggerInfo trigger);

		void StopInput();

		std::map<std::string, std::vector<InputMapInfo>> GetInputActionMap() { return m_inputActionMap; }

		/// <summary>
		/// �L�[���������ǂ���
		/// </summary>
		/// <param name="keyName">���肵�����L�[</param>
		/// <returns>���������ǂ���</returns>
		bool IsPress(const std::string& action)const;

		/// <summary>
		/// �L�[��������Ă��鎞�Ԃ�Ԃ�()
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

