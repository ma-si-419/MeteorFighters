#pragma once
#include "Collidable.h"

class Rock : public Collidable
{
public:
	
	Rock();
	
	virtual ~Rock();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="handle">モデルハンドル</param>
	void Init(int handle);

	void Update();
	
	void Draw();

	void End();

	/// <summary>
	/// キャラクターの座標とぶつかった場所をセットする
	/// </summary>
	/// <param name="bumpPos">ぶつかった場所</param>
	/// <param name="characterPos">キャラクターの座標</param>
	void SetBumpPosAndCharacterPos(MyEngine::Vector3 bumpPos, MyEngine::Vector3 characterPos);
	
private:

	int m_alpha;

	int m_handle;

	MyEngine::Vector3 m_moveVec;
};

