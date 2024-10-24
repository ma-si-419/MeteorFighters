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

	bool GetIsTrigger() { return m_isTrigger; }

private:
	//当たり判定の種類
	Kind m_kind;
	//物理挙動をしないときにtrueにする
	bool m_isTrigger;

};

