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
		/// キー情報を設定する
		/// </summary>
		void Init();

		/// <summary>
		/// 入力情報を更新する
		/// </summary>
		void Update();

		/// <summary>
		/// 入力情報を持っているデータを返す
		/// </summary>
		/// <param name="index">何番目のデータを返すか</param>
		/// <returns>入力情報を持っているクラス</returns>
		std::shared_ptr<InputData> GetInputData(int index) { return m_inputData[index]; }

		/// <summary>
		/// 入力を受け付けないようにする
		/// </summary>
		void StopAllInput();
	private:

		std::shared_ptr<InputData> m_inputData[2];
	};
}