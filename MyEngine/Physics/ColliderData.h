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

	/// <summary>
	/// �X�e�[�W�̏��ɐG��Ă��邩�ǂ�����Ԃ�
	/// </summary>
	/// <returns>�X�e�[�W�̏��ɐG��Ă�����true</returns>
	bool IsGround() { return m_isGround; }

	/// <summary>
	/// �X�e�[�W�̏��ɐG��Ă��邩�ǂ�����ݒ肷��
	/// </summary>
	/// <param name="flag">�X�e�[�W�̏��ɐG��Ă�����true</param>
	void SetIsGround(bool flag) { m_isGround = flag; }

	bool GetIsTrigger() { return m_isTrigger; }

private:
	//�����蔻��̎��
	Kind m_kind;
	//�������������Ȃ��Ƃ���true�ɂ���
	bool m_isTrigger;
	//�n�ʂƂԂ����Ă�Ƃ���true�ɂ���
	bool m_isGround;

};

