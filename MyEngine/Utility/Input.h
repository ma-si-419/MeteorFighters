#pragma once
#include "InputData.h"
#include <memory>
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
		/// �L�[����ݒ肷��
		/// </summary>
		void Init();

		/// <summary>
		/// ���͏����X�V����
		/// </summary>
		void Update();

		/// <summary>
		/// ���͏��������Ă���f�[�^��Ԃ�
		/// </summary>
		/// <param name="index">���Ԗڂ̃f�[�^��Ԃ���</param>
		/// <returns>���͏��������Ă���N���X</returns>
		std::shared_ptr<InputData> GetInputData(int index) { return m_inputData[index]; }

		/// <summary>
		/// ���͂��󂯕t���Ȃ��悤�ɂ���
		/// </summary>
		void StopAllInput();
	private:

		std::shared_ptr<InputData> m_inputData[2];
	};
}