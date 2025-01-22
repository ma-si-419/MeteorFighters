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
	public:

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

	public:


		/// <summary>
		/// ボタンを押す
		/// </summary>
		/// <param name="key">何のボタンか</param>
		void PushButton(const std::string& key);

		/// <summary>
		/// ボタンを連打する
		/// </summary>
		/// <param name="key">何のボタンを連打するか</param>
		void BashButton(const std::string& key);

		/// <summary>
		/// トリガーを押す
		/// </summary>
		/// <param name="isLeft">レフトトリガーを押す場合true</param>
		void PushTrigger(bool isLeft);

		/// <summary>
		/// スティックを傾ける
		/// </summary>
		/// <param name="stickTilt">スティックの傾き</param>
		/// <param name="isLeft">左のスティックを傾けるならtrue</param>
		void TiltStick(MyEngine::Vector2 stickTilt,bool isLeft);

		/// <summary>
		/// キー入力を設定する
		/// </summary>
		/// <param name="inputMap">どのキーが押されているか</param>
		void SetInputMap(std::map<std::string, bool> inputMap);

		/// <summary>
		/// スティックの入力情報を設定する
		/// </summary>
		/// <param name="stick">スティックの入力情報</param>
		void SetStickInfo(StickInfo stick);

		/// <summary>
		/// トリガー入力を設定する
		/// </summary>
		/// <param name="trigger">トリガーの入力情報</param>
		void SetTriggerInfo(TriggerInfo trigger);

		/// <summary>
		/// 入力をすべて消す
		/// </summary>
		void StopInput();


		/// <summary>
		/// 何のキーが設定されているかを取得する
		/// </summary>
		/// <returns>設定されたキー情報</returns>
		std::map<std::string, std::vector<InputMapInfo>> GetInputActionMap() { return m_inputActionMap; }

		/// <summary>
		/// キー押したかどうか
		/// </summary>
		/// <param name="keyName">判定したいキー</param>
		/// <returns>押したかどうか</returns>
		bool IsPress(const std::string& action)const;

		/// <summary>
		/// キーが押されている時間を返す(60フレームまで)
		/// </summary>
		/// <param name="action">判定したいキー</param>
		/// <returns>押している時間</returns>
		int GetPressTime(const std::string& action)const;

		/// <summary>
		/// キーが押された瞬間だけ取得する
		/// </summary>
		/// <param name="action">判定したいキー</param>
		/// <returns>押された瞬間のみtrueを返す</returns>
		bool IsTrigger(const std::string& action)const;

		/// <summary>
		/// 何かキーを押したかどうか
		/// </summary>
		/// <returns>登録されているキーのどれかが押されたらtrueを返す</returns>
		bool IsAnyPress()const;

		/// <summary>
		/// キーが離された瞬間を取得する
		/// </summary>
		/// <param name="action">判定したいキー</param>
		/// <returns>離された瞬間のみTrueを返す</returns>
		bool IsRelease(const std::string& action)const;

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
		/// <returns>押されていたらtrue</returns>
		bool IsPushTrigger(bool right, int power);

		/// <summary>
		/// トリガーボタンを押しているかどうかを返す(押し込みの強さ固定)
		/// </summary>
		/// <param name="right">右側のボタンを取得したい場合true</param>
		/// <returns>押されていたらtrue</returns>
		bool IsPushTrigger(bool right);

		/// <summary>
		/// トリガーボタンを押している時間を返す(押し込みの強さ可変)
		/// </summary>
		/// <param name="right">右側のトリガーの情報を取得したい場合true</param>
		/// <param name="power">どのくらい押し込まれていたらtrueとするか(最大255,最小0)</param>
		/// <returns>押されている時間</returns>
		int GetPushTriggerTime(bool right, int power);

		/// <summary>
		/// トリガーボタンを押している時間を返す(押し込みの強さ固定)
		/// </summary>
		/// <param name="right">右側のトリガーの情報を取得したい場合true</param>
		/// <returns>押されている時間</returns>
		int GetPushTriggerTime(bool right);

	private:

		std::map<std::string, std::vector<InputMapInfo>> m_inputActionMap;

		//現在のフレームの入力
		std::map<std::string, bool> m_currentInput;
		//前の入力
		std::vector<std::map<std::string, bool>>m_lastInput;

		StickInfo m_stickInfo = StickInfo();

		TriggerInfo m_triggerInfo = TriggerInfo();

		std::vector<TriggerInfo> m_lastTriggerInfo;

	};
}

