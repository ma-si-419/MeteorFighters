#pragma once
class ColliderData abstract
{
public:
	enum class Kind
	{
		kSphere,
		kCapsule
	};

	ColliderData(Kind kind) { m_kind = kind; m_isTrigger = false; }
	virtual ~ColliderData(){}

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

	bool GetIsTrigger() { return m_isTrigger; }

private:
	//当たり判定の種類
	Kind m_kind;
	//物理挙動をしないときにtrueにする
	bool m_isTrigger;
	//地面とぶつかってるときにtrueにする
	bool m_isGround;

};

