#pragma once
#include "ObjectTag.h"
#include "ColliderData.h"
#include "Rigidbody.h"
#include <memory>
class Physics;
class Collidable abstract : public std::enable_shared_from_this<Collidable>
{
public:
	Collidable(ObjectTag tag, ColliderData::Kind colKind);
	virtual ~Collidable();
	virtual void Init(std::shared_ptr<Physics> physics);
	virtual void Final(std::shared_ptr<Physics> physics);

	ObjectTag GetTag() const { return m_tag; }

	virtual void OnCollide(std::shared_ptr<Collidable> collider) abstract;

protected:
	//物理データ
	Rigidbody m_rigidbody;
	//当たり判定データ
	std::shared_ptr<ColliderData> m_pColData;
private:
	std::shared_ptr<ColliderData> CreateColliderData(ColliderData::Kind kind);

	ObjectTag m_tag;

	MyEngine::Vector3 m_nextPos;
	//PhysicsがCollidableを自由に管理できるように
	friend Physics;
};

