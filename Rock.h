#pragma once
#include "ObjectBase.h"

class Rock : public ObjectBase
{
public:
	
	Rock();
	
	virtual ~Rock();

	virtual void Init() override;

	virtual void Update() override;

	/// <summary>
	/// キャラクターの座標とぶつかった場所をセットする
	/// </summary>
	/// <param name="bumpPos">ぶつかった場所</param>
	/// <param name="characterPos">キャラクターの座標</param>
	void SetMoveVec(MyEngine::Vector3 bumpPos, MyEngine::Vector3 characterPos);
	
private:

	int m_time;

	MyEngine::Vector3 m_moveVec;
};

