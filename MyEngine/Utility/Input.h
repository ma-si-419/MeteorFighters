#pragma once
#include <map>
#include <string>
#include <vector>
#include <list>
namespace MyEngine
{
	class Input
	{
	private:
		Input() = default;
		~Input();
		Input(const Input&) = delete;
		Input& operator = (const Input&) = delete;
		Input(Input&&) = delete;
		Input& operator = (Input&&) = delete;
	public:

		static Input& GetInstance()
		{
			static Input instance;
			return instance;
		}
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

		/// <summary>
		/// �L�[����ݒ肷��
		/// </summary>
		void Init();

		/// <summary>
		/// ���͏����X�V����
		/// </summary>
		void Update();

		/// <summary>
		/// ���͏����󂯕t���Ȃ��悤�ɂ���
		/// </summary>
		void StopUpdate();

		/// <summary>
		/// �L�[���������ǂ���
		/// </summary>
		/// <param name="keyName">���肵�����L�[</param>
		/// <returns>���������ǂ���</returns>
		bool IsPress(const std::string& action)const;

		/// <summary>
		/// �L�[�������ꂽ�u�Ԃ����擾����
		/// </summary>
		/// <param name="action">���肵�����L�[</param>
		/// <returns>�����ꂽ�u�Ԃ̂�true��Ԃ�</returns>
		bool IsTrigger(const std::string& action)const;

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
		/// <returns></returns>
		bool IsPushTrigger(bool right, int power);

		/// <summary>
		/// �g���K�[�{�^���������Ă��邩�ǂ�����Ԃ�(�������݂̋����Œ�)
		/// </summary>
		/// <param name="right">�E���̃{�^�����擾�������ꍇ</param>
		/// <returns></returns>
		bool IsPushTrigger(bool right);

	private:
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


		std::map<std::string, std::vector<InputMapInfo>> m_inputActionMap;

		//���݂̃t���[���̓���
		std::map<std::string, bool> m_currentInput;
		//�P�t���[���O�̓���
		std::map<std::string, bool> m_lastInput;

		StickInfo m_stickInfo = StickInfo();

		TriggerInfo m_triggerInfo = TriggerInfo();
	};
}