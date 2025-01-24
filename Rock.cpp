#include "Rock.h"
#include "DxLib.h"
#include "SphereColliderData.h"

Rock::Rock():
	Collidable(ObjectTag::kStageObject,ColliderData::Kind::kSphere),
	m_alpha(255),
	m_handle(-1)
{
	auto sphereData = std::dynamic_pointer_cast<SphereColliderData>(m_pColData);
	sphereData->m_radius = 1.0f;
}

Rock::~Rock()
{

}

void Rock::Init(int handle)
{
	m_handle = MV1DuplicateModel(handle);
	Collidable::Init();
}

void Rock::Update()
{
	
}

void Rock::Draw()
{

}

void Rock::End()
{
	Collidable::Final();
}

void Rock::SetBumpPosAndCharacterPos(MyEngine::Vector3 bumpPos, MyEngine::Vector3 characterPos)
{
	m_rigidbody.SetPos(bumpPos);
	m_rigidbody.SetPos(characterPos);
}
