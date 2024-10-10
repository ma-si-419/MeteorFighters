#pragma once
#include "Collidable.h"
class Attack : public Collidable
{
public:

	struct AttackStatus
	{
		int damage
	};

	/// <summary>
	/// 攻撃のステータスの設定
	/// </summary>
	void Init();

	/// <summary>
	/// 攻撃の更新(座標の更新など)
	/// </summary>
	void Update();

	/// <summary>
	/// 描画するものがあれば描画する
	/// </summary>
	void Draw();
private:



};

