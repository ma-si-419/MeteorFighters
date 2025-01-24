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

	/// <summary>
	/// �X�e�[�W�̕ǂɐG��Ă��邩�ǂ�����ݒ肷��
	/// </summary>
	/// <param name="flag">�X�e�[�W�̕ǂɐG��Ă�����true</param>
	void SetIsWall(bool flag) { m_isWall = flag; }

	/// <summary>
	/// �X�e�[�W�̕ǂɐG��Ă��邩�ǂ�����Ԃ�
	///	</summary>
	/// <returns>�X�e�[�W�̕ǂɐG��Ă�����true</returns>
	bool IsWall() { return m_isWall; }

	/// <summary>
	/// �������������Ȃ����ǂ�����Ԃ�
	/// </summary>
	bool GetIsTrigger() { return m_isTrigger; }

private:
	//�����蔻��̎��
	Kind m_kind;
	//�������������Ȃ��Ƃ���true�ɂ���
	bool m_isTrigger;
	//�n�ʂƂԂ����Ă�Ƃ���true�ɂ���
	bool m_isGround;
	//�ǂƂԂ����Ă�Ƃ���true�ɂ���
	bool m_isWall;
};

