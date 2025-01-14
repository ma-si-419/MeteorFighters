#include "Stage.h"
#include "Physics.h"

namespace
{
	const VECTOR kStagePos = VGet(0, -50, 0);

	const VECTOR kSkyDomeScale = VGet(10.0f, 10.0f, 10.0f);

	const VECTOR kStageScale = VGet(0.1f, 0.1f, 0.1f);

	constexpr float kSkyDomeRotaSpeed = 0.001f;
}

Stage::Stage():
	m_skyDomeHandle(-1),
	m_stageHandle(-1)
{
	m_stagePath = "data/model/Stage.mv1";
	m_skyDomePath = "data/model/Dome.mv1";
}

Stage::~Stage()
{

}

void Stage::Init()
{

}

void Stage::Update()
{
	auto rota = MV1GetRotationXYZ(m_skyDomeHandle);

	rota.y += kSkyDomeRotaSpeed;

	MV1SetRotationXYZ(m_skyDomeHandle,rota);

}

void Stage::Draw()
{
	MV1DrawModel(m_skyDomeHandle);
	MV1DrawModel(m_stageHandle);
}

void Stage::Final()
{
	MV1DeleteModel(m_skyDomeHandle);
	MV1DeleteModel(m_stageHandle);
	Physics::GetInstance().DeleteStage();
}

void Stage::SetStageModelHandle(int handle)
{
	m_stageHandle = handle;

	//ステージのモデル設定
	MV1SetPosition(m_stageHandle, kStagePos);
	MV1SetScale(m_stageHandle, kStageScale);
	MV1SetupCollInfo(m_stageHandle);

	Physics::GetInstance().SetStage(m_stageHandle);
}

void Stage::SetSkyDomeModelHandle(int handle)
{
	m_skyDomeHandle = handle;

	//スカイドームの設定
	MV1SetPosition(m_skyDomeHandle, kStagePos);
	MV1SetScale(m_skyDomeHandle, kSkyDomeScale);
}
