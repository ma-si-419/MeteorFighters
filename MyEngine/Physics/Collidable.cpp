#include "Collidable.h"
#include "CapsuleColliderData.h"
#include "SphereColliderData.h"
#include "Physics.h"
#include <cassert>

Collidable::Collidable(ObjectTag tag, ColliderData::Kind colKind) :
	m_pColData(nullptr),
	m_tag(tag),
	m_isTrigger(false),
	m_isMove(false)
{
	m_pColData = CreateColliderData(colKind);
}

Collidable::~Collidable()
{
}

void Collidable::Init()
{
	//物理情報登録
	Physics::GetInstance().Entry(shared_from_this());
}

void Collidable::Final()
{
	//物理情報解除
	Physics::GetInstance().Exit(shared_from_this());
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
		return std::make_shared<SphereColliderData>();
	}
	else
	{
		return nullptr;
	}
}
