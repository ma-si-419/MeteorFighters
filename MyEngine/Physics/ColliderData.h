#pragma once
class ColliderData abstract
{
public:
	enum class Kind
	{
		kSphere,
		kCapsule
	};

public:

	struct float3
	{
		float3() { x = 0; y = 0; z = 0; }
		float3(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }

		float x;
		float y;
		float z;
	};

public:

	ColliderData(Kind kind) { m_kind = kind; m_isTrigger = false; m_isGround = false; m_isWall = false; }
	virtual ~ColliderData() {}

	/// <summary>
	/// 物理挙動をしないかどうか
	/// </summary>
	/// <param name="isTrigger">物理挙動をしないときはfalse、するときはtrue</param>
	void SetIsTrigger(bool isTrigger) { m_isTrigger = isTrigger; }

	/// <summary>
	/// 当たり判定種別取得
	/// </summary>
	/// <returns>当たり判定の種類を取得する</returns>
	Kind GetKind()const { return m_kind; }

	/// <summary>
	/// ステージの床に触れているかどうかを返す
	/// </summary>
	/// <returns>ステージの床に触れていたらtrue</returns>
	bool IsGround() { return m_isGround; }

	/// <summary>
	/// ステージの床に触れているかどうかを設定する
	/// </summary>
	/// <param name="flag">ステージの床に触れていたらtrue</param>
	void SetIsGround(bool flag) { m_isGround = flag; }

	/// <summary>
	/// ステージの壁に触れているかどうかを設定する
	/// </summary>
	/// <param name="flag">ステージの壁に触れていたらtrue</param>
	void SetIsWall(bool flag) { m_isWall = flag; }

	/// <summary>
	/// ステージの壁に触れているかどうかを返す
	///	</summary>
	/// <returns>ステージの壁に触れていたらtrue</returns>
	bool IsWall() { return m_isWall; }

	/// <summary>
	/// 物理挙動をしないかどうかを返す
	/// </summary>
	bool GetIsTrigger() { return m_isTrigger; }

private:
	//当たり判定の種類
	Kind m_kind;
	//物理挙動をしないときにtrueにする
	bool m_isTrigger;
	//地面とぶつかってるときにtrueにする
	bool m_isGround;
	//壁とぶつかってるときにtrueにする
	bool m_isWall;
};

