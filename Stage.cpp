#include "Stage.h"
#include "Physics.h"

namespace
{
	const MyEngine::Vector3 kStagePos(0,-50,0);
}

Stage::Stage()
{
	m_modelHandle = MV1LoadModel("data/model/protoStage.mv1");
}

Stage::~Stage()
{

}

void Stage::Init()
{
	m_pos = kStagePos;

	MV1SetPosition(m_modelHandle,m_pos.CastVECTOR());

	MV1SetScale(m_modelHandle,VGet(0.1f, 0.1f, 0.1f));

	MV1SetupCollInfo(m_modelHandle);

	Physics::GetInstance().SetStage(m_modelHandle);

}

void Stage::Draw()
{
	MV1DrawModel(m_modelHandle);
}

void Stage::Final()
{
	MV1DeleteModel(m_modelHandle);
	Physics::GetInstance().DeleteStage();
}
