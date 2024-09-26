#pragma once
#include <map>
#include <string>
#include <vector>
namespace MyEngine
{
	class Input
	{
	public:
		/// <summary>
		/// スティックの傾きを保存する構造体
		/// </summary>
		struct StickInfo
		{
			int leftStickX;
			int leftStickY;
			int rightStickX;
			int rightStickY;
		};

		struct TriggerInfo
		{
			int left;
			int right;
		};
		
		Input();

		/// <summary>
		/// 入力情報を更新する
		/// </summary>
		void Update();

		/// <summary>
		/// キー押したかどうか
		/// </summary>
		/// <param name="keyName">判定したいキー</param>
		/// <returns>押したかどうか</returns>
		bool IsPress(const std::string& action)const;

		/// <summary>
		/// キーが押された瞬間だけ取得する
		/// </summary>
		/// <param name="action">判定したいキー</param>
		/// <returns>押された瞬間のみtrueを返す</returns>
		bool IsTrigger(const std::string& action)const;

		/// <summary>
		/// スティックの傾きを取得する
		/// </summary>
		/// <returns>スティックがどれだけ倒れているか</returns>
		StickInfo GetStickInfo() { return m_stickInfo; }

		/// <summary>
		/// トリガーの押し込み具合を取得する
		/// </summary>
		/// <returns>トリガーがどのくらい押し込まれているか</returns>
		TriggerInfo GetTriggerInfo() { return m_triggerInfo; }
	private:
		/// <summary>
		/// 入力種類
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

		std::map<std::string, bool> m_currentInput;
		std::map<std::string, bool> m_lastInput;

		StickInfo m_stickInfo;

		TriggerInfo m_triggerInfo;
	};
}