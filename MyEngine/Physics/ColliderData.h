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
	/// •¨—‹““®‚ğ‚µ‚È‚¢‚©‚Ç‚¤‚©
	/// </summary>
	/// <param name="isTrigger">•¨—‹““®‚ğ‚µ‚È‚¢‚Æ‚«‚ÍfalseA‚·‚é‚Æ‚«‚Ítrue</param>
	void SetIsTrigger(bool isTrigger) { m_isTrigger = isTrigger; }

	/// <summary>
	/// “–‚½‚è”»’èí•Êæ“¾
	/// </summary>
	/// <returns>“–‚½‚è”»’è‚Ìí—Ş‚ğæ“¾‚·‚é</returns>
	Kind GetKind()const { return m_kind; }

	bool GetIsTrigger() { return m_isTrigger; }

private:
	//“–‚½‚è”»’è‚Ìí—Ş
	Kind m_kind;
	//•¨—‹““®‚ğ‚µ‚È‚¢‚Æ‚«‚Étrue‚É‚·‚é
	bool m_isTrigger;

};

