#include "Collidable.h"
#include "CapsuleColliderData.h"
#include "SphereColliderData.h"
#include "Physics.h"
#include <cassert>

Collidable::Collidable(ObjectTag tag, ColliderData::Kind colKind) :
	m_pColData(nullptr),
	m_tag(tag)
{
	m_pColData = CreateColliderData(colKind);
}

Collidable::~Collidable()
{
}

void Collidable::Init(std::shared_ptr<Physics> physics)
{
	//物理情報登録
	physics->Entry(shared_from_this());
}

void Collidable::Final(std::shared_ptr<Physics> physics)
{
	//物理情報解除
	physics->Exit(shared_from_this());
}

void Collidable::OnCollide(std::shared_ptr<Collidable> collider)
{
}

std::shared_ptr<ColliderData> Collidable::CreateColliderData(ColliderData::Kind kind)
{
	if (m_pColData != nullptr)
	{
		assert(0 && "colliderDataはすでに作られています");
		return m_pColData;
	}
	if (kind == ColliderData::Kind::kCapsule)
	{
		//カプセルコライダーの情報を入れる
		return std::make_shared<CapsuleColliderData>();
	}
	else if (kind == ColliderData::Kind::kSphere)
	{
		//スフィアコライダーの情報を入れる
		return std::make_shared<CapsuleColliderData>();
	}
	else
	{
		return nullptr;
	}
}
