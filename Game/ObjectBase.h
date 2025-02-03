#pragma once
#include "Collidable.h"

class ObjectBase : public Collidable
{
public:
	ObjectBase(ColliderData::Kind colKind);
	virtual ~ObjectBase();

	/// <summary>
	/// �n���h�����Z�b�g����
	/// </summary>
	/// <param name="handle">���f���n���h��</param>
	void SetHandle(int handle);

	virtual void Init() abstract;

	virtual void Update() abstract;

	void Draw();

	/// <summary>
	/// ���݂��Ă��邩�ǂ������擾����
	/// </summary>
	/// <returns>���݂��Ă���Ȃ�true</returns>
	bool IsExist() const { return m_isExist; }

	virtual void Final();

	// ���Ƃ��Ԃ���Ȃ��悤�ɂ���̂ŋ����
	virtual void OnCollide(std::shared_ptr<Collidable> collider) override {};

protected:

	bool m_isExist;

	int m_alpha;

	int m_handle;
};

