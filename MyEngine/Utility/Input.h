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
		/// スティックの傾きを保存する構造体
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
		/// キー情報を設定する
		/// </summary>
		void Init();

		/// <summary>
		/// 入力情報を更新する
		/// </summary>
		void Update();

		/// <summary>
		/// 入力情報を受け付けないようにする
		/// </summary>
		void StopUpdate();

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

		/// <summary>
		/// トリガーボタンを押しているかどうかを返す(押し込みの強さ可変)
		/// </summary>
		/// <param name="right">右側のボタンが取得したいならtrue</param>
		/// <param name="power">どのくらい押し込まれていたらtrueとするか(最大255,最小0)</param>
		/// <returns></returns>
		bool IsPushTrigger(bool right, int power);

		/// <summary>
		/// トリガーボタンを押しているかどうかを返す(押し込みの強さ固定)
		/// </summary>
		/// <param name="right">右側のボタンを取得したい場合</param>
		/// <returns></returns>
		bool IsPushTrigger(bool right);

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

		//現在のフレームの入力
		std::map<std::string, bool> m_currentInput;
		//１フレーム前の入力
		std::map<std::string, bool> m_lastInput;

		StickInfo m_stickInfo = StickInfo();

		TriggerInfo m_triggerInfo = TriggerInfo();
	};
}