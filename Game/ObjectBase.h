#pragma once
#include "Collidable.h"

class ObjectBase : public Collidable
{
public:
	ObjectBase(ColliderData::Kind colKind);
	virtual ~ObjectBase();

	/// <summary>
	/// ハンドルをセットする
	/// </summary>
	/// <param name="handle">モデルハンドル</param>
	void SetHandle(int handle);

	virtual void Init() abstract;

	virtual void Update() abstract;

	void Draw();

	/// <summary>
	/// 存在しているかどうかを取得する
	/// </summary>
	/// <returns>存在しているならtrue</returns>
	bool IsExist() const { return m_isExist; }

	virtual void Final();

	// 何ともぶつからないようにするので空実装
	virtual void OnCollide(std::shared_ptr<Collidable> collider) override {};

protected:

	bool m_isExist;

	int m_alpha;

	int m_handle;
};

