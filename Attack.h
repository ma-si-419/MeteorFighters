#pragma once
#include "Collidable.h"
class Attack : public Collidable
{
public:

	/// <summary>
	/// 攻撃のステータス
	/// </summary>
	struct AttackStatus
	{
		int damage = 0;
		float speed = 0;
	};

	/// <summary>
	/// 攻撃のステータスの設定
	/// </summary>
	void Init(AttackStatus status);

	/// <summary>
	/// 攻撃の更新(座標の更新など)
	/// </summary>
	void Update();

	/// <summary>
	/// 描画するものがあれば描画する
	/// </summary>
	void Draw();
private:

	AttackStatus m_status;
};

