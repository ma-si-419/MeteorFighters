#pragma once
#include "Vector2.h"

class TitleUi
{
private:

	enum class Side
	{
		kUp,
		kRight,
		kDown,
		kLeft,
		kSideNum = kLeft
	};

public:

	TitleUi();

	~TitleUi();

	void Init();

	void Update();

	void Draw();

private:

	void DrawCharacter(Side side,MyEngine::Vector2 pos,int handle);

	MyEngine::Vector2 GetEnterMoveVec(Side side);

	MyEngine::Vector2 GetReturnMoveVec(Side side);

	MyEngine::Vector2 ClampPos(Side side,MyEngine::Vector2 pos);

	MyEngine::Vector2 SetRandomPos(Side side);

	/// <summary>
	/// 画面内にいるかどうかを返す
	/// </summary>
	/// <param name="pos">キャラクターの座標</param>
	/// <returns>画面に移っているのならばtrueを返す</returns>
	bool CheckPos(MyEngine::Vector2 pos);
private:

	int m_time;

	bool m_isExistString;

	int m_fontHandle;

	int m_skyDomeHandle;

	int m_mouseMoveTime;

	int m_blueHeadMoveTime;

	Side m_mouseSide;

	Side m_blueHeadSide;

	MyEngine::Vector2 m_mousePos;

	MyEngine::Vector2 m_blueHeadPos;
};

