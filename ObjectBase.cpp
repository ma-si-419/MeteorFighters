#include "ObjectBase.h"
#include "SphereColliderData.h"
#include "CapsuleColliderData.h"
#include "DxLib.h"

ObjectBase::ObjectBase(ColliderData::Kind colKind) :
	Collidable(ObjectTag::kStageObject, colKind),
	m_handle(-1),
	m_alpha(255),
	m_isExist(true)
{
	if (colKind == ColliderData::Kind::kSphere)
	{
		auto sphereData = std::dynamic_pointer_cast<SphereColliderData>(m_pColData);
		sphereData->m_radius = 1.0f;
	}
	else if (colKind == ColliderData::Kind::kCapsule)
	{
		auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(m_pColData);
		capsuleData->m_radius = 1.0f;
		capsuleData->m_lange = MyEngine::Vector3(1.0f,1.0f,1.0f);
	}
}

ObjectBase::~ObjectBase()
{
}

void ObjectBase::SetHandle(int handle)
{
	m_handle = MV1DuplicateModel(handle);
}

void ObjectBase::Draw()
{
	MV1SetPosition(m_handle, m_rigidbody.GetPos().CastVECTOR());

	//アルファ値の設定
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_alpha);

	MV1DrawModel(m_handle);

	//元に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}

void ObjectBase::Final()
{
	MV1DeleteModel(m_handle);
	Collidable::Final();
}
