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
	/// �������������Ȃ����ǂ���
	/// </summary>
	/// <param name="isTrigger">�������������Ȃ��Ƃ���false�A����Ƃ���true</param>
	void SetIsTrigger(bool isTrigger) { m_isTrigger = isTrigger; }

	/// <summary>
	/// �����蔻���ʎ擾
	/// </summary>
	/// <returns>�����蔻��̎�ނ��擾����</returns>
	Kind GetKind()const { return m_kind; }

	bool GetIsTrigger() { return m_isTrigger; }

private:
	//�����蔻��̎��
	Kind m_kind;
	//�������������Ȃ��Ƃ���true�ɂ���
	bool m_isTrigger;

};

